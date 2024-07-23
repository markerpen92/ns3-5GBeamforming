/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "network-measure.h"


#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <ios>
#include <cstdlib>
#include <vector>


#include "ns3/ptr.h"
#include "ns3/packet-sink.h"
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
        Ipv4Address      Antenna1NodeAddress,
        Ipv4Address      Antenna2NodeAddress,
        Ptr<PacketSink>  *PacketSink, 
        uint64_t*        TotalRx    , 
        double*          Throughput
    )
    {
        Time now = Simulator::Now();
        double CurrentRx   = (*PacketSink)->GetTotalRx()*8.0/1e6;
        double Interval    = now.GetSeconds()-(*TotalRx)/((*Throughput)+1e-5);
        double IntervalThr = (CurrentRx-*TotalRx)/Interval;
        *TotalRx           = CurrentRx;
        *Throughput        = CurrentRx/now.GetSeconds();

        cout << "    " <<  left << "\n==========" << "IP<" << Antenna1NodeAddress << " & " << Antenna2NodeAddress << ">" << "at time " << now.GetSeconds() << " ==========\n";
        cout << "    " <<  setw(10) << "Total-Throughput : " << *Throughput  << endl         ;
        cout << "    " <<  setw(10) << "Itval-Throughput : " << IntervalThr  << endl         ;
        cout << "    " <<  setw(10) << "CurrentRx        : " << CurrentRx    << endl         ;
        cout << "    " <<  setw(10) <<  "Itval            : " << Interval    << '\n' << endl ;

        Simulator::Schedule(
            Seconds(0.1) , 
            &ns3_ForEveryone_TCPThroughputMeasure_FunctionTCPMeasure1 , 
            Antenna1NodeAddress , Antenna2NodeAddress , PacketSink , TotalRx , Throughput 
        );
    }



    double ns3_ForNetwork_TCPThroughputMeasure_FunctionTCPMeasure2(
        vector<Ptr<PacketSink>>& NetworkAllRxPackeSink
    )
    {
        Time now = Simulator::Now();
        int SinkAmount  = NetworkAllRxPackeSink.size();
        double TotalRx  = 0.0;
        
        for (int sink_idx = 0; sink_idx < SinkAmount; sink_idx++)
        {
            TotalRx += NetworkAllRxPackeSink[sink_idx]->GetTotalRx() * 8.0 / 1e6;
        }
        
        return TotalRx / now.GetSeconds();
    }



    void ns3_ForEveryone_SNRMeasure_FunctionTCPMeasure3(
        Ptr<DmgStaWifiMac> AntennaxConnectAPx_DmgStaWifiMac_ForTraceLog , 
        Ptr<DmgApWifiMac>    APxMac_DmgApWifiMac_ForTraceLog
    )
    {
        double AntennaxConnectAPx_SNR = 
        AntennaxConnectAPx_DmgStaWifiMac_ForTraceLog->GetSnrTable();

        if(AntennaxConnectAPx_SNR > -50)
        {
            Ptr<Ipv4> Antennax_Ipv4 = AntennaxConnectAPx_DmgStaWifiMac_ForTraceLog
                                      ->GetDevice()->GetNode()->GetObject<Ipv4>();

            Ptr<Ipv4> APx_Ipv4      = APxMac_DmgApWifiMac_ForTraceLog
                                      ->GetDevice()->GetNode()->GetObject<Ipv4>();

            int32_t Antennax_Interface = Antennax_Ipv4->GetInterfaceForDevice(
                AntennaxConnectAPx_DmgStaWifiMac_ForTraceLog->GetDevice()
            );

            int32_t APx_Interface      = APx_Ipv4->GetInterfaceForDevice(
                APxMac_DmgApWifiMac_ForTraceLog->GetDevice()
            );

            cout << Antennax_Ipv4->GetAddress(Antennax_Interface , 0).GetLocal()
            << " -> SNR " << AntennaxConnectAPx_SNR << " is normal";

            if(AntennaxConnectAPx_SNR < 8)
            {
                cout << " || Secotr Sweep";
            }

            cout << "-> AP-IP[" << APx_Ipv4->GetAddress(APx_Interface , 0).GetLocal()
            << "]\n";
        }

        Simulator::Schedule(
            MilliSeconds(100) , 
            &ns3_ForEveryone_SNRMeasure_FunctionTCPMeasure3 , 
            AntennaxConnectAPx_DmgStaWifiMac_ForTraceLog    ,
            APxMac_DmgApWifiMac_ForTraceLog
        );
    }



    void ns3_ForEveryone_DelayMeasure_FunctionTCPMeasure4(
        int none
    )
    {
        cout << "Not Implement FunctionTCPMeasure4\n" << endl;
    }



    void ns3_ForEveryone_FlowMonitorStatistics_FunctionTCPMeasure5(
        FlowMonitorHelper &FlowMonitorProgram , 
        Ptr<FlowMonitor>  MonitorDevice       , 
        double            simulationTime
    )
    {
        MonitorDevice->CheckForLostPackets();
        Ptr<Ipv4FlowClassifier> Classifier = 
        DynamicCast<Ipv4FlowClassifier>(FlowMonitorProgram.GetClassifier());
        FlowMonitor::FlowStatsContainer stats = MonitorDevice->GetFlowStats();
        for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
        {
            Ipv4FlowClassifier::FiveTuple t = Classifier->FindFlow (i->first);
            cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")" << std::endl;
            cout << "  Tx Packets: " << i->second.txPackets << std::endl;
            cout << "  Tx Bytes:   " << i->second.txBytes << std::endl;
            cout << "  TxOffered:  " << i->second.txBytes * 8.0 / (simulationTime * 1e6)  << " Mbps" << std::endl;
            cout << "  Rx Packets: " << i->second.rxPackets << std::endl;
            cout << "  Rx Bytes:   " << i->second.rxBytes << std::endl;
            cout << "  Throughput: " << i->second.rxBytes * 8.0 / (simulationTime * 1e6)  << " Mbps" << std::endl;
        }
    }
}

