/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef NETWORK_MEASURE_H
#define NETWORK_MEASURE_H


#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/timestamp-tag.h"
#include "ns3/flow-monitor-module.h"


#include "ns3/datatype.h"


using namespace ns3;
using namespace std;


namespace ns3 {

/* ... */

    void ns3_ForEveryone_TCPThroughputMeasure_FunctionTCPMeasure1(
        Ipv4Address      NodeAddress,
        Ptr<PacketSink>  *PacketSink,
        uint64_t*        TotalRx,
        double*          Throughput
    );



    double ns3_ForNetwork_TCPThroughputMeasure_FunctionTCPMeasure2(
        Ptr<PacketSink>  *PacketSink
    );



    void ns3_ForEveryone_SNRMeasure_FunctionTCPMeasure3(
        Ptr<DmgStaWifiMac> AntennaxConnectAPx_DmgApWifiMac_ForTraceLog , 
        Ptr<DmgApWifiMac>    APxMac_DmgApWifiMac_ForTraceLog
    );



    void ns3_ForEveryone_DelayMeasure_FunctionTCPMeasure4(
        int none
    );



    void ns3_ForEveryone_FlowMonitorStatistics_FunctionTCPMeasure5(
        FlowMonitorHelper &FlowMonitorProgram , 
        Ptr<FlowMonitor>  MonitorDevice      , 
        double            simulationTime
    );
}

#endif /* NETWORK_MEASURE_H */

