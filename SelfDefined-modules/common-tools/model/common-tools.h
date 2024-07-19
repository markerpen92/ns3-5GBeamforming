/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef COMMON_TOOLS_H
#define COMMON_TOOLS_H

/*
 * Copyright (c) 2015-2020 IMDEA Networks Institute
 * Author: Hany Assasa <hany.assasa@gmail.com>
 */
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"


using namespace std;
using namespace ns3;

namespace ns3 {

/****** Common Variables and Definitions ******/

struct CommunicationPair {
  Ptr<Application> srcApp;
  Ptr<PacketSink> packetSink;
  uint64_t totalRx = 0;
  double throughput = 0;
  Time startTime;
};

#define PI 3.14159

typedef std::map<uint32_t, CommunicationPair> CommunicationPairList;
typedef CommunicationPairList::iterator CommunicationPairList_I;
typedef CommunicationPairList::const_iterator CommunicationPairList_CI;
typedef std::map<string, string> TCP_VARIANTS;                  //!< Map TCP variant name to ns3 TCP class name.
typedef TCP_VARIANTS::iterator TCP_VARIANTS_I;                            //!< Typedef for iterator over the TCP variants.
static TCP_VARIANTS TCP_VARIANTS_LIST = {{"NewReno",       "ns3::TcpNewReno"} ,
                                         {"Hybla",         "ns3::TcpHybla"},
                                         {"HighSpeed",     "ns3::TcpHighSpeed"},
                                         {"Vegas",         "ns3::TcpVegas"},
                                         {"Scalable",      "ns3::TcpScalable"},
                                         {"Veno",          "ns3::TcpVeno"},
                                         {"Bic",           "ns3::TcpBic"},
                                         {"Westwood",      "ns3::TcpWestwood"},
                                         {"WestwoodPlus",  "ns3::TcpWestwoodPlus"},
                                         {"YeAH",          "ns3::TcpYeah"},
                                         {"Illinois",      "ns3::TcpIllinois"},
                                         {"DCTCP",         "ns3::TcpDctcp"},
                                         {"TCP-LP",        "ns3::TcpLp"},
                                         {"LEDBAT",        "ns3::TcpLedbat"}
                                        };

static string TCP_VARIANTS_NAMES = "Transport protocol to use: NewReno, Hybla, HighSpeed, Vegas, "
                                        "Scalable, Veno, Bic, Westwood, WestwoodPlus, YeAH, "
                                        "Illinois, DCTCP, TCP-LP, and LEDBAT";

/**
 * Additional list SLS parameters structure.
 */
struct SLS_PARAMETERS : public SimpleRefCount<SLS_PARAMETERS> {
  uint32_t srcNodeID;
  uint32_t dstNodeID;
  Ptr<DmgWifiMac> wifiMac;
};

/****** Common Functions ******/

//CommunicationPair
//InstallApplications (string applicationType, string socketType,
//                     double simulationTime,
//                     Ptr<Node> srcNode, Ptr<Node> dstNode, Ipv4Address address)
//{
//  CommunicationPair commPair;

//  /* Install TCP/UDP Transmitter on the source node */
//  Address dest (InetSocketAddress (address, 9999));
//  ApplicationContainer srcApp;
//  if (applicationType == "onoff")
//    {
//      OnOffHelper src (socketType, dest);
//      src.SetAttribute ("MaxPackets", UintegerValue (maxPackets));
//      src.SetAttribute ("PacketSize", UintegerValue (packetSize));
//      src.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1e6]"));
//      src.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
//      src.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
//      srcApp = src.Install (srcNode);
//    }
//  else if (applicationType == "bulk")
//    {
//      BulkSendHelper src (socketType, dest);
//      srcApp = src.Install (srcNode);
//    }
//  srcApp.Start (Seconds (simulationTime + 1));
//  srcApp.Stop (Seconds (simulationTime));
//  commPair.srcApp = srcApp.Get (0);

//  /* Install Simple TCP/UDP Server on the destination node */
//  PacketSinkHelper sinkHelper (socketType, InetSocketAddress (Ipv4Address::GetAny (), 9999));
//  ApplicationContainer sinkApp = sinkHelper.Install (dstNode);
//  commPair.packetSink = StaticCast<PacketSink> (sinkApp.Get (0));
//  sinkApp.Start (Seconds (0.0));

//  return commPair;
//}
/**
 * Create and return an output stream wrapper for SLS trace stream.
 * \param fileName The name of the file to store the trace results.
 * \return A pointer to the output stream wrapper.
 */
Ptr<OutputStreamWrapper> CreateSlsTraceStream (string fileName = "slsResults");

/**
 * Convert a value to string with specified precision.
 * \param a_value The value to convert.
 * \param n The number of decimal places.
 * \return The string representation of the value with specified precision.
 */
template <typename T>
string to_string_with_precision (const T a_value, const int n = 6);

/**
 * Calculate the throughput of a single stream.
 * \param sink Pointer to the packet sink.
 * \param lastTotalRx Reference to the last total received bytes.
 * \param averageThroughput Reference to the average throughput.
 * \return The calculated throughput.
 */
double CalculateSingleStreamThroughput (Ptr<PacketSink> sink, uint64_t &lastTotalRx, double &averageThroughput);

/**
 * Calculate the delay of a single stream.
 * \param sink Pointer to the packet sink.
 * \param totalDelay Reference to the total delay.
 * \param pktNum Reference to the number of packets.
 * \return The calculated delay.
 */
long CalculateSingleStreamDelay (Ptr<PacketSink> sink, Time &totalDelay, uint64_t &pktNum);

/**
 * Print flow monitor statistics.
 * \param flowmon The flow monitor helper class.
 * \param monitor Pointer to the flow monitor engine.
 * \param simulationTime The simulation time in seconds.
 */
void PrintFlowMonitorStatistics (FlowMonitorHelper &flowmon, Ptr<FlowMonitor> monitor, double simulationTime);

/**
 * Print application layer and flow monitor statistics.
 * \param flowmon The flow monitor helper class.
 * \param monitor Pointer to the flow monitor engine.
 * \param communicationPairList List of communication pairs in the network.
 * \param applicationType The type of the application (onoff/bulk).
 * \param simulationTime The simulation time in seconds.
 */
void PrintApplicationLayerAndFlowMonitorStatistics (FlowMonitorHelper &flowmon, Ptr<FlowMonitor> monitor,
                                                    CommunicationPairList &communicationPairList, string applicationType,
                                                    double simulationTime);

/**
 * Configure TCP options.
 * \param tcpVariant The name of the used TCP variant.
 * \param segmentSize The TCP segment size in bytes.
 * \param bufferSize The size of the TCP send and receive buffers in bytes.
 */
void ConfigureTcpOptions (string tcpVariant, uint32_t segmentSize, uint32_t bufferSize);

/**
 * Configure RTS/CTS and fragmentation options.
 * \param enableRts Flag to indicate if RTS/CTS handshake should be enabled.
 * \param rtsCtsThreshold RTS/CTS threshold.
 * \param enableFragmentation Flag to indicate if fragmentation should be enabled.
 * \param fragmentationThreshold Fragmentation threshold.
 */
void ConfigureRtsCtsAndFragmenatation (bool enableRts = false, uint32_t rtsCtsThreshold = 0,
                                        bool enableFragmentation = false, uint32_t fragmentationThreshold = 0);

/**
 * Change queue size for all the devices in the simulation.
 * \param queueSize The size of the queue in packets or bytes.
 */
void ChangeQueueSize (string queueSize);

/**
 * Validate A-MSDU and A-MPDU frame aggregation attributes.
 * \param msduAggSize The maximum A-MSDU frame aggregation size.
 * \param mpduAggSize The maximum A-MPDU frame aggregation size.
 * \param standard The WiGig standard being utilized (IEEE 802.11ad or IEEE 802.11ay).
 */
void ValidateFrameAggregationAttributes (string &msduAggSize, string &mpduAggSize,
                                          WifiPhyStandard standard = WIFI_PHY_STANDARD_80211ad);

/**
 * Populate the ARP cache for all nodes in the network.
 */
void PopulateArpCache (void);

/**
 * Change the antenna orientation of a node.
 * \param netDevice Pointer to the network device.
 * \param psi The azimuth angle.
 * \param theta The elevation angle.
 * \param phi The roll angle.
 */
void ChangeNodeAntennaOrientation (Ptr<NetDevice> netDevice, double psi, double theta, double phi);

/**
 * Change the antenna orientation of multiple nodes.
 * \param container The container of network devices.
 * \param psi The azimuth angle.
 * \param theta The elevation angle.
 * \param phi The roll angle.
 */
void ChangeNodesAntennaOrientation (NetDeviceContainer &container, double psi, double theta, double phi);


} //namespace ns3

#endif /* COMMON_TOOLS_H */
