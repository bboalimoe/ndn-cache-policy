

#include "ndn-cs-tag-tracer.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/config.h"
#include "ns3/names.h"
#include "ns3/callback.h"

#include "ns3/ndn-app.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndn-data.h"
#include "ns3/ndn-content-store.h"
#include "ns3/simulator.h"
#include "ns3/node-list.h"
#include "ns3/log.h"


#include <boost/lexical_cast.hpp>

#include <fstream>

NS_LOG_COMPONENT_DEFINE ("ndn.CsTracerByTag");

using namespace std;

namespace ns3 {
namespace ndn {

static std::list< boost::tuple< boost::shared_ptr<std::ostream>, std::list<Ptr<CsTracerByTag> > > > g_tracers;

template<class T>
static inline void
NullDeleter (T *ptr)
{
}

void
CsTracerByTag::Destroy ()
{
  g_tracers.clear ();
}

void
CsTracerByTag::InstallAll (const std::string &file, Time averagingPeriod/* = Seconds (0.5)*/)
{
  using namespace boost;
  using namespace std;

  std::list<Ptr<CsTracerByTag> > tracers;
  boost::shared_ptr<std::ostream> outputStream;
  if (file != "-")
    {
      boost::shared_ptr<std::ofstream> os (new std::ofstream ());
      os->open (file.c_str (), std::ios_base::out | std::ios_base::trunc);

      if (!os->is_open ())
        {
          NS_LOG_ERROR ("File " << file << " cannot be opened for writing. Tracing disabled");
          return;
        }

      outputStream = os;
    }
  else
    {
	  /// NullDeleter ??? what is it
      outputStream = boost::shared_ptr<std::ostream> (&std::cout, NullDeleter<std::ostream>);
    }

  for (NodeList::Iterator node = NodeList::Begin ();
       node != NodeList::End ();
       node++)
    {
      Ptr<CsTracerByTag> trace = Install (*node, outputStream, averagingPeriod);
      tracers.push_back (trace);
    }

  if (tracers.size () > 0)
    {
      // *m_l3RateTrace << "# "; // not necessary for R's read.table
      tracers.front ()->PrintHeader (*outputStream);
      *outputStream << "\n";
    }

  g_tracers.push_back (boost::make_tuple (outputStream, tracers));
}

void
CsTracerByTag::Install (const NodeContainer &nodes, const std::string &file, Time averagingPeriod/* = Seconds (0.5)*/)
{
  using namespace boost;
  using namespace std;

  std::list<Ptr<CsTracerByTag> > tracers;
  boost::shared_ptr<std::ostream> outputStream;
  if (file != "-")
    {
      boost::shared_ptr<std::ofstream> os (new std::ofstream ());
      os->open (file.c_str (), std::ios_base::out | std::ios_base::trunc);

      if (!os->is_open ())
        {
          NS_LOG_ERROR ("File " << file << " cannot be opened for writing. Tracing disabled");
          return;
        }

      outputStream = os;
    }
  else
    {
      outputStream = boost::shared_ptr<std::ostream> (&std::cout, NullDeleter<std::ostream>);
    }

  for (NodeContainer::Iterator node = nodes.Begin ();
       node != nodes.End ();
       node++)
    {
      Ptr<CsTracerByTag> trace = Install (*node, outputStream, averagingPeriod);
      tracers.push_back (trace);
    }

  if (tracers.size () > 0)
    {
      // *m_l3RateTrace << "# "; // not necessary for R's read.table
      tracers.front ()->PrintHeader (*outputStream);
      *outputStream << "\n";
    }

  g_tracers.push_back (boost::make_tuple (outputStream, tracers));
}

void
CsTracerByTag::Install (Ptr<Node> node, const std::string &file, Time averagingPeriod/* = Seconds (0.5)*/)
{
  using namespace boost;
  using namespace std;

  std::list<Ptr<CsTracerByTag> > tracers;
  boost::shared_ptr<std::ostream> outputStream;
  if (file != "-")
    {
      boost::shared_ptr<std::ofstream> os (new std::ofstream ());
      os->open (file.c_str (), std::ios_base::out | std::ios_base::trunc);

      if (!os->is_open ())
        {
          NS_LOG_ERROR ("File " << file << " cannot be opened for writing. Tracing disabled");
          return;
        }

      outputStream = os;
    }
  else
    {
      outputStream = boost::shared_ptr<std::ostream> (&std::cout, NullDeleter<std::ostream>);
    }

  Ptr<CsTracerByTag> trace = Install (node, outputStream, averagingPeriod);
  tracers.push_back (trace);

  if (tracers.size () > 0)
    {
      // *m_l3RateTrace << "# "; // not necessary for R's read.table
      tracers.front ()->PrintHeader (*outputStream);
      *outputStream << "\n";
    }

  g_tracers.push_back (boost::make_tuple (outputStream, tracers));
}


Ptr<CsTracerByTag>
CsTracerByTag::Install (Ptr<Node> node,
                   boost::shared_ptr<std::ostream> outputStream,
                   Time averagingPeriod/* = Seconds (0.5)*/)
{
  NS_LOG_DEBUG ("Node: " << node->GetId ());

  Ptr<CsTracerByTag> trace = Create<CsTracerByTag> (outputStream, node);
  trace->SetAveragingPeriod (averagingPeriod);

  return trace;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CsTracerByTag::CsTracerByTag (boost::shared_ptr<std::ostream> os, Ptr<Node> node)
: m_nodePtr (node)
, m_os (os)
{
  m_node = boost::lexical_cast<string> (m_nodePtr->GetId ());
  ////this is the fuck !!!
  Connect ();

  string name = Names::FindName (node);
  if (!name.empty ())
    {
      m_node = name;
    }
}

CsTracerByTag::CsTracerByTag (boost::shared_ptr<std::ostream> os, const std::string &node)
: m_node (node)
, m_os (os)
{
  Connect ();
}

CsTracerByTag::~CsTracerByTag ()
{
};


void
CsTracerByTag::Connect ()
{
  Ptr<ContentStore> cs = m_nodePtr->GetObject<ContentStore> ();
  cs->TraceConnectWithoutContext ("CacheHits",   MakeCallback (&CsTracerByTag::CacheHits,   this));
  cs->TraceConnectWithoutContext ("CacheMisses", MakeCallback (&CsTracerByTag::CacheMisses, this));

  Reset ();
}


void
CsTracerByTag::SetAveragingPeriod (const Time &period)
{
  m_period = period;
  m_printEvent.Cancel ();
  m_printEvent = Simulator::Schedule (m_period, &CsTracerByTag::PeriodicPrinter, this);
}

void
CsTracerByTag::PeriodicPrinter ()
{

  Print (*m_os);
  //Reset ();     计算总的数量
  m_printEvent = Simulator::Schedule (m_period, &CsTracerByTag::PeriodicPrinter, this);

}

void
CsTracerByTag::PrintHeader (std::ostream &os) const
{
  os << "Time" << "\t"

     << "Node" << "\t"
     << "Node" << "\t"
     << "Node" << "\t"
     << "Node" << "\t"
     << "Node" << "\t"
     << "Type" << "\t"
     << "Tag 0x1 Packets" << "\t"
     << "Tag 0x2 Packets" << "\t"
     << "Tag 0x3 Packets" << "\t"
     << "Tag 0x4 Packets" << "\t"
     << "Tag 0x5 Packets" << "\t" ;

}

void
CsTracerByTag::Reset ()
{
  m_stats.Reset();
}

#define PRINTER(printName, fieldName)           \
  os << time.ToDouble (Time::S) << "\t"         \
  << m_node << "\t"                             \
  << m_node << "\t"                             \
  << m_node << "\t"      							\
  << m_node << "\t"       						\
  << m_node << "\t"      							\
  << printName << "\t"                          \
  << (m_stats.fieldName[0]).int_tag   << "\n";
/*\
  << (m_stats.fieldName)[1]				\
  << (m_stats.fieldName)[2]				\
  << (m_stats.fieldName)[3]				\
  << (m_stats.fieldName)[4]				\
  */
  //<< "\n";



void
CsTracerByTag::Print (std::ostream &os) const
{
  Time time = Simulator::Now ();

  PRINTER ("CacheHitsVect",   m_cacheHits_vect);
  PRINTER ("CacheMissesVect", m_cacheMisses_vect);

}

void
CsTracerByTag::CacheHits (Ptr<const Interest> interest, Ptr<const Data> data)
{
/// list 0, 1, 2, 3, 4  --> tag 0x1, 0x2 ,0x3, 0x4, 0x5
  //m_stats.m_cacheHits ++;
	DataSaveTag tag;
if( data->GetPayload()->PeekPacketTag(tag) )
{
	int  tag2index = tag.Get() - 1;
	(this->m_stats.m_cacheHits_vect[tag2index]).int_tag++ ;
}

}

void
CsTracerByTag::CacheMisses (Ptr<const Interest> interest, Ptr<const Data> data)
{
  //m_stats.m_cacheMisses ++;
	DataSaveTag tag;
    if( data->GetPayload()->PeekPacketTag(tag) )
    {
		int  tag2index = tag.Get() - 1;
		(this->m_stats.m_cacheMisses_vect[tag2index]).int_tag++ ;
    }

}


} // namespace ndn
} // namespace ns3
