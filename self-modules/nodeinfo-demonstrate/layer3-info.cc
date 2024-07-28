/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
// layer3-info.cc
#include <iostream>
#include <ios>


#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"


using namespace std;
using namespace ns3;


// Define the logging component
NS_LOG_COMPONENT_DEFINE("RouteTable");



namespace ns3 {

/* ... */

    void ns3_PrintRoutingTable_FunctionRouteTable1(Ptr<Node> node)
    {
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        if (ipv4 == nullptr)
        {
            NS_LOG_ERROR("Node does not have an Ipv4 object");
            return;
        }

        // Get the static routing object
        Ipv4StaticRoutingHelper ipv4RoutingHelper;
        Ptr<Ipv4StaticRouting> staticRouting = ipv4RoutingHelper.GetStaticRouting(ipv4);
        if (staticRouting == nullptr)
        {
            NS_LOG_ERROR("Node does not have a static routing object");
            return;
        }

        // Print the routing table
        uint32_t numRoutes = staticRouting->GetNRoutes();
        NS_LOG_INFO("Routing Table for Node " << node->GetId());
        for (uint32_t i = 0; i < numRoutes; ++i)
        {
            Ipv4RoutingTableEntry entry = staticRouting->GetRoute(i);
            cout << "Destination: " << entry.GetDestNetwork()
                << ", Gateway: " << entry.GetGateway()
                << ", Interface Index: " << entry.GetInterface()
                << endl;
        }
    }
}


