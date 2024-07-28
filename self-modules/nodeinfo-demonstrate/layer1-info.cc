//layer1-info.cc
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"


using namespace ns3;



namespace ns3 {

/* ... */

    void ns3_PrintNodePositions_FunctionPhysicalInfo1(
        NodeContainer nodes , std::string containerName
    ) 
    {
        for (NodeContainer::Iterator it = nodes.Begin (); it != nodes.End (); ++it)
        {
            Ptr<Node> node = *it;
            Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
            Vector pos = mobility->GetPosition ();
            std::cout << containerName << " Node " << node->GetId () << ": (" 
                    << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
        }
    }

}