/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef APP_INSTALL_H
#define APP_INSTALL_H


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
        Ptr<PacketSink>*      Users_PacketSink  , 
        Ptr<Node>            Users_Node         ,  
        uint32_t             Users_Port         ,
        double               SimulateStartTime , 
        double               SimulateEndTime
    );



    void ns3_ForServer_InstallTCPTxSink_TCPApp2(
        ApplicationContainer *ServerSink_ApplicationContainer , 
        Ptr<Node>   Server_Node           ,  
        uint32_t    Server_Port           ,
        uint32_t    Server_PayloadSize    , 
        string      Server_DataRate       , 
        double      SimulateStartTime  , 
        double      SimulateEndTime    ,
        Ipv4Address UsersAddress
    );
}

#endif /* APP_INSTALL_H */

