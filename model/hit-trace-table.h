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
#ifndef HIT_TRACE_TABLE_H_
#define HIT_TRACE_TABLE_H_

#include "ns3/ndnSIM-module.h"
#include "../utils/ndn-data-save-tag.h"
#include "ns3/object.h"
#include <vector>
#include "ns3/vector.h"
#include "ns3/ndn-data.h"
#include "ns3/ndn-interest.h"
#include "ns3/log.h"
//#include "ns3/ndnSIM/model/cs/ndn-content-store.h"


namespace ns3 {
namespace ndn {

class ContentStore;
// declare

static std::vector<float> initialWeight(5) ;

class StatsByTagCs
	{
public:
		StatsByTagCs():m_cacheHits_vect(5) ,m_cacheMisses_vect(5)
		{

		}

		struct CountByTagCs
		{
			int int_tag;
		};

	  inline void Reset ()
	  {
	    m_cacheHits_vect.clear();
	    m_cacheMisses_vect.clear();
	  }

	  std::vector<CountByTagCs> m_cacheHits_vect;
	  std::vector<CountByTagCs> m_cacheMisses_vect;

private:

	};



class HitTraceTable :  public  SimpleRefCount<HitTraceTable>
 {
 public:


	 struct DataByTag{

		 DataByTag():m_old_hits(0) , m_new_hits(0), deno_new_hits(0), deno_old_hits(0){}
	  	   float m_old_hitsrate;
	  	   float m_new_hitsrate;

	  	   float m_old_error; //第一次的误差为0
	  	   float m_new_error;

	  	   float deno_old_hitsrate;
	  	   float deno_new_hitsrate;

	  	   int m_old_hits ;
	  	   int m_new_hits ;

	  	   int deno_old_hits ;
	  	   int deno_new_hits ;
	     };




   /**
    * \brief Construct content store entry
    *
    * \param header Parsed Data header
    * \param packet Original Ndn packet
    *
    * The constructor will make a copy of the supplied packet and calls
    * RemoveHeader and RemoveTail on the copy.
    */
 	HitTraceTable (Ptr< ContentStore> m_cs_base)
 {
 		 //m_cs(&(*m_cs_base));  //初始化m_cs
 		 //这儿的问题
 		this->SetContentStore(m_cs_base) ;
 	//	std::cout << " 这是HitTraceTable的拷贝构造函数" <<std::endl;


 }

 	HitTraceTable():m_tag_data(5),m_stats(),m_delta_tag(5)
 	{

 		//std::cout << " 这是HitTraceTable的默认构造函数" <<std::endl;
 		//here initial the initialWeight
 		//当执行第一次时候，进行初始化
 		float temp[5] = {  0.4647, 0.2201, 0.2201, 0.0661, 0.0289  };
 		//Her=0.4647   H=0.2201   H=0.2201   L=0.0661   Ler=0.0289
 	    for (int i = 0 ; i<= 4 ; i++) {  initialWeight.push_back(temp[i]) ; }
 		// 初始化k = 1的降噪hitrate值和误差值
 		for (int i=0; i<=4 ; i++)
 		{
 			m_tag_data[i].deno_old_hitsrate = initialWeight[i] ;
 			m_tag_data[i].m_old_error = 0;
 		}


  ///// 默认构造函数,必须得有

 	}
		void Connect()
		{

		 }
   /**
    * @brief Get pointer to access store, to which this entry is added
    */
   Ptr< ContentStore>
   GetContentStore ()
   {
	   return m_cs ;
   }

   void SetContentStore(Ptr<ContentStore>  m_cs_copy)
   {
	   m_cs = m_cs_copy ;

   }

   void
   CacheHits (Ptr<const  Interest> interest, Ptr<const  Data> data)
   {
   /// list 0, 1, 2, 3, 4  --> tag 0x1, 0x2 ,0x3, 0x4, 0x5
     //m_stats.m_cacheHits ++;


   	DataSaveTag tag;
   if( interest->GetPayload()->PeekPacketTag(tag) )
   {
   	int  tag2index = tag.Get() - 1;
   	(this->m_stats.m_cacheHits_vect[tag2index]).int_tag++ ;

   //NS_LOG_DEBUG( "cachehit " << tag2index << "号" << (m_stats.m_cacheHits_vect[tag2index]).int_tag );
   	//std::cout << "node " <<(this->GetContentStore()->GetObject<ns3::Node>() )->GetId() <<"号，缓存命中" << std::endl;
   }

   }

   void
  CacheMisses (Ptr<const   Interest> interest)
   {
     //m_stats.m_cacheMisses ++;
   	DataSaveTag tag;
       if( interest->GetPayload()->PeekPacketTag(tag) )
       {
   		int  tag2index = tag.Get() - 1;
   		(this->m_stats.m_cacheMisses_vect[tag2index]).int_tag++ ;
   	   //<< std::endl<< "cachemiss  "  <<std::endl << tag2index << "号" << (m_stats.m_cacheHits_vect[tag2index]).int_tag ;

       }


   }

   void Reset()
   {
 	  m_stats.Reset();
   }

   /// 设置各个tag的cs的比例
   void ChangeCsRatioByTag( )
   {

	   //NS_LOG_DEBUG(  "we will change the cs ratio NOW!!" );

 	  ////因为m_cs已经绑定了Csimplinherit的实例,所以可以在这里操作CS中的条目了

       std::vector<float> original_ratio = m_cs->GetRatio();
 	  Ptr<ns3::Node> m_node = m_cs->GetObject<ns3::Node> ();
 	  std::cout << "节点号 = " << m_node->GetId() <<std::endl;
      uint32_t max_size_cs = m_cs->GetMaxSize();


      for (int i =0 ; i <=4 ;i++)
      {
    	  std::cout << "each tag ratio=" << original_ratio[i] << std::endl;
      }
     // std::cout <<  "MaxSize=" << max_size_cs << std::endl;

       std::vector<float> ratio_vect(5) ;
       std::vector<int>  n_vect(5);
       std::vector<int>  temp_vect(5);
       //这里做个循环,使得改变后的各cs值都大于1
       int n_sum = 0;
    	   for (int i = 0 ; i <= 3; i++)
       {
    	  temp_vect[i] =  int(max_size_cs * original_ratio[i]) + m_delta_tag[i]  ;
    	  n_sum += temp_vect[i] ;
    	 // std::cout << "sum " << n_sum << std::endl;
       }
    	   temp_vect[4] = max_size_cs - n_sum ;
            //change all to +
    	   ToAllPositive(temp_vect);
    	   n_vect = temp_vect ;
       ///////////艹你大爷....


      this->PrintCsSpace(m_delta_tag, n_vect);
       /////
   	for( int i = 0 ; i <= 4 ; i++)
   	{
         ratio_vect[i] = float(n_vect[i]) / float(max_size_cs) ;
   	}
       //这里我们设置了新的缓存分配比例
   	//为啥 没调用这个函数....................

   	 m_cs->SetRatio(ratio_vect) ;
  	//std::cout << "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"  << std::endl;

   }

   void PrintCsSpace(std::vector<int> &m_delta_tag, std::vector<int> &n_vect)
   {
	  /**
	   * 0s        CsChSpace
	            0s        CsOccuSpace
	   */
		 std::cout << Simulator::Now() << "\t"\
	    	         		 <<"CsChSpace(各个tag在ContentStore中要改变的条目数)" << "\t" ;
	    	   for (int i = 0; i<= 4 ; i++)
	    	   {
	    		   std::cout <<"\t" <<   m_delta_tag[i] << "\t" ;
	    	   }
	    	   std::cout << std::endl;

	    	 //  0s        deRelHrate
	    	   std::cout << Simulator::Now() << "\t"\
	    	      	         		 <<" CsOccuSpace(各个tag 在ContentStore占有的条目数)" << "\t" ;
	      	   for (int i = 0; i<= 4 ; i++)
	         {
	           std::cout <<"\t" << n_vect[i]<< "\t" ;
	         }
	      	   std::cout << std::endl;

   }

   void CalCsRatioByTag(Time &t)
    {
	   //Schedule 第一项time 是 超过当前时间的时间
	   //在到相对时间"time"时(相当于当前的仿真时间加入传入
			//   的参数"time")

	   Simulator::Schedule (t, &HitTraceTable::CalCsRatioByTag, this, t);

	   //Simulator::Schedule (t+Seconds(10.0), &HitTraceTable::CalCsRatioByTag, this, t+Seconds(10));
	   //做check 如果cachehits 各个tag有一个为0 ，则return 不执行cs限定数目的更改

	   /*
	   int count_out = 0;
	   for(int i = 0 ; i <= 4; i ++)
	   {
		   if (m_stats.m_cacheHits_vect[i].int_tag == 0) count_out++ ;
		   //NS_LOG_DEBUG(  "第" <<i << "类的hits数量 =" << m_stats.m_cacheHits_vect[i].int_tag );
           if(count_out > 2) return ;
           //todo
           //因为这里ｒｅｔｕｒｎ了　所以．．．只显示１号和５３号的
	   }
	   */

	   ///如果都不为零 ，则进行计算！
     float sum;



     float  K = 5;
      float alpha = 0.2;




   	for( int i = 0;i<= 4; i++)
   	{
   		m_tag_data[i].m_new_hits = m_stats.m_cacheHits_vect[i].int_tag ;
   		sum +=  m_stats.m_cacheHits_vect[i].int_tag;
   	}


     for(int i = 0; i <= 4 ;i++)
     {
   	   m_tag_data[i].m_new_hitsrate =  m_stats.m_cacheHits_vect[i].int_tag / sum ;
   	   m_tag_data[i].m_new_error = m_stats.m_cacheHits_vect[i].int_tag / sum - initialWeight[i] ;
   	    //把initial weight 改为 old value

     }
     
  // 计算降噪的hits数
     int deno_hits_sum = 0 ;
     for (int i = 0; i<=4 ; i++)
     {
   	   m_tag_data[i].deno_new_hits= alpha * m_tag_data[i].m_new_hits + \
   			                                                               (1-alpha) * m_tag_data[i].deno_old_hits ;
   	   deno_hits_sum += m_tag_data[i].deno_new_hits ;

     }
      int delta_tag_sum = 0;
     for ( int i = 0 ; i<=3 ; i++)
     {
   	   m_delta_tag[i] = int(   deno_hits_sum / K *       (m_tag_data[i].m_new_error - (1-alpha) * m_tag_data[i].m_old_error)    ) ;
   	    //todo;

   	   delta_tag_sum += m_delta_tag[i] ;
     }
     //  为了让
     m_delta_tag[4] = -(delta_tag_sum) ; // here 各个tag的缓存entry调整数目便知

     for( int i = 0 ; i <= 4 ; i++)
     {
    	// std::cout << "改变的数量" << i << " "<< m_delta_tag[i] <<std::endl;
     }

      // 做变化,改cs
     //NS_LOG_DEBUG(  "now the time is " << Simulator::Now() );
     //NS_LOG_DEBUG(  m_tag_data[1].deno_new_hitsrate );
     this->PrintHitRate(m_tag_data);
      this->ChangeCsRatioByTag();
      // 将new值转化给old值



      for (int i = 0; i<= 4 ; i++)
      {

      m_tag_data[i].m_old_hitsrate = m_tag_data[i].m_new_hitsrate ;
      m_tag_data[i].deno_old_hitsrate = m_tag_data[i].deno_new_hitsrate ;
      m_tag_data[i].m_old_error = m_tag_data[i].m_new_error ;

      m_tag_data[i].m_old_hits = m_tag_data[i].m_new_hits ;
      m_tag_data[i].deno_old_hits = m_tag_data[i].deno_new_hits ;

      }


    }

    void PrintHitRate( std::vector<DataByTag > & m_tag_data)
    {
    	// 0s        reRelHrate
    	 std::cout << Simulator::Now() << "\t"\
    	         		 <<"reRelHrate(各个tag最新的相对命中率)" << "\t" ;
    	   for (int i = 0; i<= 4 ; i++)
    	   {
    		   std::cout <<"\t" <<   m_tag_data[i].m_new_hitsrate << "\t" ;
    	   }
    	   std::cout << std::endl;

    	 //  0s        deRelHrate
    	   std::cout << Simulator::Now() << "\t"\
    	      	         		 <<"deRelHits(降噪后的命中次数)" << "\t" ;
      	   for (int i = 0; i<= 4 ; i++)
         {
           std::cout <<"\t" << m_tag_data[i].deno_new_hits << "\t" ;
         }
      	   std::cout << std::endl;

    }


 protected:
   //< \brief content store to which the htt is binded with
    Ptr< ContentStore> m_cs;  //在csimplinherit构造函数执行后,m_cs指向csimplinherit的一个运行实例

   //Ptr<const Data> m_data; ///< \brief non-modifiable Data

   std::vector<DataByTag > m_tag_data ;  //记录命中率,降噪命中率和误差值
   StatsByTagCs m_stats ;
   std::vector<int> m_delta_tag ; //各个tag的缓存调整数目
//


	void ToAllPositive( std::vector<int>& temp_vect) {
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

   //static LogComponent g_log ;  因为hitTraceTable不是单独一个可以被绑定的object,所以在这儿定义glog后,就会报错多次定义
private:


 };  //end HittraceTable


//
//LogComponent HitTraceTable::g_log = LogComponent ("ndn.hittracetable");

}
}
#endif /* CONTENT_STORE_WITH_STATS_INHERIT_H_ */
