/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
// layer3-info.h

#ifndef LAYER3_INFO_H
#define LAYER3_INFO_H


#include <iostream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"



namespace ns3 {

    /**
     * @brief Function to print the routing table of a node
     *
     * This function retrieves the Ipv4 object from the given node, and if it
     * exists, it retrieves the static routing object and prints the routing
     * table entries.
     *
     * @param node The node whose routing table is to be printed
     */
    void ns3_PrintRoutingTable_FunctionRouteTable1(Ptr<Node> node);

    void ns3_PrintIpv4Address_FunctionLayer3Info2 (Ptr<Node> node);

} // namespace ns3

#endif // LAYER3_INFO_H
