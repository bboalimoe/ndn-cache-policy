/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Ilya Moiseenko <iliamo@cs.ucla.edu>
 */

#include "ndn-consumer.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"

#include "ns3/ndn-app-face.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndn-data.h"
#include "ns3/ndnSIM/utils/ndn-fw-hop-count-tag.h"
#include "ns3/ndnSIM/utils/ndn-data-save-tag.h"

#include "ns3/ndnSIM/utils/ndn-rtt-mean-deviation.h"

#include <boost/ref.hpp>

#include "ns3/names.h"

NS_LOG_COMPONENT_DEFINE ("ndn.Consumer");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (Consumer);

TypeId
Consumer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ndn::Consumer")
    .SetGroupName ("Ndn")
    .SetParent<App> ()
    .AddAttribute ("StartSeq", "Initial sequence number",
                   IntegerValue (0),
                   MakeIntegerAccessor(&Consumer::m_seq),
                   MakeIntegerChecker<int32_t>())

    .AddAttribute ("Prefix","Name of the Interest",
                   StringValue ("/"),
                   MakeNameAccessor (&Consumer::m_interestName),
                   MakeNameChecker ())
    .AddAttribute ("LifeTime", "LifeTime for interest packet",
                   StringValue ("2s"),
                   MakeTimeAccessor (&Consumer::m_interestLifeTime),
                   MakeTimeChecker ())

    .AddAttribute ("RetxTimer",
                   "Timeout defining how frequent retransmission timeouts should be checked",
                   StringValue ("50ms"),
                   MakeTimeAccessor (&Consumer::GetRetxTimer, &Consumer::SetRetxTimer),
                   MakeTimeChecker ())

    .AddTraceSource ("LastRetransmittedInterestDataDelay", "Delay between last retransmitted Interest and received Data",
                     MakeTraceSourceAccessor (&Consumer::m_lastRetransmittedInterestDataDelay))

    .AddTraceSource ("FirstInterestDataDelay", "Delay between first transmitted Interest and received Data",
                     MakeTraceSourceAccessor (&Consumer::m_firstInterestDataDelay))
    ;

  return tid;
}

Consumer::Consumer ()
  : m_rand (0, std::numeric_limits<uint32_t>::max ())
  , m_seq (0)
  , m_seqMax (0) // don't request anything
{
  NS_LOG_FUNCTION_NOARGS ();

  m_rtt = CreateObject<RttMeanDeviation> ();

  for (int i = 0 ; i<=4 ; i++)
  {
  	Consumer::TotalInterestSendOutCounts[i] = 0;
  }

}

void
Consumer::SetRetxTimer (Time retxTimer)
{
  m_retxTimer = retxTimer;
  if (m_retxEvent.IsRunning ())
    {
      // m_retxEvent.Cancel (); // cancel any scheduled cleanup events
      Simulator::Remove (m_retxEvent); // slower, but better for memory
    }

  // schedule even with new timeout
  m_retxEvent = Simulator::Schedule (m_retxTimer,
                                     &Consumer::CheckRetxTimeout, this);
}

Time
Consumer::GetRetxTimer () const
{
  return m_retxTimer;
}

void
Consumer::CheckRetxTimeout ()
{
  Time now = Simulator::Now ();

  Time rto = m_rtt->RetransmitTimeout ();
  // NS_LOG_DEBUG ("Current RTO: " << rto.ToDouble (Time::S) << "s");

  while (!m_seqTimeouts.empty ())
    {
      SeqTimeoutsContainer::index<i_timestamp>::type::iterator entry =
        m_seqTimeouts.get<i_timestamp> ().begin ();
      if (entry->time + rto <= now) // timeout expired?
        {
          uint32_t seqNo = entry->seq;
          m_seqTimeouts.get<i_timestamp> ().erase (entry);
          OnTimeout (seqNo);
        }
      else
        break; // nothing else to do. All later packets need not be retransmitted
    }

  m_retxEvent = Simulator::Schedule (m_retxTimer,
                                     &Consumer::CheckRetxTimeout, this);
}

// Application Methods
void
Consumer::StartApplication () // Called at time specified by Start
{
  NS_LOG_FUNCTION_NOARGS ();

  // do base stuff
  App::StartApplication ();

  ScheduleNextPacket ();
bool isStart = true;
 for(int i = 0 ;i <= 4 ; i++)
 {
  if(TotalInterestSendOutCounts[i] != 0) isStart = false;
 }
 if(isStart)
  {
	  Simulator::Schedule( Seconds(5),  &Consumer::PrintSendInterests  ,Seconds(5) ) ;
	  Simulator::Schedule(Seconds(5), &Consumer::PrintHopCount , Seconds(5) ) ;
  }

}

void
Consumer::StopApplication () // Called at time specified by Stop
{
  NS_LOG_FUNCTION_NOARGS ();

  // cancel periodic packet generation
  Simulator::Cancel (m_sendEvent);

  // cleanup base stuff
  App::StopApplication ();
}

void
Consumer::SendPacket ()
{
  if (!m_active) return;

  NS_LOG_FUNCTION_NOARGS ();

  uint32_t seq=std::numeric_limits<uint32_t>::max (); //invalid

  while (m_retxSeqs.size ())
    {
      seq = *m_retxSeqs.begin ();
      m_retxSeqs.erase (m_retxSeqs.begin ());
      break;
    }

  if (seq == std::numeric_limits<uint32_t>::max ())
    {
      if (m_seqMax != std::numeric_limits<uint32_t>::max ())
        {
          if (m_seq >= m_seqMax)
            {
              return; // we are totally done
            }
        }

      seq = m_seq++;
    }

  //
  Ptr<Name> nameWithSequence = Create<Name> (m_interestName);
  nameWithSequence->appendSeqNum (seq);
  //

  Ptr<Interest> interest = Create<Interest> ();
  interest->SetNonce               (m_rand.GetValue ());
  interest->SetName                (nameWithSequence);

  //std::cout << "前缀为" << nameWithSequence->getPrefix(2, 0) << std::endl;

  interest->SetInterestLifetime    (m_interestLifeTime);

  // NS_LOG_INFO ("Requesting Interest: \n" << *interest);
  NS_LOG_INFO ("> Interest for " << seq);

 WillSendOutInterest (seq);
 ////hengheng
 if(m_interestName.size() >= 2)
 {
 DataSaveTag dataSaveTag ;
 std::string str_temp[5] = {"0x1", "0x2", "0x3","0x4","0x5"} ;
 for (int i = 0 ; i <=4 ; i++)
	 {
	 	 if (m_interestName[1].toUri() == str_temp[i])   \
			dataSaveTag.Set(i+1);
	 	TotalInterestSendOutCounts[i] ++ ;
	 }
 interest->GetPayload ()->AddPacketTag (dataSaveTag);

 ///// hengheng
 }
  FwHopCountTag hopCountTag;
  interest->GetPayload ()->AddPacketTag (hopCountTag);


  m_transmittedInterests (interest, this, m_face);
  m_face->ReceiveInterest (interest);     //传给自己的fw oninterest 来看是否local满足自己,不满足
                   	   	   	   	   	   	   	   	   	   	   	   	   	   	   // propagate 出去

   //std::cout << "consumer sended packet   " << dataSaveTag.Get() <<std::endl;
//hengheng 这个是调用子类的该函数,因为consumer类不是一个impl类
  ScheduleNextPacket ();  ///

}
///////////////////////////////////////////////////
//          Process incoming packets             //
///////////////////////////////////////////////////


void
Consumer::OnData (Ptr<const Data> data)
{
  if (!m_active) return;

  App::OnData (data); // tracing inside

  NS_LOG_FUNCTION (this << data);

  // NS_LOG_INFO ("Received content object: " << boost::cref(*data));

  uint32_t seq = data->GetName ().get (-1).toSeqNum ();
  NS_LOG_INFO ("< DATA for " << seq);

  int hopCount = -1;
  FwHopCountTag hopCountTag;
  DataSaveTag dataSaveTag ;
  if (data->GetPayload ()->PeekPacketTag (hopCountTag))
    {
      hopCount = hopCountTag.Get ();
    }

  if (data->GetPayload ()->PeekPacketTag (dataSaveTag))
    {
	  HopCntTotalByTag[dataSaveTag.Get()-1] += hopCount ;
	  InterestBackTotalByTag[dataSaveTag.Get()-1] ++ ;
	  NS_LOG_DEBUG( "Got the Data no.  is     "<<  dataSaveTag.Get () );
    }

  SeqTimeoutsContainer::iterator entry = m_seqLastDelay.find (seq);
  if (entry != m_seqLastDelay.end ())
    {
      m_lastRetransmittedInterestDataDelay (this, seq, Simulator::Now () - entry->time, hopCount);
    }

  entry = m_seqFullDelay.find (seq);
  if (entry != m_seqFullDelay.end ())
    {
      m_firstInterestDataDelay (this, seq, Simulator::Now () - entry->time, m_seqRetxCounts[seq], hopCount);
    }

  m_seqRetxCounts.erase (seq);
  m_seqFullDelay.erase (seq);
  m_seqLastDelay.erase (seq);

  m_seqTimeouts.erase (seq);
  m_retxSeqs.erase (seq);

  m_rtt->AckSeq (SequenceNumber32 (seq));
}

void
Consumer::OnNack (Ptr<const Interest> interest)
{
  if (!m_active) return;

  App::OnNack (interest); // tracing inside

  // NS_LOG_DEBUG ("Nack type: " << interest->GetNack ());

  // NS_LOG_FUNCTION (interest->GetName ());

  // NS_LOG_INFO ("Received NACK: " << boost::cref(*interest));
  uint32_t seq = interest->GetName ().get (-1).toSeqNum ();  // name最后一个component是seqno
  NS_LOG_INFO ("< NACK for " << seq);
  // std::cout << Simulator::Now ().ToDouble (Time::S) << "s -> " << "NACK for " << seq << "\n";

  // put in the queue of interests to be retransmitted
  // NS_LOG_INFO ("Before: " << m_retxSeqs.size ());
  m_retxSeqs.insert (seq);
  // NS_LOG_INFO ("After: " << m_retxSeqs.size ());

  m_seqTimeouts.erase (seq);

  m_rtt->IncreaseMultiplier ();             // Double the next RTO ??
  ScheduleNextPacket ();
}

void
Consumer::OnTimeout (uint32_t sequenceNumber)
{
  NS_LOG_FUNCTION (sequenceNumber);
  // std::cout << Simulator::Now () << ", TO: " << sequenceNumber << ", current RTO: " << m_rtt->RetransmitTimeout ().ToDouble (Time::S) << "s\n";

  m_rtt->IncreaseMultiplier ();             // Double the next RTO
  m_rtt->SentSeq (SequenceNumber32 (sequenceNumber), 1); // make sure to disable RTT calculation for this sample
  m_retxSeqs.insert (sequenceNumber);
  ScheduleNextPacket ();
}

void
Consumer::WillSendOutInterest (uint32_t sequenceNumber)
{
  NS_LOG_DEBUG ("Trying to add " << sequenceNumber << " with " << Simulator::Now () << ". already " << m_seqTimeouts.size () << " items");

  m_seqTimeouts.insert (SeqTimeout (sequenceNumber, Simulator::Now ()));
  m_seqFullDelay.insert (SeqTimeout (sequenceNumber, Simulator::Now ()));

  m_seqLastDelay.erase (sequenceNumber);
  m_seqLastDelay.insert (SeqTimeout (sequenceNumber, Simulator::Now ()));

  m_seqRetxCounts[sequenceNumber] ++;

  m_rtt->SentSeq (SequenceNumber32 (sequenceNumber), 1);
}

std::vector<int> Consumer::HopCntTotalByTag (5);
std::vector<int> Consumer::InterestBackTotalByTag(5) ;
std::vector<uint32_t> Consumer::TotalInterestSendOutCounts(5) ;
Time Consumer::TrackTime1( Seconds(1000000) ) ;
Time Consumer::TrackTime2( Seconds(1000000) ) ;

void
Consumer::PrintSendInterests(Time t)
{

	if(TrackTime1 == Simulator::Now() ) return;

	    TrackTime1 = Simulator::Now();
	 Simulator::Schedule( Seconds(5),  &Consumer::PrintSendInterests, t  ) ;

	 std::cout << Simulator::Now() << "\t"\
		    	         		 <<"SendInterests(发送的请求总数)" << "\t" ;
		    	   for (int i = 0; i<= 4 ; i++)
		    	   {
		    		   std::cout <<"\t" << TotalInterestSendOutCounts[i] << "\t" ;
		    	   }
		    	   std::cout << std::endl;
}
void
Consumer::PrintHopCount(Time t)
{

	if(TrackTime2 == Simulator::Now() ) return;

     TrackTime2 = Simulator::Now();

     Simulator::Schedule(Seconds(5), &Consumer::PrintHopCount,t  ) ;
	 std::cout << Simulator::Now() << "\t"\
	    	         		 <<"HopcountTotal(请求被响应所需要的总跳数)" << "\t" ;
	    	   for (int i = 0; i<= 4 ; i++)
	    	   {
	    		   std::cout <<"\t" << HopCntTotalByTag [i] << "\t" ;
	    	   }
	    	   std::cout << std::endl;

	   std::cout << Simulator::Now() << "\t"\
								 <<"BackData(传回的内容包总数)" << "\t" ;
				   for (int i = 0; i<= 4 ; i++)
				   {
					   std::cout <<"\t" << InterestBackTotalByTag [i] << "\t" ;
				   }
				   std::cout << std::endl;


}

} // namespace ndn
} // namespace ns3
