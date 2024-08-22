/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef SRNC_HEADER_H
#define SRNC_HEADER_H

#include "ns3/header.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/sequence-number.h"
#include "ns3/ipv4-address.h"
#include "ns3/tcp-header.h"
#include "ns3/log.h"
#include <stdio.h>
#include <iostream>

namespace ns3 {

class SrncHeader : public Header
{
public:
		SrncHeader();
		virtual ~SrncHeader();
		static TypeId GetTypeId (void);
  		virtual TypeId GetInstanceTypeId (void) const;
		virtual void Print (std::ostream &os) const;
  		


  		void SetEncodedCoeffVector(const std::vector<uint8_t>& vector);
  		const std::vector<uint8_t>& GetEncodedCoeffVector() const;
  		virtual void Serialize (Buffer::Iterator BufferStart) const;

		virtual uint32_t Deserialize (Buffer::Iterator BufferStart);
  		virtual uint32_t GetSerializedSize (void) const;
		std::vector <uint8_t> GetReceivedHeaderCoeffVector() const;

  		void SetEncodedLabel (uint32_t label);
  		uint32_t GetEncodedLabel() const;

  		void SetPri(uint8_t pri);
  		uint8_t GetPri() const;

  		void SetEncodedPacketNumber(uint8_t numbers);
  		uint8_t GetEncodedPacketNumber() const;

		void PrintHeader (void);


private:
		uint8_t NcPri;
		uint8_t EncodedNumber;
		uint32_t EncodedLabel;
		std::vector <uint8_t> EncodedCoeffVector;
		std::vector <uint8_t> ReceivedHeaderCoeffVector;

};
/* ... */

}

#endif /* SRNC_H */

