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
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_CONTENT_STORE_IMPL_H_
#define NDN_CONTENT_STORE_IMPL_H_

#include "ndn-content-store.h"
#include "ns3/packet.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndn-data.h"
#include <boost/foreach.hpp>
#include <vector>

#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

#include "../../utils/trie/trie-with-policy.h"
#include "../../model/hit-trace-table.h"


namespace ns3 {
namespace ndn {

namespace cs {

static  Ptr<typename ns3::ndn::HitTraceTable> s_htt = Create<typename ns3::ndn::HitTraceTable>();

/**
 * @ingroup ndn-cs
 * @brief Cache entry implementation with additional references to the base container
 */
template<class CS>
class EntryImpl : public Entry
{
public:

	///base_type   --->Entry
  typedef Entry base_type;

public:
  EntryImpl (Ptr<ContentStore> cs, Ptr<const Data> data)
    : Entry (cs, data)
    , item_ (0)
  {
  }


  /////
  ////  super  --> line 89     , iterator 就是指向 trie的pointer 就是一个链表的一个位置,
  void
  SetTrie (typename CS::super::iterator item)
  {
    item_ = item;
  }


  //CS::super::iterator 是trie with policy 的trie *
  typename CS::super::iterator to_iterator () { return item_; }
  typename CS::super::const_iterator to_iterator () const { return item_; }

private:
  typename CS::super::iterator item_;
};



/**
 * @ingroup ndn-cs
 * @brief Base implementation of NDN content store
 */
template<class Policy>
class ContentStoreImpl : public ContentStore,
                         protected ndnSIM::trie_with_policy< Name,
                                                             ndnSIM::smart_pointer_payload_traits< EntryImpl< ContentStoreImpl< Policy > >, Entry >,
                                                             Policy >
{
public:

	/////
	////
	/////   super       smart_pointer_paylaod ---> ptr<payload>
  typedef ndnSIM::trie_with_policy< Name,
                                    ndnSIM::smart_pointer_payload_traits< EntryImpl< ContentStoreImpl< Policy > >, Entry >,
                                    Policy > super;

  /////
  //////
//////   actually the entryimpl is the real entry !
  typedef EntryImpl< ContentStoreImpl< Policy > > entry;



  static TypeId
  GetTypeId ();

  ContentStoreImpl () {

	NS_LOG_DEBUG( "contentstoreimpl的构造函数" );

  };
  virtual ~ContentStoreImpl () { };

  // from ContentStore

  virtual inline Ptr<Data>
  Lookup (Ptr<const Interest> interest);

  virtual inline bool
  Add (Ptr<const Data> data);

  // virtual bool
  // Remove (Ptr<Interest> header);

  virtual inline void
  Print (std::ostream &os) const;

  virtual uint32_t
  GetSize () const;

  virtual Ptr<Entry>
  Begin ();

  virtual Ptr<Entry>
  End ();

  virtual Ptr<Entry>
  Next (Ptr<Entry>);

  const typename super::policy_container &
  GetPolicy () const { return super::getPolicy (); }

  typename super::policy_container &
  GetPolicy () { return super::getPolicy (); }
  
  //////// set the cs ratio
     void SetRatio (std::vector<float> &vect)
    {

    }

  ////////// get the cs ratio
    std::vector<float> GetRatio() const
    {
  	  /// 这是调用了 policy中的lfu-data-save-traits
    	std::vector<float> a(5) ;
       return a;

    }

  typename ns3::ndn::HitTraceTable*
    GetHtt()
    {
    	 return &(*s_htt);
    }

  void
  SetMaxSize (uint32_t maxSize);

  uint32_t
  GetMaxSize () const;

private:
  static LogComponent g_log; ///< @brief Logging variable

  /// @brief trace of for entry additions (fired every time entry is successfully added to the cache): first parameter is pointer to the CS entry
  TracedCallback< Ptr<const Entry> > m_didAddEntry;

};

//////////////////////////////////////////
////////// Implementation ////////////////
//////////////////////////////////////////


template<class Policy>
LogComponent ContentStoreImpl< Policy >::g_log = LogComponent (("ndn.cs." + Policy::GetName ()).c_str ());


template<class Policy>
TypeId
ContentStoreImpl< Policy >::GetTypeId ()
{
  static TypeId tid = TypeId (("ns3::ndn::cs::"+Policy::GetName ()).c_str ())
    .SetGroupName ("Ndn")
    .SetParent<ContentStore> ()
    .AddConstructor< ContentStoreImpl< Policy > > ()
    .AddAttribute ("MaxSize",
                   "Set maximum number of entries in ContentStore. If 0, limit is not enforced",
                   StringValue ("400"),
                   MakeUintegerAccessor (&ContentStoreImpl< Policy >::GetMaxSize,
                                         &ContentStoreImpl< Policy >::SetMaxSize),
                   MakeUintegerChecker<uint32_t> ())

    .AddTraceSource ("DidAddEntry", "Trace fired every time entry is successfully added to the cache",
                     MakeTraceSourceAccessor (&ContentStoreImpl< Policy >::m_didAddEntry))

    ;

  return tid;
}

//////
//////
/////// is a functor !
struct isNotExcluded
{
  inline
  isNotExcluded (const Exclude &exclude)
    : m_exclude (exclude)
  {
  }
  
  bool
  operator () (const name::Component &comp) const
  {
    return !m_exclude.isExcluded (comp);
  }

private:
  const Exclude &m_exclude;
};

//////
template<class Policy>
Ptr<Data>
ContentStoreImpl<Policy>::Lookup (Ptr<const Interest> interest)
{
  NS_LOG_FUNCTION (this << interest->GetName ());

  ////
  ////
  ///// line 94    super ---> trie with policy
  typename super::const_iterator node; //node -- > type is trie*


  if (interest->GetExclude () == 0)
    {
      node = this->deepest_prefix_match (interest->GetName ());
      //trie
    }
  else
    {
	  //////
	  //////  就是找到一个trie node后,看是否需要再找其子节点...~~~,
      node = this->deepest_prefix_match_if_next_level (interest->GetName (),
                                                       isNotExcluded (*interest->GetExclude ()));
    }






  if (node != this->end ())
  {
      this->m_cacheHitsTrace (interest, node->payload ()->GetData () ); //问题就在这儿.........二....
// hengheng
    	this->m_cachehits_tag(interest, node->payload ()->GetData () );  //
    	NS_LOG_DEBUG("有没有从这儿走的？？ " ) ;

       Ptr<Data> copy = Create<Data> (*node->payload ()->GetData ());

      //这里有个removealltags,记住先取tag,再removeall
      ///因为这里的countTags可能是之前的interest加的,所以要去掉.
      ConstCast<Packet> (copy->GetPayload ())->RemoveAllPacketTags ();
      return copy;
    }
  else
	{

	       this->m_cacheMissesTrace (interest);
	       //hengheng
	       this->m_cachemisses_tag(interest);

		  //DataSaveTag* tag_ptr(&tag);

		 // this->m_cachemisses_tag(interest, node->payload()->GetData());
	       // m_ininterest - m_cachehits = m_cachemisses


	  return 0;
	}
}

template<class Policy>
bool
ContentStoreImpl<Policy>::Add (Ptr<const Data> data)
{
  NS_LOG_FUNCTION (this << data->GetName ());

  //std::cout << std::endl<< std::endl<< std::endl<< "hengheng有没有从这儿走的？？" << std::endl<< std::endl<< std::endl<< std::endl;

  Ptr< entry > newEntry = Create< entry > (this, data);
  //////
  /////// trie with policy is the super !!!!!
  //////// super::iterator is the parent_trie* like iterator

  std::pair< typename super::iterator, bool > result = super::insert (data->GetName (), newEntry);

  if (result.first != super::end ())
    {
      if (result.second)
        {
          newEntry->SetTrie (result.first);   ///这里trie 和 entry是个双向接口,相当于绑定了

          m_didAddEntry (newEntry);
          return true;
        }
      else
        {
          // should we do anything?
          // update payload? add new payload?
          return false;
        }
    }
  else
    return false; // cannot insert entry
}

template<class Policy>
void
ContentStoreImpl<Policy>::Print (std::ostream &os) const
{
  for (typename super::policy_container::const_iterator item = this->getPolicy ().begin ();
       item != this->getPolicy ().end ();
       item++)
    {
      os << item->payload ()->GetName () << std::endl;
    }
}

template<class Policy>
void
ContentStoreImpl<Policy>::SetMaxSize (uint32_t maxSize)
{

  this->getPolicy ().set_max_size (maxSize);
}

template<class Policy>
uint32_t
ContentStoreImpl<Policy>::GetMaxSize () const
{
	//std::cout << "这是contentStoreimpl的GetmaxSize" <<std::endl;
  return this->getPolicy ().get_max_size ();
}


template<class Policy>
uint32_t
ContentStoreImpl<Policy>::GetSize () const
{
  return this->getPolicy ().size ();
}

template<class Policy>
Ptr<Entry>
ContentStoreImpl<Policy>::Begin ()
{
  typename super::parent_trie::recursive_iterator item (super::getTrie ()), end (0);
  for (; item != end; item++)
    {
      if (item->payload () == 0) continue;
      break;
    }

  if (item == end)
    return End ();
  else
    return item->payload ();
}

template<class Policy>
Ptr<Entry>
ContentStoreImpl<Policy>::End ()
{
  return 0;
}

template<class Policy>
Ptr<Entry>
ContentStoreImpl<Policy>::Next (Ptr<Entry> from)
{
  if (from == 0) return 0;

  typename super::parent_trie::recursive_iterator
    item (*StaticCast< entry > (from)->to_iterator ()),
    end (0);

  for (item++; item != end; item++)
    {
      if (item->payload () == 0) continue;
      break;
    }

  if (item == end)
    return End ();
  else
    return item->payload ();
}


} // namespace cs
} // namespace ndn
} // namespace ns3

#endif // NDN_CONTENT_STORE_IMPL_H_
