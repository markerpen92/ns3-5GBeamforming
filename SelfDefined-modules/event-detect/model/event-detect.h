/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef EVENT_DETECT_H
#define EVENT_DETECT_H


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


namespace ns3 {

/* ... */

    void ns3_ForAntennas_AssociationHappened_FunctionAPtoUsers1(
        Ptr<DmgStaWifiMac> AntennaxConnectAPx_DmgStaWifiMac ,
        Ptr<DmgApWifiMac > AP1Mac_DmgApWifiMac_ForTraceLog  ,
        Dict<string,int  >        DictForAntennax           , 
        Mac48Address              APx_Mac48Address          ,
        uint16_t           AntennaxConnectAPx_AssociationID
    );



    void ns3_ForAntennas_SLSCompleted_FunctionAPtoUsers2(
        Ptr<DmgStaWifiMac> AntennaxConnectAPx_DmgWifiMac ,
        SlsCompletionAttrbitutes Attribute_SlsCompletionAttrbitutes
    );



    void ns3_ForAPandAntenna_SectorSweep_FunctionAPtoUsers3(
        Ptr<DmgStaWifiMac> AntennaxConnectAPx_DmgStaWifiMac_ForTraceLog , 
        Ptr<DmgApWifiMac>    APxMac_DmgApWifiMac_ForTraceLog
    );

}

#endif /* EVENT_DETECT_H */