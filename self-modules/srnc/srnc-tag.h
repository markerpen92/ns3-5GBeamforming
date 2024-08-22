/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */


#ifndef SRNC_TAG_H
#define SRNC_TAG_H

#include "ns3/packet.h"
#include "ns3/nstime.h"


namespace ns3 {

class Tag;

/**
 * Tag to calculate the per-PDU delay from eNb SRNC to UE SRNC
 */

class SrncTag : public Tag
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId  GetTypeId (void);
  virtual TypeId  GetInstanceTypeId (void) const;

  /**
   * Create an empty SRNC tag
   */
  SrncTag ();

  virtual void  Serialize (TagBuffer i) const;
  virtual void  Deserialize (TagBuffer i);
  virtual uint32_t  GetSerializedSize () const;
  virtual void Print (std::ostream &os) const;

  void SetEncodedPacketNumber(uint8_t numbers);
  uint8_t GetEncodedPacketNumber() const;

  void SetTcpSeqVector(const std::vector<uint32_t>& SeqVector);
  std::vector <uint32_t> GetTcpSeqVector() const;

  void SetTcpTsVector(const std::vector<uint32_t>& TsVector);
  std::vector <uint32_t> GetTcpTsVector() const;

  void SetTcpEhVector(const std::vector<uint32_t>& EhVector);
  std::vector <uint32_t> GetTcpEhVector() const;

  void SetUseUdp(bool useudp);
  bool GetUseUdp() const;
  
private:
  uint8_t EncodedNumber;
  std::vector <uint32_t> TcpSeqVector;
  std::vector <uint32_t> TcpTsVector;
  std::vector <uint32_t> TcpEhVector;
  bool useUdp;

};

} //namespace ns3

#endif /* SRNC_TAG_H */
