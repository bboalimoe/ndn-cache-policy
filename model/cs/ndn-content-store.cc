/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011,2012 University of California, Los Angeles
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
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *         Ilya Moiseenko <iliamo@cs.ucla.edu>
 *         
 */

#include "ndn-content-store.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/ndn-name.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndn-data.h"
#include "../../model/hit-trace-table.h"


//#include "ns3/ndnSIM/model/hit-trace-table.h"

NS_LOG_COMPONENT_DEFINE ("ndn.cs.ContentStore");

namespace ns3 {
namespace ndn {

 static  Ptr<typename ns3::ndn::HitTraceTable> v_htt = Create<typename ns3::ndn::HitTraceTable>();




NS_OBJECT_ENSURE_REGISTERED (ContentStore);

TypeId
ContentStore::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ndn::ContentStore")
    .SetGroupName ("Ndn")
    .SetParent<Object> ()

    .AddTraceSource ("CacheHits", "Trace called every time there is a cache hit",
                     MakeTraceSourceAccessor (&ContentStore::m_cacheHitsTrace))

    .AddTraceSource ("CacheMisses", "Trace called every time there is a cache miss",
                     MakeTraceSourceAccessor (&ContentStore::m_cacheMissesTrace))
     .AddTraceSource ("CacheHits_tag",  "cache hits count by tag",
                    					 MakeTraceSourceAccessor (&ContentStore::m_cachehits_tag))
     .AddTraceSource ("CacheMisses_tag",  "cache misses count by tag",
                    					 MakeTraceSourceAccessor (&ContentStore::m_cachemisses_tag))
    ;

  return tid;
}


ContentStore::~ContentStore () 
{
}

typename ns3::ndn::HitTraceTable*
ContentStore::GetHtt()
{
	 return &(*v_htt);
}



void ContentStore::SetRatio (std::vector<float> &vect)
{
	NS_LOG_DEBUG( "这是基类ContentStore的SetRatio" );
    return ;
}

////////// get the cs ratio
std::vector<float> ContentStore::GetRatio() const
{
	  /// 这是调用了 policy中的lfu-data-save-traits
	NS_LOG_DEBUG( "这是基类ContentStore的GetRatio" );
	std::vector<float> a(5) ;
    return a;
}

void
 ContentStore::SetMaxSize (uint32_t maxSize)
{
	NS_LOG_DEBUG(  "这是基类ContentStore的SetMaxSize" );
	return ;
	}

uint32_t
  ContentStore::GetMaxSize() const
{
	NS_LOG_DEBUG(  "这是基类ContentStore的GetMaxSize");
	uint32_t x = 10;
	return  x;
}


namespace cs {

//////////////////////////////////////////////////////////////////////

Entry::Entry (Ptr<ContentStore> cs, Ptr<const Data> data)
  : m_cs (cs)
  , m_data (data)
{
}

const Name&
Entry::GetName () const
{
  return m_data->GetName ();
}

Ptr<const Data>
Entry::GetData () const
{
  return m_data;
}

Ptr<ContentStore>
Entry::GetContentStore ()
{
  return m_cs;
}


} // namespace cs
} // namespace ndn
} // namespace ns3
