/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
// layer2-info.h

#ifndef LAYER2_INFO_H
#define LAYER2_INFO_H


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/ssid.h"
#include <iostream>



namespace ns3 {

    /**
     * @brief Function to print the MAC addresses of a node's devices
     *
     * This function iterates through a node's devices and prints the MAC addresses
     * of the devices that are WifiNetDevices.
     *
     * @param node The node whose devices' MAC addresses are to be printed
     */
    void ns3_PrintNodeMacAddresses_FunctionLayer2Info1(Ptr<Node> node);

} // namespace ns3

#endif // LAYER2_INFO_H
