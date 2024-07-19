/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "common-tools.h"


#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"


using namespace std;
using namespace ns3;

namespace ns3 {

/* ... */

    Ptr<OutputStreamWrapper>
    CreateSlsTraceStream (string fileName)
    {
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> slsStream = ascii.CreateFileStream (fileName + ".csv");
    *slsStream->GetStream () << "SRC_ID,DST_ID,TRACE_IDX,SECTOR_ID,ANTENNA_ID,ROLE,BSS_ID,Timestamp" << std::endl;
    return slsStream;
    }

    template <typename T>
    string to_string_with_precision (const T a_value, const int n)
    {
    std::ostringstream out;
    out.precision (n);
    out << std::fixed << a_value;
    return out.str ();
    }

    double
    CalculateSingleStreamThroughput (Ptr<PacketSink> sink, uint64_t &lastTotalRx, double &averageThroughput)
    {
    double thr = (sink->GetTotalRx() - lastTotalRx) * (double) 8/1e5;     /* Convert Application RX Packets to MBits. */
    lastTotalRx = sink->GetTotalRx ();
    averageThroughput += thr;
    return thr;
    }

    long
    CalculateSingleStreamDelay (Ptr<PacketSink> sink, Time &totalDelay, uint64_t &pktNum)
    {
    Time latency = sink->GetAverageDelay();
    uint64_t Nowpkts = sink->GetTotalReceivedPackets();
    long diffdelay;
    long diff = latency.GetMicroSeconds()*Nowpkts - totalDelay.GetMicroSeconds()*pktNum;
    long diffNum = Nowpkts - pktNum;
    if (diffNum == 0)
    {
        diffdelay = 0 ;
    }
    else
    {
        diffdelay = diff/diffNum;
    }
    
    //   std::cout << "latency*Nowpkts = " <<  latency.GetMicroSeconds()
    //         << ", totalDelay*pktNum = " << totalDelay.GetMicroSeconds()
    //         << ", diffNum = " << diff << " ";

    // std::cout << "diffdelay = " << diffdelay ;
    totalDelay = latency;
    pktNum = Nowpkts;

    return diffdelay;
    }

    /**
    * Print application layer and flow monitor statistics.
    * \param monitor The flow monitor helper class
    * \param mointor Poiner to the flow monitor engine.
    * \param simulationTime The simulation time in seconds.
    */
    void
    PrintFlowMonitorStatistics (FlowMonitorHelper &flowmon, Ptr<FlowMonitor> monitor, double simulationTime)
    {
    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
        {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")" << std::endl;
        std::cout << "  Tx Packets: " << i->second.txPackets << std::endl;
        std::cout << "  Tx Bytes:   " << i->second.txBytes << std::endl;
        std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / (simulationTime * 1e6)  << " Mbps" << std::endl;
        std::cout << "  Rx Packets: " << i->second.rxPackets << std::endl;
        std::cout << "  Rx Bytes:   " << i->second.rxBytes << std::endl;
        std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (simulationTime * 1e6)  << " Mbps" << std::endl;
        }
    }

    /**
    * Print application layer and flow monitor statistics.
    * \param monitor The flow monitor helper class
    * \param mointor Poiner to the flow monitor engine.
    * \param communicationPairList List of communication pairs in the network.
    * \param applicationType The type of the application (onoff/bulk).
    * \param simulationTime The simulation time in seconds.
    */
    void
    PrintApplicationLayerAndFlowMonitorStatistics (FlowMonitorHelper &flowmon, Ptr<FlowMonitor> monitor,
                                                CommunicationPairList &communicationPairList, string applicationType,
                                                double simulationTime)
    {
    PrintFlowMonitorStatistics (flowmon, monitor, simulationTime);

    /* Print Application Layer Results Summary */
    std::cout << "\nApplication Layer Statistics:" << std::endl;
    Ptr<OnOffApplication> onoff;
    Ptr<BulkSendApplication> bulk;
    uint16_t communicationLinks = 1;
    for (CommunicationPairList_I it = communicationPairList.begin (); it != communicationPairList.end (); it++)
        {
        std::cout << "Communication Link (" << communicationLinks << ") Statistics:" << std::endl;
        if (applicationType == "onoff")
            {
            onoff = StaticCast<OnOffApplication> (it->second.srcApp);
            std::cout << "  Tx Packets: " << onoff->GetTotalTxPackets () << std::endl;
            std::cout << "  Tx Bytes:   " << onoff->GetTotalTxBytes () << std::endl;
            }
        else
            {
            bulk = StaticCast<BulkSendApplication> (it->second.srcApp);
            std::cout << "  Tx Packets: " << bulk->GetTotalTxPackets () << std::endl;
            std::cout << "  Tx Bytes:   " << bulk->GetTotalTxBytes () << std::endl;
            }
        Ptr<PacketSink> packetSink = StaticCast<PacketSink> (it->second.packetSink);
        std::cout << "  Rx Packets: " << packetSink->GetTotalReceivedPackets () << std::endl;
        std::cout << "  Rx Bytes:   " << packetSink->GetTotalRx () << std::endl;
        std::cout << "  Throughput: " << packetSink->GetTotalRx () * 8.0 / ((simulationTime - it->second.startTime.GetSeconds ()) * 1e6)
                    << " Mbps" << std::endl;
        communicationLinks++;
        }
    }

    /**
    * Configure TCP Options.
    * \param tcpVariant The name of the used TCP variant
    * \param segmentSize The TCP segment size in bytes.
    * \param bufferSize The size of the TCP send and receive buffers in bytes.
    */
    void
    ConfigureTcpOptions (string tcpVariant, uint32_t segmentSize, uint32_t bufferSize)
    {
    /* Select TCP variant */
    TCP_VARIANTS_I iter = TCP_VARIANTS_LIST.find (tcpVariant);
    NS_ASSERT_MSG (iter != TCP_VARIANTS_LIST.end (), "Cannot find Tcp Variant");
    TypeId tid = TypeId::LookupByName (iter->second);
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (tid));
    if (tcpVariant.compare ("Westwood") == 0)
        {
        Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOOD));
        Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
        }
    else if (tcpVariant.compare ("WestwoodPlus") == 0)
        {
        Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
        Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue (TcpWestwood::TUSTIN));
        }

    /* Configure TCP Segment Size */
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (segmentSize));
    Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (bufferSize));
    Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (bufferSize));
    }

    /**
    * Disable RTS/CTS and Fragmentation.
    * \param enableRts Flag to indicate if we want to enable RTS/CTS hansdhake before transmission.
    * \param rtsCtsThreshold If the size of the PSDU is bigger than this value,
    * we use an RTS/CTS handshake before sending the data frame.
    * Note: This value will not have any effect on some rate control algorithms.
    * \param enableFragmentation Flag to indicate if we want to fragment PSDU before transmission.
    * \param fragmentationThreshold If the size of the PSDU is bigger than this value, we fragment it such that
    * the size of the fragments are equal or smaller. This value does not apply when it is carried in an A-MPDU.
    * Note: This value will not have any effect on some rate control algorithms.
    */
    void
    ConfigureRtsCtsAndFragmenatation (bool enableRts , uint32_t rtsCtsThreshold ,
                                      bool enableFragmentation , uint32_t fragmentationThreshold)
    {
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold",
                        enableRts ? UintegerValue (rtsCtsThreshold) : UintegerValue (999999));
    Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold",
                        enableFragmentation ? UintegerValue (fragmentationThreshold) : UintegerValue (999999));
    }

    /**
    * Change queue size for all the devices in the simulation.
    * \param queueSize The size of the queue in packets or bytes.
    */
    void
    ChangeQueueSize (string queueSize)
    {
    Config::SetDefault ("ns3::WifiMacQueue::MaxSize", QueueSizeValue (QueueSize (queueSize)));
    }

    /**
    * Validate A-MSDU and A-MPDU frame aggregation attributes.
    * \param msduAggSize The maximum A-MSDU frame aggregation size.
    * \param mpduAggSize The maximum A-MPDU frame aggregation size.
    * \param standard The WiGig standard being utilized (IEEE 802.11ad or IEEE 802.11ay).
    */
    void
    ValidateFrameAggregationAttributes (string &msduAggSize, string &mpduAggSize,
                                        WifiPhyStandard standard)
    {
    // Valdiate A-MSDU value
    if (msduAggSize == "max")
        {
        msduAggSize = MAX_DMG_AMSDU_LENGTH;
        }
    else if (std::stoul (msduAggSize) > std::stoul (MAX_DMG_AMSDU_LENGTH))
        {
        NS_FATAL_ERROR ("The maximum size for A-MSDU is " << MAX_DMG_AMSDU_LENGTH);
        }
    // Valdiate A-MPDU value
    if (mpduAggSize == "max")
        {
        mpduAggSize = (standard == WIFI_PHY_STANDARD_80211ad) ? MAX_DMG_AMPDU_LENGTH : MAX_EDMG_AMPDU_LENGTH;
        }
    else if ((standard == WIFI_PHY_STANDARD_80211ad) && (std::stoul (mpduAggSize) > std::stoul (MAX_DMG_AMPDU_LENGTH)))
        {
        NS_FATAL_ERROR ("The maximum size for A-MPDU in IEEE 802.11ad is " << MAX_DMG_AMPDU_LENGTH);
        }
    else if ((standard == WIFI_PHY_STANDARD_80211ay) && (std::stoul (mpduAggSize) > std::stoul (MAX_EDMG_AMPDU_LENGTH)))
        {
        NS_FATAL_ERROR ("The maximum size for A-MPDU in IEEE 802.11ay is " << MAX_EDMG_AMPDU_LENGTH);
        }
    }

    /**
    * Populate the ARP Cache for all the nodes in the network.
    */
    void
    PopulateArpCache (void)
    {
    Ptr<ArpCache> arp = CreateObject<ArpCache> ();
    arp->SetAliveTimeout (Seconds (3600 * 24 * 365));

    for (NodeList::Iterator i = NodeList::Begin (); i != NodeList::End (); ++i)
        {
        Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
        NS_ASSERT (ip != 0);
        ObjectVectorValue interfaces;
        ip->GetAttribute ("InterfaceList", interfaces);
        for (ObjectVectorValue::Iterator j = interfaces.Begin (); j != interfaces.End (); j++)
            {
            Ptr<Ipv4Interface> ipIface = (j->second)->GetObject<Ipv4Interface> ();
            NS_ASSERT (ipIface != 0);
            Ptr<NetDevice> device = ipIface->GetDevice ();
            NS_ASSERT (device != 0);
            Mac48Address addr = Mac48Address::ConvertFrom (device->GetAddress ());
            for (uint32_t k = 0; k < ipIface->GetNAddresses (); k++)
                {
                Ipv4Address ipAddr = ipIface->GetAddress (k).GetLocal ();
                if (ipAddr == Ipv4Address::GetLoopback ())
                    {
                    continue;
                    }
                ArpCache::Entry *entry = arp->Add (ipAddr);
                entry->MarkWaitReply (0);
                entry->MarkAlive (addr);
                }
            }
        }

    for (NodeList::Iterator i = NodeList::Begin (); i != NodeList::End (); ++i)
        {
        Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
        NS_ASSERT (ip != 0);
        ObjectVectorValue interfaces;
        ip->GetAttribute ("InterfaceList", interfaces);
        for (ObjectVectorValue::Iterator j = interfaces.Begin (); j != interfaces.End (); j++)
            {
            Ptr<Ipv4Interface> ipIface = (j->second)->GetObject<Ipv4Interface> ();
            ipIface->SetAttribute ("ArpCache", PointerValue (arp));
            }
        }
    }


    void
    ChangeNodeAntennaOrientation (Ptr<NetDevice> netDevice, double psi, double theta, double phi)
    {
    Ptr<WifiNetDevice> wifiNetDevice = StaticCast<WifiNetDevice> (netDevice);
    Ptr<DmgWifiMac> wifiMac = StaticCast<DmgWifiMac> (wifiNetDevice->GetMac ());
    Ptr<Codebook> codebook = wifiMac->GetCodebook ();
    codebook->ChangeAntennaOrientation (1, psi, theta, phi);
    }

    void
    ChangeNodesAntennaOrientation (NetDeviceContainer &container, double psi, double theta, double phi)
    {
    for (NetDeviceContainer::Iterator iter = container.Begin (); iter != container.End (); iter++)
        {
        ChangeNodeAntennaOrientation (*iter, psi, theta, phi);
        }
    }
}

