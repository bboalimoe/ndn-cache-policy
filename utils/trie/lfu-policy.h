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

#ifndef LFU_POLICY_H_
#define LFU_POLICY_H_

#include <boost/intrusive/options.hpp>
#include <boost/intrusive/set.hpp>

namespace ns3 {
namespace ndn {
namespace ndnSIM {

/**
 * @brief Traits for LFU replacement policy
 */
struct lfu_policy_traits
{



  /// @brief Name that can be used to identify the policy (for NS-3 object model and logging)
  static std::string GetName () { return "Lfu"; }


   ///
  ///   set_member_hook 是set的memberhook.....蛋疼
  /// 声明过后,该类就具备了 intrusive的特性了
  struct policy_hook_type : public boost::intrusive::set_member_hook<> { double frequency; };

  /// frequency...干嘛用的...




/////// container_hook

  template<class Container>
  struct container_hook
  {
    typedef boost::intrusive::member_hook< Container,
                                           policy_hook_type,
                                           &Container::policy_hook_ > type;
  };

  ///////
   ///////
   /**
     typedef typename        PolicyTraits::template policy<
     															trie_with_policy<FullKey, PayloadTraits, PolicyTraits>,
     															parent_trie,
     							typename PolicyTraits::template container_hook<parent_trie>::type
     																									>::type         policy_container;

   **/


  template<class Base,
           class Container,
           class Hook>  ////Hook ----> container_hook  line56
  struct policy
  {

	  ///
	  ///获得 每个parent_trie的frequency
    static double& get_order (typename Container::iterator item)
    {
      return static_cast<policy_hook_type*>
        (policy_container::value_traits::to_node_ptr(*item))->frequency;
    }
//////// 获得 每个const parent_trie的frequency
    static const double& get_order (typename Container::const_iterator item)
    {
      return static_cast<const policy_hook_type*>
        (policy_container::value_traits::to_node_ptr(*item))->frequency;
    }
////

    template<class Key>
    struct MemberHookLess
    {
      bool operator () (const Key &a, const Key &b) const
      {
        return get_order (&a) < get_order (&b);
      }
    };

    /////
    ////
    /////     policy_container   multiset中第二个参数,就是完成把元素从小到到大排起来.
    typedef boost::intrusive::multiset< Container,
                                   boost::intrusive::compare< MemberHookLess< Container > >,
                                   Hook > policy_container;

    // could be just typedef
    class type : public policy_container
    {
    public:
      typedef policy policy_base; // to get access to get_order methods from outside
      typedef Container parent_trie;

      type (Base &base)
        : base_ (base)
        , max_size_ (100)          ///// 设置最大
      {
      }

      /////
      ///// update should be the  behavior that interest meets the data cached in the node
      inline void
      update (typename parent_trie::iterator item)
      {
        policy_container::erase (policy_container::s_iterator_to (*item));
        get_order (item) += 1;
        policy_container::insert (*item);
      }

      inline bool
      insert (typename parent_trie::iterator item)
      {
        get_order (item) = 0;

        if (max_size_ != 0 && policy_container::size () >= max_size_)
          {
            // this erases the "least frequently used item" from cache
            base_.erase (&(*policy_container::begin ()));
          }

        policy_container::insert (*item);
        return true;
      }


     //
      ///
      ///should be the  behavior that interest meets the data cached in the node
      inline void
      lookup (typename parent_trie::iterator item)
      {
        policy_container::erase (policy_container::s_iterator_to (*item));
        get_order (item) += 1;
        policy_container::insert (*item);
      }

      inline void
      erase (typename parent_trie::iterator item)
      {
        policy_container::erase (policy_container::s_iterator_to (*item));
      }

      inline void
      clear ()
      {
        policy_container::clear ();
      }

      inline void
      set_max_size (size_t max_size)
      {
        max_size_ = max_size;
      }

      inline size_t
      get_max_size () const
      {
        return max_size_;
      }

    private:
      type () : base_(*((Base*)0)) { };

    private:
      Base &base_;
      size_t max_size_;
    };
  };
};

} // ndnSIM
} // ndn
} // ns3

#endif // LFU_POLICY_H
