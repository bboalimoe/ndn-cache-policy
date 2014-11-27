/*
 * content-store-with-stats-inherit.h
 *
 *  Created on: 2014年9月17日
 *      Author: hengheng
 */

#ifndef NDN_CONTENT_STORE_WITH_STATS_H_
#define NDN_CONTENT_STORE_WITH_STATS_H_

#include "content-store-impl.h"

#include "../../utils/trie/multi-policy.h"
#include "custom-policies/lifetime-stats-policy.h"
#include "../../utils/ndn-data-save-tag.h"
#include <vector>
#include "ns3/vector.h"
#include "../../model/hit-trace-table.h"

namespace ns3 {
namespace ndn {
namespace cs {




// declare






/**
 * @ingroup ndn-cs
 * @brief Special content store realization that provides ability to track stats of CS operations
 */
template<class Policy>
class ContentStoreWithStatsinherit :
    public ContentStoreImpl< ndnSIM::multi_policy_traits< boost::mpl::vector2< Policy, ndnSIM::lifetime_stats_policy_traits > > >
{
public:
  typedef ContentStoreImpl< ndnSIM::multi_policy_traits< boost::mpl::vector2< Policy, ndnSIM::lifetime_stats_policy_traits > > > super;

  typedef typename super::policy_container::template index<1>::type lifetime_stats_container;
  //typedef typename super::policy_container::template index<1>::type  lfu-tag-container;


  ContentStoreWithStatsinherit ():m_htt()
  {
	  Ptr< HitTraceTable > p_htt = Create< HitTraceTable > (this);

	 // 原因在于构造函数里，还没有调用gettypeid，所以那几个trace的name还没有绑定到该typeid下，所以返回找不到

	  m_htt.SetContentStore(p_htt->GetContentStore());

    // connect traceback to the policy
    super::getPolicy ().template get<1> ().set_traced_callback (&m_willRemoveEntry);

    //// cs初始化时,将CacheHits绑定到htt的CacheHits函数上


    ////初始化m_htt的绑定cs
    //m_htt.m_cs = CreateObject<ContentStore>this;

    Simulator::Schedule (Seconds(5.0), &ns3::ndn::HitTraceTable::CalCsRatioByTag, &m_htt ,Seconds(5.0) );

  //  std::cout << Simulator::Now()  << std::endl ;
   // Ptr<ns3::Node> node = this->GetObject<ns3::Node> ();
   // std::cout <<"node " <<" 号    schedule has launched" << std::endl;


  }

 virtual  ~ContentStoreWithStatsinherit()
  {
	  /////// ha virtual destructor method
  }

  static TypeId
  GetTypeId ();

  virtual ns3::ndn::HitTraceTable *
   GetHtt()
  {
	//  std::cout << "这是contentimplwithstats的Hittracetable"<<std::endl;
	  return &m_htt;
  }

  virtual inline void
  Print (std::ostream &os) const;

  //////// set the cs ratio
   void SetRatio (std::vector<float> &vect)
  {
	  // std::cout << "这是contentstorewithstatsinherit的SET RATIO" << std::endl;
	   super::getPolicy ().template get<0> ().set_ratio(vect);

  }

////////// get the cs ratio
  std::vector<float> GetRatio() const
  {
	  /// 这是调用了 policy中的lfu-data-save-traits
	    std::vector<float> ratio = super::getPolicy ().template get<0> ().get_ratio();
        NS_LOG_DEBUG("这是contentstorewithstatsinherit的GetRatio");
       // std::cout << "这是contentstorewithstatsinherit的GetRatio" << std::endl;
  		return ratio;

  }

  /*
  void
    SetMaxSize (uint32_t maxSize)
  {
	  super::SetMaxSize(maxSize);
	  //SetMaxSize(maxSize);
  }

    uint32_t
    GetMaxSize () const
    {
    	//为什么一直调用错了！！
    	 std::cout << "这是contentstorewithstatsinherit的GetMaxSize"<<std::endl;
    	return super::GetMaxSize();
    	//   std::vector<float> ratio = super::getPolicy ().template get<0> ().get_ratio();
    	//return 100;
    }

*/
private:
  static LogComponent g_log; ///< @brief Logging variable
  HitTraceTable m_htt;  ///htt instance   转到contentstore中定义

  /// @brief trace of for entry removal: first parameter is pointer to the CS entry, second is how long entry was in the cache
  TracedCallback< Ptr<const Entry>, Time > m_willRemoveEntry;


};

//////////////////////////////////////////
////////// Implementation ////////////////
//////////////////////////////////////////


template<class Policy>
LogComponent
ContentStoreWithStatsinherit< Policy >::g_log = LogComponent (("ndn.cs.Statsinherit." + Policy::GetName ()).c_str ());


template<class Policy>
TypeId
ContentStoreWithStatsinherit< Policy >::GetTypeId ()
{
  static TypeId tid = TypeId (("ns3::ndn::cs::Statsinherit::"+Policy::GetName ()).c_str ())
    .SetGroupName ("Ndn")
    .SetParent<super> ()
    .template AddConstructor< ContentStoreWithStatsinherit< Policy > > ()

    .AddTraceSource ("WillRemoveEntry", "Trace called just before content store entry will be removed",
                     MakeTraceSourceAccessor (&ContentStoreWithStatsinherit< Policy >::m_willRemoveEntry))

      ;
  return tid;
}

/////


template<class Policy>
void
ContentStoreWithStatsinherit< Policy >::Print (std::ostream &os) const
{
  // const freshness_policy_container &freshness = this->getPolicy ().template get<freshness_policy_container> ();

  for (typename super::policy_container::const_iterator item = this->getPolicy ().begin ();
       item != this->getPolicy ().end ();
       item++)
    {
      Time alive = lifetime_stats_container::policy_base::get_time (&(*item)) - Simulator::Now ();
      os << item->payload ()->GetName () << "(alive: " << alive.ToDouble (Time::S) << "s)" << std::endl;
    }
}



} // namespace cs
} // namespace ndn
} // namespace ns3




#endif /* CONTENT_STORE_WITH_STATS_INHERIT_H_ */
