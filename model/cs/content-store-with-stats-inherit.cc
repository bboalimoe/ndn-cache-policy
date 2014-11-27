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

#include "content-store-with-stats-inherit.h"

#include "../../utils/trie/random-policy.h"
#include "../../utils/trie/lru-policy.h"
#include "../../utils/trie/fifo-policy.h"
#include "../../utils/trie/lfu-policy.h"
#include "../../utils/trie/lfu-policy-save-tag.h"

#define NS_OBJECT_ENSURE_REGISTERED_TEMPL(type, templ)  \
  static struct X ## type ## templ ## RegistrationClass \
  {                                                     \
    X ## type ## templ ## RegistrationClass () {        \
      ns3::TypeId tid = type<templ>::GetTypeId ();      \
      tid.GetParent ();                                 \
    }                                                   \
  } x_ ## type ## templ ## RegistrationVariable

namespace ns3 {
namespace ndn {

using namespace ndnSIM;

namespace cs {

// explicit instantiation and registering


/**
 * @brief ContentStore with stats and Least Frequently Used (LFU) cache replacement policy and the tag diff
 **/
template class ContentStoreWithStatsinherit<lfu_policy_save_tag_traits>;

//template class ContentStoreWithStatsinherit<lfu_policy_save_tag_traits>;



NS_OBJECT_ENSURE_REGISTERED_TEMPL(ContentStoreWithStatsinherit, lfu_policy_save_tag_traits);


#ifdef DOXYGEN
/**
 * \brief Content Store with stats implementing FIFO cache replacement policy
 */
class Statsinherit::Lfu_save_tag : public ContentStoreWithStatsinherit<lfu_policy_save_tag_traits> { };

#endif


} // namespace cs
} // namespace ndn
} // namespace ns3
