/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2014 BUPT
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
 * Authors:    hengyang zhang <bboalimoe@gmail.com>
 */

#ifndef LFU_POLICY_SAVE_TAG_H_
#define LFU_POLICY_SAVE_TAG_H_

#include <boost/intrusive/options.hpp>
#include <boost/intrusive/set.hpp>

#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/functional/hash.hpp>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/mpl/if.hpp>
#include<iostream>
#include<string>
#include <vector>
#include "../../utils/ndn-data-save-tag.h"

namespace ns3 {
namespace ndn {
namespace ndnSIM {


/**
 * @brief Traits for LFU replacement policy
 */
struct lfu_policy_save_tag_traits
{



  /// @brief Name that can be used to identify the policy (for NS-3 object model and logging)
  static std::string GetName () { return "Lfu_save_tag"; } ///shit -- __


   ///
  ///   set_member_hook 是set的memberhook.....蛋疼
  /// 声明过后,该类就具备了 intrusive的特性了
  struct policy_hook_type : public boost::intrusive::set_member_hook<>
  {
	  double frequency;

  };

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
   /**    trie with polic.h   line 48
     typedef typename        PolicyTraits::template policy<
     															trie_with_policy<FullKey, PayloadTraits, PolicyTraits>,    ////FullKey --> class Name
     															parent_trie,    ///////////// line 39
     							typename PolicyTraits::template container_hook<parent_trie>::type
     																									>::type         policy_container;

   **/


  template<class Base,
           class Container,   ///Container 就是一个 trie
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

    template<class Key>    ////Key ---> trie *
    struct MemberHookLess
    {
      bool operator () (const Key &a, const Key &b) const
      {
        return get_order (&a) < get_order (&b);
      }
    };

    /////
    ////
    /////     policy_container   multiset中第二个参数,就是完成把元素从小到到大排起来. Hook就是一个hook
    typedef boost::intrusive::multiset< Container,
                                   boost::intrusive::compare< MemberHookLess< Container > >,    ////Container ----> parent_trie
                                   Hook > policy_container;


    // could be just typedef
    class type : public policy_container    ///这里就是初始化这个multiset了
    {

    public:
      typedef policy policy_base; // to get access to get_order methods from outside
      typedef Container parent_trie;

      type (Base &base)
        : base_ (base)        ///// trie with policy
        , max_size_ (40000) ///// 设置最大
        , max_vect_by_tag(5)
               ///std::vector<float>
      {
    	  //hengheng
    	  float temp[5] = {  0.4647, 0.2201, 0.2201, 0.0661, 0.0289 };
    	  for ( int i = 0 ; i< 4 ; i++)
    	  {
    		  ratio.push_back(temp[i]) ;
    	  }
    	;
    	  CalNewMaxValue();

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

  	void ToAllPositive( std::vector<uint32_t>& temp_vect) { //这个function 可以作为一个工具类放到utils中
  		bool isAllpositive = false;
  		uint32_t limits = 10;
  		while (1) {
  			for (int i = 0; i <= 4; i++) {
  				if (temp_vect[i] <= 0) {
  					for (int j = 4; j >= 0; j--) {
  						if (i == j)
  							continue;
  						//这里把limit 改为 10
  						if ((temp_vect[j] - limits) > 0) {
  							temp_vect[j]--;
  							temp_vect[i]++;
  							break;
  						}
  					}
  				}
  			}
  			isAllpositive = true;
  			for (int i = 0; i <= 4; i++) {
  				if (temp_vect[i] <= 0)
  					isAllpositive = false;
  			}
  			for (int i = 0; i <= 4; i++) {
  				//std::cout << "" << i << "" << temp_vect[i] << std::endl;
  				//std::cout << std::endl;
  			}
  			if (isAllpositive == true)
  				break;
  		}
  	}




      void CalNewMaxValue()
      {
    	  uint32_t temp_sum = 0;
    	  for (int i = 0 ; i<= 4 ; i++) {//

        		  max_vect_by_tag[i] = int(max_size_ * ratio[i]) ;
        		  temp_sum += max_vect_by_tag[i] ;
        	  }
    	  ///
        	  this->max_vect_by_tag[4] = max_size_ -  ( temp_sum - max_vect_by_tag[4] ) ;
        	  this->ToAllPositive(max_vect_by_tag) ;

    	  //设置一下 各个部分的最大数量...关键..
        	  /*
    	        //hengheng
    	      	float temp = 0.0 ;
    	      	//// int() 取整数部分
    	      	switch(tag_value){
    	  			 case 0x1 :
    	  		     max_vect_by_tag[0] = max_size_ - int(max_size_ * ratio[0])+int(max_size_ * ratio[1])+ \
    	  				     			                              int(max_size_ * ratio[2]) + int(max_size_ * ratio[3]) ;
    	  			 break;
    	  			 case 0x2 :
    	  			 temp = ratio[1]; max_vect_by_tag[1] = int(max_size_ * temp);
    	  			 break;
    	  			 case 0x3 :
    	  			temp = ratio[2];max_vect_by_tag[2] = int(max_size_ * temp);
    	  			 break;
    	  			 case 0x4 :
    	  			temp = ratio[3];max_vect_by_tag[3] = int(max_size_ * temp);
    	  			 break;
    	  			 case 0x5 :
    	  			temp = ratio[4];max_vect_by_tag[4] = int(max_size_ * temp);
    	  			 break;
    	      	}
    	      	*/

      }


      inline bool
      insert (typename parent_trie::iterator item)    //改了
      {
    	 // 问题1:因为interest的种类太少..所以...一直添不满... 换成 1000就好了!!
    //	/ 问题2:看一下算cschangespace 肯定是0 , 搞不清楚怎么回事儿.......
    	get_order (item) = 0 ;
    	DataSaveTag tag ;
    	uint32_t tag_value ;
        if(item->payload()->GetData()->GetPayload ()->PeekPacketTag(tag))
        {
    		         ////获得了tag_value
    		    	  tag_value = tag.Get() ;
         }



        /// 统计各个tag的entry数量
        std::vector<int> count_vect_by_tag(5);
		count_vect_by_tag = GetCountByTag(count_vect_by_tag); //这是获得各个类别的cs中的数量.
        // after this ,nvect_by_tag 就被填充满了
        CalNewMaxValue() ;  //为了把 max_vect_by_tag填满. 当获得新的ratio后,

        BOOST_FOREACH(int realcount, count_vect_by_tag)
          	               {
          	              	std::cout << "实际的count数   " << realcount << "\t" ;
          	               };

			 std::cout << std::endl;

        typename parent_trie::iterator trieIt;

       if (policy_container::size() ==  max_size_ )   //if 1    不满的话就随便添.
       {
/*
    	   BOOST_FOREACH(int realcount, count_vect_by_tag)
    	               {
    	              	std::cout << "实际的count数   " << realcount << "\t" ;
    	               };
    	            //   std::cout << std::endl;

    		   BOOST_FOREACH(int realcount, max_vect_by_tag)
    				 {
    				std::cout << "最大的count数,max count   " << realcount << "\t" ;
    				 };
*/



    	   // 如果某一个tag的大于其规定的max值
    	   //hengheng 看看这里能否重构一下
    	   //std::cout << "max size" << max_size_ <<  std::endl;

          if(count_vect_by_tag[tag_value -1] >= max_vect_by_tag[tag_value -1]) // if 2 如果来的tag的数量大于max值
    	   {
					  GetTrieIter(tag, tag_value, trieIt) ;
      			  //	std::cout << "trieit结果2的count, tag-in" << trieIt->payload()->GetReferenceCount ()<<std::endl;

    				  base_.erase (&(*trieIt) ) ;
    			//	  std::cout<< "cache of the tag-In is full" <<std::endl;
	 		//		  std::cout<< "==========================================================================" <<std::endl;

    	   } //endif 2
          else // 如果来的tag的数量小于max值
          {
        	  bool isErased = false;
        	  for (int i =4 ; i >= 0 ; i-- )
        	  {
        		  if (i == (tag_value -1) ) continue;
        		  if(count_vect_by_tag[i] >= max_vect_by_tag[i])
        		  {

        				//std::cout << "trieit结果0的count" << trieIt->payload()->GetReferenceCount ();
        			     GetTrieIter(tag , i+1 ,trieIt) ;
        		//	 	std::cout << "trieit结果2的count, tag-other" << trieIt->payload()->GetReferenceCount ()<<std::endl;
        			      //问题就是没获得相应的trieIt 其为 空
        				//  std::cout << " fuck    " << trieIt->payload()->GetName().toUri() << std::endl;
						  base_.erase (&(*trieIt) );
						  isErased = true;
		 			//	  std::cout<< "cache of the tag-Other is full" <<std::endl;
		 				//  std::cout<< "==========================================================================" <<std::endl;


        		   }
        		   if (isErased) break ;
        	    }
          }
       } //endif 1
        	//multiset 返回会有一个list,遍历该list得到取 tag为目标tag_value的最小frequency 然后删除..

            // this erases the "least frequently used item" from cache
            ///base_.erase (&(*policy_container::begin ()));


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
        policy_container::insert (*item);     // 静态调用..蛋疼...
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

      inline void set_ratio(std::vector<float>  &vect)
      {
    	  ratio = vect;
/*
    	  std::cout <<  "here we call the policy set ratio START" << std::endl;

    	  for(int i = 0 ; i <=4 ; i++) { std::cout << vect[i] << std::endl;}

    	  std::cout <<  "here we call the policy set ratio END   " << std::endl;
*/
      }

      inline std::vector<float> get_ratio() const
	  {

    	 // std::cout <<  "here we call the policy GET ratio START" << std::endl;

    	     	//  for(int i = 0 ; i <=4 ; i++) { std::cout << ratio[i] << std::endl;}

    	//  std::cout <<  "here we call the policy GET ratio END   " << std::endl;
       	//  std::cout <<  "here we call the policy GET RATIO !!! " << std::endl;
    	  return ratio ;
	   }

      inline void
      set_max_size (size_t max_size)
      {
    	 // std::cout << "lfu policy here SET MAXSIZE " << max_size << std::endl;
        max_size_ = max_size;
        //add attribute 那里也有设置max size
      }

      inline size_t
      get_max_size () const
      {
    	// std::cout << "lfu policy max_size = " << max_size_ << std::endl;
        return max_size_;
      }


		protected:

			typename parent_trie::iterator
			GetTrieIter(DataSaveTag tag,
					uint32_t tag_value, typename parent_trie::iterator &trieIt) {
				for (typename policy_container::iterator iter =
						policy_container::begin();
						iter != policy_container::end(); iter++) {

					if ((&(*iter))->payload()->GetData()->GetPayload()->PeekPacketTag(
							tag)) {

						if (tag.Get() == tag_value) {


							//std::cout << "iter结果的count" << iter->payload()->GetReferenceCount ()<<std::endl;

							//std::cout << "cs的容量" << policy_container::size() << std::endl;
							trieIt = &(*iter);
						//	std::cout << "trieit结果1的count" << trieIt->payload()->GetReferenceCount ()<<std::endl;
							break;
						}
					}
				}
				return trieIt;
			}

			std::vector<int>
			GetCountByTag( std::vector<int>& count_vect_by_tag) {
				for (typename policy_container::iterator iter =
						policy_container::begin();
						iter != policy_container::end(); iter++) {
					/// payload()应该返回entry
					DataSaveTag tag_temp;
					if ((&(*iter))->payload()->GetData()->GetPayload()->PeekPacketTag(
							tag_temp)) {
						count_vect_by_tag[tag_temp.Get() - 1]++;
					}
				}
				return count_vect_by_tag;
			}

    private:
      type () : base_(*((Base*)0)) , max_size_ (40000)  , ratio() , max_vect_by_tag(){}          ///vector<float>{ };

    private:
      Base &base_;
      size_t max_size_;
      std::vector<uint32_t>  max_vect_by_tag ; //每个tag的entry数量限制

      std::vector<float>  ratio ;  //每个tag的entry所占比例
      ///// position 0 --> tag 0x1 and so on for the rest .///hard code need replace it with vector ..
    };
  };
};

} // ndnSIM
} // ndn
} // ns3

#endif //
