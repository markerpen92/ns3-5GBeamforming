/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "srnc-header.h"
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <bitset>

NS_LOG_COMPONENT_DEFINE("SrncHeader");

namespace ns3 {

SrncHeader::SrncHeader()
  : NcPri (0),
    EncodedNumber (0),
    EncodedLabel (0),
    EncodedCoeffVector (0),
    ReceivedHeaderCoeffVector (0)
{
  NS_LOG_FUNCTION (this);
}

SrncHeader::~SrncHeader()
{
  NS_LOG_FUNCTION (this);
}

TypeId
SrncHeader::GetTypeId(void)
{
	static TypeId tid = TypeId ("ns3::SrncHeader")
    		.SetParent<Header> ()
    		.AddConstructor<SrncHeader> ();
  	return tid;
}

TypeId
SrncHeader::GetInstanceTypeId (void) const
{
	return GetTypeId();
}

uint32_t  
SrncHeader::GetSerializedSize (void) const
{
	return 4 + 1 + 1 + 14;
}

std::vector <uint8_t>
SrncHeader::GetReceivedHeaderCoeffVector() const
{	
	return ReceivedHeaderCoeffVector;
}

void 
SrncHeader::SetEncodedLabel(uint32_t label)
{
 	EncodedLabel = label;
 	NS_LOG_FUNCTION(this << " SetNcLabel:"<<label);
}

uint32_t
SrncHeader::GetEncodedLabel() const
{	
	NS_LOG_FUNCTION(this << " GetNcLabel:"<< EncodedLabel);
	return EncodedLabel;
}

void 
SrncHeader::SetEncodedPacketNumber(uint8_t numbers)
{
	EncodedNumber = numbers;
}

uint8_t 
SrncHeader::GetEncodedPacketNumber() const
{
	return EncodedNumber;
}

void 
SrncHeader::SetPri(uint8_t pri)
{
	NcPri = pri;
}

uint8_t 
SrncHeader::GetPri() const
{
	return NcPri;
}

void
SrncHeader::Serialize (Buffer::Iterator BufferStart) const
{	
	std::vector <uint8_t> headerVector;
	for(uint8_t i = 0 ; i < EncodedCoeffVector.size() ; i++)
    {   
        headerVector.push_back(EncodedCoeffVector[i]);
        //NS_LOG_FUNCTION(this << " headerVector:"<<headerVector.size());
    }
    BufferStart.WriteU32(EncodedLabel);
    BufferStart.WriteU8(EncodedNumber);
    BufferStart.WriteU8(NcPri);


    if(EncodedNumber)
    {
    	for(uint8_t index = 0; index < headerVector.size();index++)
    	{
    		BufferStart.WriteU8(headerVector[index]);
    	}
    }

}

uint32_t 
SrncHeader::Deserialize (Buffer::Iterator BufferStart)
{	
	
  EncodedLabel = BufferStart.ReadU32();
	EncodedNumber = BufferStart.ReadU8();
	NcPri = BufferStart.ReadU8();

	int temp;

	if(EncodedNumber)
    {
    	for(uint8_t index = 0; index < 14 ;index++)
    	{
    		temp = BufferStart.ReadU8();
    		ReceivedHeaderCoeffVector.push_back(temp);
    	}

    }

	return GetSerializedSize ();
}

void
SrncHeader::Print (std::ostream &os) const
{
	
}

void 
SrncHeader::PrintHeader ()
{
	NS_LOG_INFO(this << " NcLabel:" << (int)EncodedLabel << " NcNum:" << (int)EncodedNumber << " NcPri:" << (int)NcPri);

	for (u_int8_t i = 0; i < ReceivedHeaderCoeffVector.size(); i++)
	{
		NS_LOG_INFO(" Nc_coeff: " << (int) ReceivedHeaderCoeffVector[i]);
	}

}

void 
SrncHeader::SetEncodedCoeffVector(const std::vector<uint8_t>& vector)
{	
	EncodedCoeffVector = vector;
}

const std::vector<uint8_t>& 
SrncHeader::GetEncodedCoeffVector() const
{
	return EncodedCoeffVector;
}

/* ... */


}

