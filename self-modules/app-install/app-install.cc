/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "app-install.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <ios>
#include <cstdlib>
#include <vector>


#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/timestamp-tag.h"


using namespace ns3;
using namespace std;

namespace ns3 {

/* ... */

    void ns3_ForUsers_InstallTCPRxSink_TCPApp1(
        ApplicationContainer *UsersSink_ApplicationContainer , 
        Ptr<PacketSink>      *Users_PacketSink   , 
        Ptr<Node>            Users_Node         ,  
        uint32_t             Users_Port         ,
        double               SimulateStartTime  , 
        double               SimulateEndTime
    )
    {
        PacketSinkHelper Users_PacketSinkHelper(
            "ns3::TcpSocketFactory" , 
            InetSocketAddress(
                Ipv4Address::GetAny(), 
                Users_Port     
            )
        );
        
        *UsersSink_ApplicationContainer = Users_PacketSinkHelper.Install(Users_Node);
        *Users_PacketSink = StaticCast<PacketSink>((*UsersSink_ApplicationContainer).Get(0));
        // *Users_PacketSink = StaticCast<PacketSink>(*UsersSink_ApplicationContainer->Get(0));
        UsersSink_ApplicationContainer->Start(Seconds(SimulateStartTime));
        UsersSink_ApplicationContainer->Stop (Seconds(SimulateEndTime)  );
        
        *Users_PacketSink = StaticCast<PacketSink>(
            UsersSink_ApplicationContainer->Get(0)
        );
    }



    void ns3_ForServer_InstallTCPTxSink_TCPApp2(
        ApplicationContainer *ServerSink_ApplicationContainer , 
        Ptr<Node>   Server_Node           ,  
        uint32_t    Server_Port           ,
        uint32_t    Server_PayloadSize    , 
        string      Server_DataRate       , 
        double      SimulateStartTime  , 
        double      SimulateEndTime    ,
        Ipv4Address UsersAddress
    )
    {
        OnOffHelper ServerApp_OnOffHelper(
            "ns3::TcpSocketFactory" , 
            InetSocketAddress(
                UsersAddress , Server_Port
            )
        );
        ServerApp_OnOffHelper.SetAttribute("MaxPackets"     , UintegerValue(0));
        ServerApp_OnOffHelper.SetAttribute("PacketSize"     , UintegerValue(Server_PayloadSize));
        ServerApp_OnOffHelper.SetAttribute("OnTime"         , StringValue("ns3::ConstantRandomVariable[Constant=1e6]"));
        ServerApp_OnOffHelper.SetAttribute("OffTime"        , StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        ServerApp_OnOffHelper.SetAttribute("DataRate"       , DataRateValue(DataRate(Server_DataRate)));
        ServerApp_OnOffHelper.SetAttribute("EnableTimestamp", BooleanValue(true));

        (*ServerSink_ApplicationContainer) = ServerApp_OnOffHelper.Install(Server_Node);
        (*ServerSink_ApplicationContainer).Start(Seconds(SimulateStartTime));
        (*ServerSink_ApplicationContainer).Stop (Seconds(SimulateEndTime)  );
    }
}

