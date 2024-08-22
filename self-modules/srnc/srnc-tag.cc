/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */


#include "srnc-tag.h"
#include "ns3/tag.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include <stdio.h>

NS_LOG_COMPONENT_DEFINE("SrncTag");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SrncTag);

SrncTag::SrncTag ()
  : EncodedNumber (0),
    TcpSeqVector (0),
    TcpTsVector (0),
    TcpEhVector (0),
    useUdp (true)
{
  // Nothing to do here
}

TypeId
SrncTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SrncTag")
    .SetParent<Tag> ()
    .AddConstructor<SrncTag> ();
  return tid;
}

TypeId
SrncTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
SrncTag::GetSerializedSize (void) const
{
  if(useUdp){
    return sizeof(EncodedNumber) + 2 * EncodedNumber;
  }else
  {
    return sizeof(EncodedNumber) + 4 * EncodedNumber ; // + 4 * EncodedNumber + 4 * EncodedNumber;
  }
  
}

void
SrncTag::Serialize (TagBuffer i) const
{
  i.Write ((const uint8_t *)&EncodedNumber, sizeof(uint8_t));
  if(EncodedNumber){
    if(!useUdp){
      for (uint8_t k = 0; k < EncodedNumber; k++){
        i.WriteU32 (TcpSeqVector[k]);
      }
      // for (uint8_t m = 0; m < EncodedNumber; m++){
      //   i.WriteU32 (TcpTsVector[m]);
      // }
      // for (uint8_t l = 0; l < EncodedNumber; l++){
      //   i.WriteU32 (TcpEhVector[l]);
      // }      
    }
  }
  
}

void
SrncTag::Deserialize (TagBuffer i)
{
  uint32_t tmp2;
  i.Read (&EncodedNumber, sizeof(uint8_t));
  if(EncodedNumber){
    if(!useUdp){
      for (uint8_t k = 0; k < EncodedNumber; k++){
        tmp2 = (uint32_t) i.ReadU32();
        TcpSeqVector.push_back(tmp2);
      }
      // for (uint8_t m = 0; m < EncodedNumber; m++){
      //   tmp2 = (uint32_t) i.ReadU32();
      //   TcpTsVector.push_back(tmp2);
      // } 
      // for (uint8_t l = 0; l < EncodedNumber; l++){
      //   tmp2 = (uint32_t) i.ReadU32();
      //   TcpEhVector.push_back(tmp2);
      // }       
    }
  }
}

void
SrncTag::Print (std::ostream &os) const
{
  os << EncodedNumber;
}

void 
SrncTag::SetEncodedPacketNumber(uint8_t numbers)
{
	EncodedNumber = numbers;
}

uint8_t 
SrncTag::GetEncodedPacketNumber() const
{
	return EncodedNumber;
}


void
SrncTag::SetTcpSeqVector(const std::vector<uint32_t>& SeqVector)
{
  TcpSeqVector = SeqVector;
}

std::vector<uint32_t>
SrncTag::GetTcpSeqVector() const
{
  return TcpSeqVector;
}

void
SrncTag::SetTcpTsVector(const std::vector<uint32_t>& TsVector)
{
  TcpTsVector = TsVector;
}

std::vector<uint32_t>
SrncTag::GetTcpTsVector() const
{
  return TcpTsVector;
}

void
SrncTag::SetTcpEhVector(const std::vector<uint32_t>& EhVector)
{
  TcpEhVector = EhVector;
}

std::vector<uint32_t>
SrncTag::GetTcpEhVector() const
{
  return TcpEhVector;
}

void
SrncTag::SetUseUdp(bool useudp)
{
  useUdp = useudp;
}

bool
SrncTag::GetUseUdp() const
{
  return useUdp;
}

} // namespace ns3

