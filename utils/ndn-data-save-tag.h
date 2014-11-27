/*
 * ndn-data-save-tag.h
 *
 *  Created on: 2014年9月18日
 *      Author: hengheng
 */

#ifndef NDN_DATA_SAVE_TAG_H_
#define NDN_DATA_SAVE_TAG_H_

#define cat_1  0x01
#define cat_2  0x02
#define cat_3  0x03
#define cat_4  0x04
#define cat_5  0x05

#include "ns3/tag.h"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-fw
 * @brief Packet tag that is used to track hop count for Interest-Data pairs
 */
class DataSaveTag : public Tag, public SimpleRefCount<DataSaveTag>
{
public:
  static TypeId
  GetTypeId (void);

  /**
   * @brief Default constructor
   */
  DataSaveTag () :  m_cat_tag (0x00) { };

  /**
   * @brief Destructor
   */
  ~DataSaveTag () { }



  /**
   * @brief Get value of hop count
   */
  uint32_t
  Get () const { return  m_cat_tag; }

  /**
    * @brief Get value of hop count
    */
  uint8_t
  Set(uint32_t c) {  m_cat_tag = c;  return m_cat_tag; }

  ////////////////////////////////////////////////////////
  // from ObjectBase
  ////////////////////////////////////////////////////////
  virtual TypeId
  GetInstanceTypeId () const;

  ////////////////////////////////////////////////////////
  // from Tag
  ////////////////////////////////////////////////////////

  virtual uint32_t
  GetSerializedSize () const;

  virtual void
  Serialize (TagBuffer i) const;

  virtual void
  Deserialize (TagBuffer i);

  virtual void
  Print (std::ostream &os) const;

private:
  uint32_t m_cat_tag;
};

} // namespace ndn
} // namespace ns3



#endif /* NDN_DATA_SAVE_TAG_H_ */
