
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007, 2008 University of Washington
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
 */

#include "ns3/log.h"
#include "ns3/queue.h"
#include "ns3/simulator.h"
#include "ns3/mac48-address.h"
#include "ns3/llc-snap-header.h"
#include "ns3/error-model.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/srnc.h"
#include "ns3/srnc-header.h"
#include "ns3/srnc-tag.h"
#include "ns3/tcp-option-ts.h"
#include "ns3/ipv4-header.h"
#include "ns3/packet.h"
#include "../Header/parameter.h"
// #include "/home/wise/MengHua/0414Auto/NC/parameter.h"



// #include "ns3/point-to-point-net-device.h"
// #include "ns3/point-to-point-channel.h"
#include "ns3/ppp-header.h"
#include <algorithm>
#include <iostream>
#include <fstream>

#include "ns3/srnc-ethernet-device.h"
#include "ns3/srnc-ethernet-channel.h"


using namespace ns3;
using namespace std;

namespace ns3 {
NS_LOG_COMPONENT_DEFINE     ("ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet");
NS_OBJECT_ENSURE_REGISTERED (ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet  );




TypeId ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetTypeId(void)
{
  static TypeId tid = TypeId("ns3::ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet")
    .SetParent      <NetDevice>()
    .SetGroupName  ("ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet")
    .AddConstructor<ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet>()

    .AddAttribute(
      "Mtu", "The MAC-level Maximum Transmission Unit",
      UintegerValue (DEFAULT_MTU),
      MakeUintegerAccessor(
        &ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetMtu,
        &ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetMtu
      ),
      MakeUintegerChecker<uint16_t>()
    )

    .AddAttribute(
      "Address", 
      "The MAC address of this device.",
      Mac48AddressValue (Mac48Address ("ff:ff:ff:ff:ff:ff")),
      MakeMac48AddressAccessor (&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_address),
      MakeMac48AddressChecker()
    )

    .AddAttribute(
      "DataRate", 
      "The default data rate for point to point links",
      DataRateValue(DataRate("32768b/s")),
      MakeDataRateAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_bps),
      MakeDataRateChecker()
    )

    .AddAttribute(
      "ReceiveErrorModel", 
      "The receiver error model used to simulate packet loss",
      PointerValue(),
      MakePointerAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_receiveErrorModel),
      MakePointerChecker<ErrorModel>()
    )

    .AddAttribute(
      "InterframeGap", 
      "The time to wait between packet (frame) transmissions",
      TimeValue(Seconds(0.0)),
      MakeTimeAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_tInterframeGap),
      MakeTimeChecker()
    )

    .AddAttribute(
      "TxQueue", 
      "A queue to use as the transmit queue in the device.",
      PointerValue(),
      MakePointerAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_queue),
      MakePointerChecker<Queue<Packet>>()
    )

    .AddTraceSource(
      "MacTx", 
      "Trace source indicating a packet has arrived "
      "for transmission by this device",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_macTxTrace),
      "ns3::Packet::TracedCallback"
    )

    .AddTraceSource(
      "MacTxDrop", 
      "Trace source indicating a packet has been dropped "
      "by the device before transmission",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_macTxDropTrace),
      "ns3::Packet::TracedCallback"
    )
    
    .AddTraceSource(
      "MacPromiscRx", 
      "A packet has been received by this device, "
      "has been passed up from the physical layer "
      "and is being forwarded up the local protocol stack.  "
      "This is a promiscuous trace,",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_macPromiscRxTrace),
      "ns3::Packet::TracedCallback"
    )

    .AddTraceSource(
      "MacRx", 
      "A packet has been received by this device, "
      "has been passed up from the physical layer "
      "and is being forwarded up the local protocol stack.  "
      "This is a non-promiscuous trace,",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_macRxTrace),
      "ns3::Packet::TracedCallback"
    )

    .AddTraceSource(
      "PhyTxBegin", 
      "Trace source indicating a packet has begun "
      "transmitting over the channel",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_phyTxBeginTrace),
      "ns3::Packet::TracedCallback"
    )

    .AddTraceSource(
      "PhyTxEnd", 
      "Trace source indicating a packet has been "
      "completely transmitted over the channel",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_phyTxEndTrace),
      "ns3::Packet::TracedCallback"
    )

    .AddTraceSource(
      "PhyTxDrop", 
      "Trace source indicating a packet has been "
      "dropped by the device during transmission",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_phyTxDropTrace),
      "ns3::Packet::TracedCallback"
    )
    
    .AddTraceSource(
      "PhyRxEnd", 
      "Trace source indicating a packet has been "
      "completely received by the device",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_phyRxEndTrace),
      "ns3::Packet::TracedCallback"
    )

    .AddTraceSource(
      "PhyRxDrop", 
      "Trace source indicating a packet has been "
      "dropped by the device during reception",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_phyRxDropTrace),
      "ns3::Packet::TracedCallback"
    )

    .AddTraceSource(
      "Sniffer", 
      "Trace source simulating a non-promiscuous packet sniffer "
      "attached to the device",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_snifferTrace),
      "ns3::Packet::TracedCallback"
    )

    .AddTraceSource(
      "PromiscSniffer", 
      "Trace source simulating a promiscuous packet sniffer "
      "attached to the device",
      MakeTraceSourceAccessor(&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::m_promiscSnifferTrace),
      "ns3::Packet::TracedCallback"
    );
    
  return tid;
}


ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet () :
    m_txMachineState (READY),
    m_channel (0),
    m_linkUp (false),
    m_currentPkt (0)
{
  NS_LOG_FUNCTION (this);
}

ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::~ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet ()
{
  NS_LOG_FUNCTION (this);
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::AddHeader (Ptr<Packet> p, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << p << protocolNumber);
  PppHeader ppp;
  ppp.SetProtocol (EtherToPpp (protocolNumber));
  p->AddHeader (ppp);
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::ProcessHeader (Ptr<Packet> p, uint16_t& param)
{
  NS_LOG_FUNCTION (this << p << param);
  PppHeader ppp;
  p->RemoveHeader (ppp);
  param = PppToEther (ppp.GetProtocol ());
  return true;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  m_channel = 0;
  m_receiveErrorModel = 0;
  m_currentPkt = 0;
  m_queue = 0;
  NetDevice::DoDispose ();
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetDataRate (DataRate bps)
{
  NS_LOG_FUNCTION (this);
  m_bps = bps;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetInterframeGap (Time t)
{
  NS_LOG_FUNCTION (this << t.GetSeconds ());
  m_tInterframeGap = t;
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::TransmitStart (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  NS_LOG_LOGIC ("UID is " << p->GetUid () << ")");

  NS_ASSERT_MSG (m_txMachineState == READY, "Must be READY to transmit");
  m_txMachineState = BUSY;
  m_currentPkt = p;
  m_phyTxBeginTrace (m_currentPkt);

  Time txTime = m_bps.CalculateBytesTxTime (p->GetSize ());
  Time txCompleteTime = txTime + m_tInterframeGap;

  NS_LOG_LOGIC ("Schedule TransmitCompleteEvent in " << txCompleteTime.GetSeconds () << "sec");
  Simulator::Schedule (txCompleteTime, &ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::TransmitComplete, this);

  bool result = m_channel->TransmitStart (p, this, txTime);
  if (result == false)
    {
      m_phyTxDropTrace (p);
    }
  return result;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::TransmitComplete (void)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_txMachineState == BUSY, "Must be BUSY if transmitting");
  m_txMachineState = READY;

  NS_ASSERT_MSG (m_currentPkt != 0, "ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::TransmitComplete(): m_currentPkt zero");

  m_phyTxEndTrace (m_currentPkt);
  m_currentPkt = 0;

  Ptr<Packet> p = m_queue->Dequeue ();
  if (p == 0)
    {
      NS_LOG_LOGIC ("No pending packets in device queue after tx complete");
      return;
    }

  m_snifferTrace (p);
  m_promiscSnifferTrace (p);
  TransmitStart (p);
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::Attach (Ptr<ns3_ClassSRNC03_PointToPointChannel> ch)
{
  NS_LOG_FUNCTION (this << &ch);

  m_channel = ch;

  m_channel->Attach (this);

  NotifyLinkUp ();
  return true;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetQueue (Ptr<Queue<Packet> > q)
{
  NS_LOG_FUNCTION (this << q);
  m_queue = q;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetReceiveErrorModel (Ptr<ErrorModel> em)
{
  NS_LOG_FUNCTION (this << em);
  m_receiveErrorModel = em;
}

SrncProcessor m_srncdec[10];
std::vector<Ptr<Packet>> m_RecoveredBuffer[10];
std::list<int> DecodeSuccesslist[10];
std::list<int> DecodeFaillist[10];
bool NC_enable = true;
int de_list = 0;

uint32_t i;

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::Receive (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  uint16_t protocol = 0;
  int LabelNumber = 0;
  Ptr<Node> RxNowNode = GetNode();
  int RxNowNodeID = RxNowNode->GetId();
  int pktSize = packet->GetSize();
  int userNode = 5+2*UserNum;

  
  PppHeader ppHD;
  Ipv4Header iHD;
  TcpHeader tHD;
  if ((RxNowNodeID >= userNode) && pktSize == 1474 && NC_enable)
  {
    // de_list = RxNowNodeID == 9 ? 0:1;
    // std::cout << "RxNowNodeID = " << RxNowNodeID << "\n";
      de_list = RxNowNodeID-userNode;
      // std::cout << "de_list = " << de_list << "\n";
      Ptr<Packet> temp = packet -> Copy();
      Ptr<Packet> temp2 = packet -> Copy();
      temp->RemoveHeader(ppHD);
      temp->PeekHeader (iHD);
      uint32_t m_RecvIncomingLabel = iHD.GetEncodedLabel();
      // std::cout << "[Rx] IncomingLabel = " << m_RecvIncomingLabel << "," << iHD.GetPri() <<"\n";
      temp->AddHeader(ppHD);
      // decode成功的
      if ( (std::find(DecodeSuccesslist[de_list].begin(), DecodeSuccesslist[de_list].end(), m_RecvIncomingLabel) != DecodeSuccesslist[de_list].end()) )
      {
        // std::cout << m_RecvIncomingLabel << " had decoded success, so discard\n";  
        // m_srncdec[de_list].DiscardAllPacketWithLabel(m_RecvIncomingLabel);
      }
      //decode失敗的
      // else if ( (std::find(DecodeFaillist[de_list].begin(), DecodeFaillist[de_list].end(), m_RecvIncomingLabel) != DecodeFaillist[de_list].end()) )
      // {
        // uint32_t maxValue = *max_element(DecodeSuccesslist[de_list].begin(), DecodeSuccesslist[de_list].end());
        // std::cout << "maxValue = " << maxValue << "\n";
        // if (m_RecvIncomingLabel  >  maxValue - Labelthreshold) // 太久以前的label 不要decode
        // {
          // m_srncdec[de_list].Enqueue(temp2);
          // std::cout << m_RecvIncomingLabel << " had decoded fail \n";
          // m_RecoveredBuffer[de_list] = m_srncdec[de_list].DecodeAllWithLabel(m_RecvIncomingLabel);
          // m_srncdec[de_list].DiscardAllPacketWithLabel(m_RecvIncomingLabel);
      //   }
      // }
      //沒decode過的
      else
      {
        // std::cout << "de_list = " << de_list << "\n"; 
        m_srncdec[de_list].Enqueue(temp2); 
        uint32_t m_smallestUnprocessedLabel = m_srncdec[de_list].FindSmallestLabelInBuffer();  //find the smallest label
        LabelNumber = m_srncdec[de_list].FindPacketNumberWithLabel(m_smallestUnprocessedLabel); // how many packet with the smallest label
        // std::cout << "NC queue = " <<  m_srncdec[de_list].GetEnqueueBufferSize() << ", "<<de_list<< ", "<<Simulator::Now ().GetSeconds () << "\n";
        // std::cout << "[Rx] LabelNumber = " << LabelNumber << ", minlabel = "  << m_smallestUnprocessedLabel << ", now label = "<<m_RecvIncomingLabel<<"\n";
        int m_smallestLabelEncodeNum = m_srncdec[de_list].FindPacketEncodedNumberWithLabel(m_smallestUnprocessedLabel);   // the enccode number with the smallest label
        if (LabelNumber >= m_smallestLabelEncodeNum )  
        {
          // std::cout <<" De_List =" << de_list << "\n";
          m_RecoveredBuffer[de_list] = m_srncdec[de_list].DecodeAllWithLabel(m_smallestUnprocessedLabel);
          m_srncdec[de_list].DiscardAllPacketWithLabel(m_smallestUnprocessedLabel);
          DecodeSuccesslist[de_list].push_back(m_smallestUnprocessedLabel);
          // if (de_list==0 && Simulator::Now ().GetSeconds ()  >2.95)
          // {
          //   std::cout << Simulator::Now ().GetSeconds ()  << ", " << m_smallestUnprocessedLabel << "\n";
          // }

          // if (m_smallestUnprocessedLabel != 0 ){
          //   ofstream fout;
          //   fout.open( "0_decodeLabel.txt" ,ios::app);
          //   fout << Simulator::Now ().GetSeconds ()  <<  " "  << m_smallestUnprocessedLabel << "\n";
          //   fout.close();
          // }
        }
      }//else



// Decode Rule
      // uint32_t m_smallestUnprocessedLabel = m_srncdec[de_list].FindSmallestLabelInBuffer();
      // LabelNumber = m_srncdec[de_list].FindPacketNumberWithLabel(m_smallestUnprocessedLabel);
      // // std::cout << "[Rx] LabelNumber = " << LabelNumber << " minlabel = "  << m_smallestUnprocessedLabel << "now label = "<<m_RecvIncomingLabel<<"\n";
      // int m_smallestLabelEncodeNum = m_srncdec[de_list].FindPacketEncodedNumberWithLabel(m_smallestUnprocessedLabel);
      // // std::cout << "m_smallestLabelEncodeNum = " << m_smallestLabelEncodeNum << "\n";
      // if (LabelNumber >= m_smallestLabelEncodeNum)  
      // {
      //   // std::cout <<" De_List =" << de_list << "\n";
      //   m_RecoveredBuffer[de_list] = m_srncdec[de_list].DecodeAllWithLabel(m_smallestUnprocessedLabel);
      //   m_srncdec[de_list].DiscardAllPacketWithLabel(m_smallestUnprocessedLabel);
      //   DecodeSuccesslist[de_list].push_back(m_smallestUnprocessedLabel);
      // }
      // else if (m_RecvIncomingLabel  > m_smallestUnprocessedLabel + Labelthreshold)  // 最小Label未滿3個且收到超過Labelthreshold的封包了,因此提前decode
      // {
      //   std::cout<< Simulator::Now().GetSeconds() <<" Got Labelthreshold, Decode Immediately! Label= " << m_smallestUnprocessedLabel << "\n";
      //   m_RecoveredBuffer[de_list] = m_srncdec[de_list].DecodeAllWithLabel(m_smallestUnprocessedLabel);
      //   m_srncdec[de_list].DiscardAllPacketWithLabel(m_smallestUnprocessedLabel);
      //   DecodeFaillist[de_list].push_back(m_smallestUnprocessedLabel);
      // }
      if (m_RecoveredBuffer[de_list].size() != 0 )
      {
        for ( i = 0; i < m_RecoveredBuffer[de_list].size(); i++)
        {
          Ptr<Packet> packet = m_RecoveredBuffer[de_list].at(i);
          if (m_receiveErrorModel && m_receiveErrorModel->IsCorrupt (packet) ) 
          {
            // 
            // If we have an error model and it indicates that it is time to lose a
            // corrupted packet, don't forward this packet up, let it go.
            //
            m_phyRxDropTrace (packet);
          }
          else 
          {
            // 
            // Hit the trace hooks.  All of these hooks are in the same place in this 
            // device because it is so simple, but this is not usually the case in
            // more complicated devices.
            //
            m_snifferTrace (packet);
            m_promiscSnifferTrace (packet);
            m_phyRxEndTrace (packet);

            //
            // Trace sinks will expect complete packets, not packets without some of the
            // headers.
            //
            Ptr<Packet> originalPacket = packet->Copy ();

            //
            // Strip off the point-to-point protocol header and forward this packet
            // up the protocol stack.  Since this is a simple point-to-point link,
            // there is no difference in what the promisc callback sees and what the
            // normal receive callback sees.
            //
            ProcessHeader (packet, protocol);

            if (!m_promiscCallback.IsNull ())
            {
              m_macPromiscRxTrace (originalPacket);
              m_promiscCallback (this, packet, protocol, GetRemote (), GetAddress (), NetDevice::PACKET_HOST);
            }

            m_macRxTrace (originalPacket);
            m_rxCallback (this, packet, protocol, GetRemote ());
          }
        }
        m_RecoveredBuffer[de_list].clear();
      }

  }// nodeID
  else 
  {
    if (m_receiveErrorModel && m_receiveErrorModel->IsCorrupt (packet) ) 
      {
        // 
        // If we have an error model and it indicates that it is time to lose a
        // corrupted packet, don't forward this packet up, let it go.
        //
        m_phyRxDropTrace (packet);
      }
    else 
      {
        // 
        // Hit the trace hooks.  All of these hooks are in the same place in this 
        // device because it is so simple, but this is not usually the case in
        // more complicated devices.
        //
        m_snifferTrace (packet);
        m_promiscSnifferTrace (packet);
        m_phyRxEndTrace (packet);

        //
        // Trace sinks will expect complete packets, not packets without some of the
        // headers.
        //
        Ptr<Packet> originalPacket = packet->Copy ();

        //
        // Strip off the point-to-point protocol header and forward this packet
        // up the protocol stack.  Since this is a simple point-to-point link,
        // there is no difference in what the promisc callback sees and what the
        // normal receive callback sees.
        //
        ProcessHeader (packet, protocol);

        if (!m_promiscCallback.IsNull ())
          {
            m_macPromiscRxTrace (originalPacket);
            m_promiscCallback (this, packet, protocol, GetRemote (), GetAddress (), NetDevice::PACKET_HOST);
          }

        m_macRxTrace (originalPacket);
        m_rxCallback (this, packet, protocol, GetRemote ());
      }
  }
}

Ptr<Queue<Packet> >
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetQueue (void) const
{ 
  NS_LOG_FUNCTION (this);
  return m_queue;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::NotifyLinkUp (void)
{
  NS_LOG_FUNCTION (this);
  m_linkUp = true;
  m_linkChangeCallbacks ();
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION (this);
  m_ifIndex = index;
}

uint32_t
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetIfIndex (void) const
{
  return m_ifIndex;
}

Ptr<Channel>
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetChannel (void) const
{
  return m_channel;
}

//
// This is a point-to-point device, so we really don't need any kind of address
// information.  However, the base class NetDevice wants us to define the
// methods to get and set the address.  Rather than be rude and assert, we let
// clients get and set the address, but simply ignore them.

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetAddress (Address address)
{
  NS_LOG_FUNCTION (this << address);
  m_address = Mac48Address::ConvertFrom (address);
}

Address
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetAddress (void) const
{
  return m_address;
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::IsLinkUp (void) const
{
  NS_LOG_FUNCTION (this);
  return m_linkUp;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::AddLinkChangeCallback (Callback<void> callback)
{
  NS_LOG_FUNCTION (this);
  m_linkChangeCallbacks.ConnectWithoutContext (callback);
}

//
// This is a point-to-point device, so every transmission is a broadcast to
// all of the devices on the network.
//
bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::IsBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  return true;
}

//
// We don't really need any addressing information since this is a 
// point-to-point device.  The base class NetDevice wants us to return a
// broadcast address, so we make up something reasonable.
//
Address
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::IsMulticast (void) const
{
  NS_LOG_FUNCTION (this);
  return true;
}

Address
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_LOG_FUNCTION (this);
  return Mac48Address ("01:00:5e:00:00:00");
}

Address
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this << addr);
  return Mac48Address ("33:33:00:00:00:00");
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION (this);
  return true;
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::IsBridge (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}

// 
SrncProcessor m_srncprocessor[10];
std::list<Ptr<Packet>> coded_buffer[10];
std::list<Ptr<Packet>> redud_buffer[10];
double encodetime = 1.401; // time to encode
int m_LabelK[10] = {0};
uint16_t m_TimeVal=100; //us
bool b[10] = {0};
// uint32_t EncodeMaxNum = 1;  // 1 is PD
// bool NC_enable = false;
// bool NC_enable = false;
Ipv4Address address_list1[10];
std::vector <uint32_t> RecvTcpSeqVector[10];
std::vector <uint32_t> CodingTcpSeqVector[10]; //sequence number
std::vector <uint32_t> RecvTcpTsVector[10];
std::vector <uint32_t> CodingTcpTsVector[10];//time stamp
std::vector <uint32_t> RecvTcpEhVector[10];
std::vector <uint32_t> CodingTcpEhVector[10];//time stamp echo


void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::NCpreprocess(Ptr<Packet> packet, int list)
{
    NS_LOG_FUNCTION (this);
    Ptr<Packet> temp = packet -> Copy();
    Ptr<Packet> p = packet -> Copy();

    m_srncprocessor[list].Enqueue(temp);
    // std::cout << "list = " << list << " is Encode" << "\n";

  // 如果準備encode的數量超過預設值則直接encode,預防一次encode大量封包
    if (m_srncprocessor[list].GetEnqueueBufferSize() >= EncodeMaxNum)
    {
      Simulator::ScheduleNow (&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::UpdateTimeInterval, this, list);
    }
  // 由時間區間觸發的encode(正常都應該走這)
    if(coded_buffer[list].size() == 0 && b[list] == false)  //第一次會進來
    {
      Simulator::Schedule (MicroSeconds (m_TimeVal),&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::StartTimeInterval, this, list);
      b[list] = true;
    }
    //----------------------------------------------------------------------
    //Get TCP sequence number. (skip the TCP control message)
    PppHeader ppHD;
    Ipv4Header ipv4header;
    TcpHeader tcpheader;
    p->RemoveHeader (ppHD);
    p->RemoveHeader (ipv4header);
    p->PeekHeader (tcpheader);
    SequenceNumber32 sequencenumber32;
    sequencenumber32 = tcpheader.GetSequenceNumber();
    RecvTcpSeqVector[list].push_back((uint32_t)sequencenumber32.GetValue ());
    p->AddHeader (ipv4header);
    p->AddHeader (ppHD);
}


bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::Send (
  Ptr<Packet> packet, 
  const Address &dest, 
  uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet << dest << protocolNumber);
  NS_LOG_LOGIC ("p=" << packet << ", dest=" << &dest);
  NS_LOG_LOGIC ("UID is " << packet->GetUid ());

  //
  // If IsLinkUp() is false it means there is no channel to send any packet 
  // over so we just hit the drop trace on the packet and return an error.
  //
  if (IsLinkUp () == false)
    {
      m_macTxDropTrace (packet);
      return false;
    }

  //
  // Stick a point to point protocol header on the packet in preparation for
  // shoving it out the door.
  //
    AddHeader (packet, protocolNumber);
    m_macTxTrace (packet);
  //
  // when packets go into the Gatway node(Node ID = 1),start Encode.
  //
  
  Ptr<Node> NowNode = GetNode();
  Ptr<Packet> NCpacket;
  int NowNodeID = NowNode->GetId();
  // std::cout << "NowNodeID = " << NowNodeID << "\n";
  double time =  Simulator::Now().GetSeconds();

  // if (NowNodeID == 5 && packet -> GetSize() > 1454 && NC_enable){
  //     ofstream fout;
  //     fout.open( "STA1.txt" ,ios::app);
  //     fout << Simulator::Now ().GetSeconds ()  <<  "    1\n";
  //     fout.close();

  // }
  // else if (NowNodeID == 6 && packet -> GetSize() > 1454 && NC_enable){
  //     ofstream fout;
  //     fout.open( "STA2.txt" ,ios::app);
  //     fout << Simulator::Now ().GetSeconds ()  <<  "    2\n";
  //     fout.close();
  // }


  if (time > encodetime && NowNodeID == 1 && packet -> GetSize() == 1454 && NC_enable) //需要encode的封包(會用其他Function傳出去),
  {                                                                         //NowNodeID == 0的原因是從0到1的途中encode
    Ptr<Packet> temp = packet -> Copy();
    Ptr<Packet> p = packet -> Copy();
    PppHeader ppHD;
    Ipv4Header ipv4header;
    TcpHeader tcpheader;
    p->RemoveHeader (ppHD);
    p->PeekHeader (ipv4header);
    p->AddHeader (ppHD);
    // std::cout << ipv4header.GetDestination();

      // std::cout << address_list1;
    for (int list = 0; list < 10; list++)
    {
      if (address_list1[list] == ipv4header.GetDestination())
      {
        // std::cout << "the same" << ipv4header.GetDestination() << "\n";
        // std::cout << "Choose = " << list << "\n";
        NCpreprocess(packet,list);
        break;
      }
      else if (address_list1[list] != ipv4header.GetDestination() && address_list1[list] == "102.102.102.102")  
      {        
        address_list1[list] = ipv4header.GetDestination();
        NCpreprocess(packet,list);
        break;
      }
    }




  //   m_srncprocessor.Enqueue(temp);
  //   // std::cout << "NowNodeID = " << NowNodeID << "\n";

  // // 如果準備encode的數量超過預設值則直接encode,預防一次encode大量封包
  //   if (m_srncprocessor.GetEnqueueBufferSize() >= EncodeMaxNum)
  //   {
  //     Simulator::ScheduleNow (&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::UpdateTimeInterval, this);
  //   }
  // // 由時間區間觸發的encode(正常都應該走這)
  //   if(coded_buffer.size() == 0 && b == false)  //第一次會進來
  //   {
  //     Simulator::Schedule (MicroSeconds (m_TimeVal),&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::StartTimeInterval, this);
  //     b = true;
  //   }

    
    //----------------------------------------------------------------------
    //Get TCP sequence number. (skip the TCP control message)
    // PppHeader ppHD;
    // Ipv4Header ipv4header;
    // TcpHeader tcpheader;
    // p->RemoveHeader (ppHD);
    // p->RemoveHeader (ipv4header);
    // p->PeekHeader (tcpheader);
    // SequenceNumber32 sequencenumber32;
    // sequencenumber32 = tcpheader.GetSequenceNumber();
    // RecvTcpSeqVector.push_back((uint32_t)sequencenumber32.GetValue ());
    // p->AddHeader (ipv4header);
    // p->AddHeader (ppHD);
    //----------------------------------------------------------------------
    return true;
  }
  else  // 不需要Encode的封包
  {    
    //
    // We should enqueue and dequeue the packet to hit the tracing hooks.
    //
    if (m_queue->Enqueue (packet))
      {
        //
        // If the channel is ready for transition we send the packet right now
        // 
        if (m_txMachineState == READY)
          {
            packet = m_queue->Dequeue ();
            m_snifferTrace (packet);
            m_promiscSnifferTrace (packet);
            bool ret = TransmitStart (packet);
            return ret;
          }
        return true;
      }
  }
 

  // Enqueue may fail (overflow)

  m_macTxDropTrace (packet);
  return false;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::UpdateTimeInterval(int list)
{
  // std::cout << "update time = " << Simulator::Now().GetSeconds() << "\n" ;
  NS_LOG_FUNCTION (this);
  Ptr<Packet> NCpacket;
  Ptr<Packet> NCpacket2;
  UpdateLabelK(list);//labelk++
  m_srncprocessor[list].SetStoreLabel(m_LabelK[list]);//encoded label

  coded_buffer[list].clear();
  redud_buffer[list].clear();

  CodingTcpSeqVector[list].clear();
  CodingTcpTsVector[list].clear();
  CodingTcpEhVector[list].clear();
  CodingTcpSeqVector[list] = RecvTcpSeqVector[list];
  CodingTcpTsVector[list] = RecvTcpTsVector[list];
  CodingTcpEhVector[list] = RecvTcpEhVector[list];
  RecvTcpSeqVector[list].clear();
  RecvTcpTsVector[list].clear();
  RecvTcpEhVector[list].clear();

  //NC process
  coded_buffer[list] = m_srncprocessor[list].NetworkCoding(list);
  uint32_t HighPrioCount = coded_buffer[list].size();
  // std::cout << "Now label = " << m_LabelK << "\n";
  redud_buffer[list] = m_srncprocessor[list].NetworkCodingRedundancy(HighPrioCount,list);//低優先權封包要盡量利用可用頻寬傳送
  // redud_buffer[list] = m_srncprocessor[list].NetworkCodingRedundancy(0);
  while (coded_buffer[list].size() > 0)
  {
      NCpacket = coded_buffer[list].front();
      DoTransmitNcPacket(NCpacket,list);
      coded_buffer[list].pop_front();
  }

  while (redud_buffer[list].size() > 0)
  {
      NCpacket2 = redud_buffer[list].front();
      DoTransmitNcPacket(NCpacket2,list);
      redud_buffer[list].pop_front();
  }

  m_srncprocessor[list].ClearEnqueueBuffer();

  // Simulator::Schedule (MicroSeconds (m_TimeVal), &ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::UpdateTimeInterval, this);
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::StartTimeInterval(int list)
{
  NS_LOG_FUNCTION (this);
  // 定時呼叫自己,並執行encode動作
  Simulator::ScheduleNow (&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::UpdateTimeInterval,this, list);
  Simulator::Schedule (MicroSeconds (m_TimeVal),&ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::StartTimeInterval,this, list);
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetTimeInterval(uint16_t TimeVal)
{
  NS_LOG_FUNCTION (this);
  m_TimeVal = TimeVal;
}

uint16_t
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetTimeInterval(void) const
{
  NS_LOG_FUNCTION (this);
  return m_TimeVal;
}
void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::UpdateLabelK(int list)
{
  m_LabelK[list]++;
  // std::cout << "time = " << Simulator::Now().GetSeconds() << "Label = " << m_LabelK << "\n";
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetLabelK(uint32_t labelk,int list)
{
  NS_LOG_FUNCTION (this);
  m_LabelK[list] = labelk;
}

uint32_t
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetLabelK(int list) const
{
  //NS_LOG_FUNCTION (this);
  return m_LabelK[list];
}


void 
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::DoTransmitNcPacket(Ptr<Packet> NcPacket,int list)
{
  //Add tag success
  SrncTag srncTag;
  // SrncHeader srncHeader;
  PppHeader ppHD;
  Ipv4Header iHD;
  NcPacket -> RemoveHeader(ppHD);
  NcPacket->PeekHeader(iHD);
  uint8_t EncodedNumberTmp = iHD.GetEncodedPacketNumber();
  srncTag.SetUseUdp(0);
  srncTag.SetEncodedPacketNumber(EncodedNumberTmp);
  srncTag.SetTcpSeqVector(CodingTcpSeqVector[list]);
  // srncTag.SetTcpTsVector(CodingTcpTsVector);
  // srncTag.SetTcpEhVector(CodingTcpEhVector);
  NcPacket -> AddHeader(ppHD);
  NcPacket -> AddByteTag(srncTag); 

  //傳送封包
  if (m_queue->Enqueue (NcPacket))
    {
      //
      // If the channel is ready for transition we send the packet right now
      // 
      if (m_txMachineState == READY)
        {
          NcPacket = m_queue->Dequeue ();
          m_snifferTrace (NcPacket);
          m_promiscSnifferTrace (NcPacket);
          TransmitStart (NcPacket);
        }
    }
      // Enqueue may fail (overflow)

  m_macTxDropTrace (NcPacket);
}





bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SendFrom (Ptr<Packet> packet, 
                                 const Address &source, 
                                 const Address &dest, 
                                 uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet << source << dest << protocolNumber);
  return false;
}

Ptr<Node>
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetNode (void) const
{
  return m_node;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);
  m_node = node;
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::NeedsArp (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  m_rxCallback = cb;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
{
  m_promiscCallback = cb;
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SupportsSendFrom (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}

void
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::DoMpiReceive (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  Receive (p);
}

Address 
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetRemote (void) const
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_channel->GetNDevices () == 2);
  for (std::size_t i = 0; i < m_channel->GetNDevices (); ++i)
    {
      Ptr<NetDevice> tmp = m_channel->GetDevice (i);
      if (tmp != this)
        {
          return tmp->GetAddress ();
        }
    }
  NS_ASSERT (false);
  // quiet compiler.
  return Address ();
}

bool
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::SetMtu (uint16_t mtu)
{
  NS_LOG_FUNCTION (this << mtu);
  m_mtu = mtu;
  return true;
}

uint16_t
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::GetMtu (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mtu;
}

uint16_t
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::PppToEther (uint16_t proto)
{
  NS_LOG_FUNCTION_NOARGS();
  switch(proto)
    {
    case 0x0021: return 0x0800;   //IPv4
    case 0x0057: return 0x86DD;   //IPv6
    default: NS_ASSERT_MSG (false, "PPP Protocol number not defined!");
    }
  return 0;
}

uint16_t
ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::EtherToPpp (uint16_t proto)
{
  NS_LOG_FUNCTION_NOARGS();
  switch(proto)
    {
    case 0x0800: return 0x0021;   //IPv4
    case 0x86DD: return 0x0057;   //IPv6
    default: NS_ASSERT_MSG (false, "PPP Protocol number not defined!");
    }
  return 0;
}


} // namespace ns3
