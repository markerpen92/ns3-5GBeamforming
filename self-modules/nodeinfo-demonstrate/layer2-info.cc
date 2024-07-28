/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
// layer2-info.cc

#include "layer2-info.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/ssid.h"
#include <iostream>


using namespace ns3;



namespace ns3 {

    void  ns3_PrintNodeMacAddresses_FunctionLayer2Info1(Ptr<Node> node)
    {
        for (uint32_t i = 0; i < node->GetNDevices(); ++i)
        {
            Ptr<NetDevice> device = node->GetDevice(i);
            Ptr<WifiNetDevice> wifiDevice = DynamicCast<WifiNetDevice>(device);
            if (wifiDevice)
            {
                Mac48Address macAddress = Mac48Address::ConvertFrom(wifiDevice->GetAddress());
                std::cout << "MAC Address of device " << i << ": " << macAddress << std::endl;
            }
        }
    }

}
