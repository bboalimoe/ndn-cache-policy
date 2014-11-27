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

#include "ndn-data-save-tag.h"

namespace ns3 {
namespace ndn {

TypeId
DataSaveTag::GetTypeId ()
{
  static TypeId tid = TypeId("ns3::ndn::DataSaveTag")
    .SetParent<Tag>()
    .AddConstructor<DataSaveTag>()
    ;
  return tid;
}

TypeId
DataSaveTag::GetInstanceTypeId () const
{
  return DataSaveTag::GetTypeId ();
}


////
///     puzzle about the miss use of uint8_t to replace the uint32_t
///
uint32_t
DataSaveTag::GetSerializedSize () const
{
  return sizeof(uint32_t);
}

void
DataSaveTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_cat_tag);
}

void
DataSaveTag::Deserialize (TagBuffer i)
{
  m_cat_tag = i.ReadU32 ();
}

void
DataSaveTag::Print (std::ostream &os) const
{
  os << m_cat_tag;
}

} // namespace ndn
} // namespace ns3
