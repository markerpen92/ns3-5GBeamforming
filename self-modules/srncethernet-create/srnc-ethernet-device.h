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

#ifndef NS3_CLASS_SRNC01_POINT_TO_POINT_NET_DEVICE_SRNCEthernet
#define NS3_CLASS_SRNC01_POINT_TO_POINT_NET_DEVICE_SRNCEthernet

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/ptr.h"
#include "ns3/mac48-address.h"
#include "ns3/error-model.h"
#include "ns3/queue.h"
#include "ns3/srnc.h"

#include "ns3/srnc-ethernet-channel.h"

using namespace std;


namespace ns3 {

template <typename Item> class Queue;
class ns3_ClassSRNC03_PointToPointChannel;
class ErrorModel; 

class ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet : public NetDevice
{
  public:
    static TypeId GetTypeId                                (void);
    ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet         ();
    virtual ~ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet();

  //protected : 
    /*-------fucntion for ::Init-------------------------------------------------------------------------------*/
    // /*funct*/ virtual void Initialize(vector<int> src_nodesID_list , vector<int> dst_nodesID_list , double encode_starttime);
    //   /*param*/ SrncProcessor*       SRNCDecodeList             ;
    //   /*param*/ vector<Ptr<Packet>>* RecoveredBuffer            ;
    //   /*param*/ list<int>*           DecodeSuccesslist          ;
    //   /*param*/ list<int>*           DecodeFaillist             ;
    //   /*param*/ vector<int>*         SrcNodesIDList             ;
    //   /*param*/ vector<int>*         DstNodesIDList             ;
    //   /*param*/ vector<Ipv4Address>* DstNodesAddressList        ;
    //   /*param*/ bool                 NetworkCodingEnable = true ;
    //   /*param*/ int                  ListSize   =   0           ;
    //   /*param*/ int                  ExpectedTxPktSize = 1454   ;
    //   /*param*/ int                  ExpectedRxPktSize = 1474   ;
    //   /*param*/ uint16_t             ProtocolNumber =   0       ;
    //   /*param*/ double               EncodeStartTime = 0.0      ;    
    /*---------------------------------------------------------------------------------------------------------*/
    


    /*-------fucntion for ::Send-------------------------------------------------------------------------------*/
    /*funct*/ virtual bool         Send                    (Ptr<Packet> pkt , const Address& dst , uint16_t protocol_num);
      /*funct*/ //virtual int        SetDstNodesAddressList  (Ptr<Packet> packet)                                          ;
      /*funct*/ //virtual void       NetworkCodingPreprocess (Ptr<Packet> packet , int encode_id)                          ;
      /*funct*/ //virtual void       SetEncodeStartTime      (double time)                                                 ;
    /*---------------------------------------------------------------------------------------------------------*/



    /*-------fucntion for ::Receive----------------------------------------------------------------------------*/
    /*funct*/ virtual void         Receive                      (Ptr<Packet> pkt)                          ; 
      /*funct*/ //virtual void       LoadLabelPktIntoRecoverBuffer(int RxNodeInDstList_IDX , Ptr<Packet> pkt);
      /*funct*/ //virtual void       ProcessTheDecodedPkt         (Ptr<Packet> pkt)                          ;
    /*---------------------------------------------------------------------------------------------------------*/

    virtual bool         SendFrom              (Ptr<Packet> pkt , const Address& src , const Address& dst , uint16_t protocol_num);
    void                 SetDataRate           (   DataRate bps   )              ;
    void                 SetInterframeGap      (      Time t      )              ;
    bool                 Attach                (Ptr<ns3_ClassSRNC03_PointToPointChannel> ch)     ;
    void                 SetQueue              (Ptr<Queue<Packet>>    queue)     ;
    Ptr<Queue<Packet>>   GetQueue              (void)                       const;
    void                 SetReceiveErrorModel  (Ptr<ErrorModel> em)              ;
    
    virtual void         SetIfIndex            (const uint32_t index)            ;
    virtual uint32_t     GetIfIndex            (void)                       const;
    virtual Ptr<Channel> GetChannel            (void)                       const;
    virtual void         SetAddress            (Address    address)              ;
    virtual Address      GetAddress            (void)                       const;
    virtual bool         SetMtu                (const uint16_t mtu)              ;
    virtual uint16_t     GetMtu                (void)                       const;
    virtual bool         IsLinkUp              (void)                       const;
    virtual void         AddLinkChangeCallback (Callback<void> callback)         ;
    virtual bool         IsBroadcast           (void)                       const;
    virtual Address      GetBroadcast          (void)                       const;
    virtual bool         IsMulticast           (void)                       const;
    virtual Address      GetMulticast          (Ipv4Address multicastGroup) const;
    virtual bool         IsPointToPoint        (void)                       const;
    virtual bool         IsBridge              (void)                       const;

    virtual Ptr<Node> GetNode (void) const;
    virtual void SetNode (Ptr<Node> node);
    virtual bool NeedsArp (void) const;
    virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
    virtual Address GetMulticast (Ipv6Address addr) const;
    virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
    virtual bool SupportsSendFrom (void) const;

    void DoMpiReceive (Ptr<Packet> p);

    // SrncProcessor*       SRNCDecodeList             ;
    // vector<Ptr<Packet>>* RecoveredBuffer            ;
    // list<int>*           DecodeSuccesslist          ;
    // list<int>*           DecodeFaillist             ;
    // vector<int>*         SrcNodesIDList             ;
    // vector<int>*         DstNodesIDList             ;
    // vector<Ipv4Address>* DstNodesAddressList        ;
    // bool                 NetworkCodingEnable = true ;
    // int                  ListSize            =  0   ;
    // int                  ExpectedTxPktSize   = 1454 ;
    // int                  ExpectedRxPktSize   = 1474 ;
    // uint16_t             ProtocolNumber      =  0   ;
    // double               EncodeStartTime     = 0.0  ; 

    ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet& operator = (const ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet &o);

    ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet (const ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet &o);

    virtual void DoDispose (void);

    Address GetRemote (void) const;

    void AddHeader (Ptr<Packet> p, uint16_t protocolNumber);

    bool ProcessHeader (Ptr<Packet> p, uint16_t& param);

    bool TransmitStart (Ptr<Packet> p);

    void TransmitComplete (void);

    void NotifyLinkUp (void);

    void StartTimeInterval(int list);
  
    void UpdateTimeInterval(int list);

    void SetTimeInterval(uint16_t TimeVal);

    void NCpreprocess(Ptr<Packet> packet, int list);

    uint16_t GetTimeInterval(void) const;

    void UpdateLabelK(int list);

    void SetLabelK(uint32_t labelk,int list);

    uint32_t GetLabelK(int list) const;

    void DoTransmitNcPacket(Ptr<Packet> NcPacket,int list);


    enum TxMachineState
    {
      READY,   /**< The transmitter is ready to begin transmission of a packet */
      BUSY     /**< The transmitter is busy transmitting a packet */
    };
    TxMachineState m_txMachineState;

    DataRate       m_bps;

    Time           m_tInterframeGap;

    Ptr<ns3_ClassSRNC03_PointToPointChannel> m_channel;

    Ptr<Queue<Packet> > m_queue;

    Ptr<ErrorModel> m_receiveErrorModel;

    TracedCallback<Ptr<const Packet> > m_macTxTrace;

    TracedCallback<Ptr<const Packet> > m_macTxDropTrace;

    TracedCallback<Ptr<const Packet> > m_macPromiscRxTrace;

    TracedCallback<Ptr<const Packet> > m_macRxTrace;

    TracedCallback<Ptr<const Packet> > m_macRxDropTrace;

    TracedCallback<Ptr<const Packet> > m_phyTxBeginTrace;

    TracedCallback<Ptr<const Packet> > m_phyTxEndTrace;

    TracedCallback<Ptr<const Packet> > m_phyTxDropTrace;

    TracedCallback<Ptr<const Packet> > m_phyRxBeginTrace;

    TracedCallback<Ptr<const Packet> > m_phyRxEndTrace;

    TracedCallback<Ptr<const Packet> > m_phyRxDropTrace;

    TracedCallback<Ptr<const Packet> > m_snifferTrace;

    TracedCallback<Ptr<const Packet> > m_promiscSnifferTrace;

    Ptr<Node> m_node;         //!< Node owning this NetDevice
    Mac48Address m_address;   //!< Mac48Address of this NetDevice
    NetDevice::ReceiveCallback m_rxCallback;   //!< Receive callback
    NetDevice::PromiscReceiveCallback m_promiscCallback;  //!< Receive callback
                                                          //   (promisc data)
    uint32_t m_ifIndex; //!< Index of the interface
    bool m_linkUp;      //!< Identify if the link is up or not
    TracedCallback<> m_linkChangeCallbacks;  //!< Callback for the link change event

    static const uint16_t DEFAULT_MTU = 1500; //!< Default MTU

    uint32_t m_mtu;

    Ptr<Packet> m_currentPkt; //!< Current packet processed

    static uint16_t PppToEther (uint16_t protocol);

    static uint16_t EtherToPpp (uint16_t protocol);
};

} // namespace ns3

#endif /* POINT_TO_POINT_NET_DEVICE_H */
