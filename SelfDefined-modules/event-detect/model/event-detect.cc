/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <ios>
#include <cstdlib>
#include <vector>


#include "event-detect.h"


#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/timestamp-tag.h"


#include "ns3/datatype.h"


using namespace ns3;
using namespace std;

namespace ns3 {

    /* ... */

    void ns3_ForAntennas_AssociationHappened_FunctionAPtoUsers1(
        Ptr<DmgStaWifiMac> AntennaxConnectAPx_DmgStaWifiMac , 
        Ptr<DmgApWifiMac>  AP1Mac_DmgApWifiMac_ForTraceLog  ,
        Dict<string,int>   DictForAntennax                  , 
        Mac48Address       APx_Mac48Address                 ,
        uint16_t           AntennaxConnectAPx_AssociationID
    )
    {
        int      user_idx    = DictForAntennax          ["user-idx"]   ;
        int      antenna_idx = DictForAntennax          ["antenna-id"] ;
        int      ap_idx      = DictForAntennax          ["ap-id"]      ;
        uint32_t alloc_type  = (uint32_t)DictForAntennax["AllocType"]  ;

        cout << "User<" << user_idx+1 << ">---Antenna(" << antenna_idx <<  ")---IP[" 
        << AntennaxConnectAPx_DmgStaWifiMac->GetAddress() << "]\tassociate with AP<"
        << ap_idx << ">---(" << APx_Mac48Address << ")\tAID<" << 
        AntennaxConnectAPx_AssociationID << ">\n" << endl;

        if(alloc_type == SERVICE_PERIOD_ALLOCATION)
        {
            std::cout << "Allocate DTI as Service Period" << std::endl;
            AP1Mac_DmgApWifiMac_ForTraceLog->AllocateDTIAsServicePeriod(
                1 , AntennaxConnectAPx_DmgStaWifiMac->GetAssociationID() , AID_AP
            );
        }
    }



    void ns3_ForAntennas_SLSCompleted_FunctionAPtoUsers2(
        Ptr<DmgStaWifiMac>          AntennaxConnectAPx_DmgWifiMac , 
        SlsCompletionAttrbitutes Attribute_SlsCompletionAttrbitutes
    )
    {
        cout << "Not Implement..." << endl;
    }



    void ns3_ForAPandAntenna_SectorSweep_FunctionAPtoUsers3(
        Ptr<DmgStaWifiMac> AntennaxConnectAPx_DmgStaWifiMac_ForTraceLog , 
        Ptr<DmgApWifiMac>  APxMac_DmgApWifiMac_ForTraceLog
    )
    {
        Simulator::ScheduleNow(
            &DmgWifiMac::Perform_TXSS_TXOP   , 
            AntennaxConnectAPx_DmgStaWifiMac_ForTraceLog , 
            APxMac_DmgApWifiMac_ForTraceLog ->GetAddress()
        );
    }

}