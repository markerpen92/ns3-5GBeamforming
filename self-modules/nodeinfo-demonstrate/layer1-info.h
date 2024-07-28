/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
// layer1-info.h

#ifndef LAYER1_INFO_H
#define LAYER1_INFO_H


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include <iostream>



namespace ns3 {

    /**
     * @brief Function to print the positions of nodes
     *
     * This function iterates through a NodeContainer and prints the positions
     * of the nodes using their MobilityModel.
     *
     * @param nodes The NodeContainer holding the nodes whose positions are to be printed
     * @param containerName A string representing the name of the container
     */
    void ns3_PrintNodePositions_FunctionPhysicalInfo1(NodeContainer nodes, std::string containerName);

} // namespace ns3

#endif // LAYER1_INFO_H
