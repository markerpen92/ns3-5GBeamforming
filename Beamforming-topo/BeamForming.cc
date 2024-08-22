/*
    Main Path :  ../ (ns3-allinone/ns-3.XX/)
    Now  Path :  ./  (ns3-allinone/ns-3.XX/scratch)
*/
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
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/log.h"


#include "ns3/app-install.h"
#include "ns3/network-measure.h"
#include "ns3/event-detect.h"
#include "ns3/datatype.h"
#include "ns3/layer1-info.h"
#include "ns3/layer2-info.h"
#include "ns3/layer3-info.h"


#include "ns3/srnc-ethernet-helper.h"
#include "ns3/srnc-ethernet-channel.h"
#include "ns3/srnc-ethernet-device.h"


#include "ns3/parameters.h"
#include "ns3/common-tools.h"


NS_LOG_COMPONENT_DEFINE ("CompareAccessSchemes");
using namespace ns3;
using namespace std;

/*
variable rule

if variable contains only one node
    variable = [variable-chracter]_[variable-type]_[special description]
elif variable contains nodes
    variable = [Variable-chracter1][Action][Variable-character2]..._[variable-type]_[special description]


if variable in not from ns-3 but used for one ns-3 variable
    variable = [For which character in network]_[Role]
elif variable in not from ns-3 but used for ns-3 variables
    variable = [Character1][Action][Character2]_[role]


if variable not from ns-3 and not for ns-3
    variable = [Role]



function-name rule

if function is from ns-3
    function-name = [ns3]_[For Which Character In Network]_[Role]_[this fuinction's FunctionID]
                                                                        FunctionXX

    if this function is for every character
        function-name = [ns3]_[Role]_[this function's FunctionID]

if function is not from ns-3 but used for ns-3
    function-name = [ns3Sub]_For[Which FunctionID]_[Role]_[this fuinction's FunctionID]


if function-name is not from ns-3 and not for ns-3
    function-name = [Role]_[this fuinction's FunctionID]



indent rule

each block 5-lines interval

each purpose-inBlock 3-lines interval

each action-inPurpose 1-lines interval
*/





int main(int argc , char* argv[])
{
    /*----------------Set OuputFile Path---------------------------------------------------------------------------------------------------------------------------------------------------*/
    string ConditionFile = condition==1    ? "S1"    : "S2" ;
    string Method        = EncodeMaxNum==1 ? "PD"    : "NC" ;
    string Method1       = BM_enable == 1  ? "_15ms" : ""   ; 

    string OutputFileName = "./2_rawdata/seed"+ std::to_string(int(seed)) + "/"+ ConditionFile +"/" + Method + Method1 + "/" + std::to_string((int)UserNum) + "/";
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



    /*----------------Simulation Parameters------------------------------------------------------------------------------------------------------------------------------------------------*/
    int user_num = UserNum;
    uint32_t port = 6666;
    // double   SimulationTime = condition==1 ? 10.0 : 4.0 ;
    double   SimulationTime = 5.0      ;
    uint32_t PayloadSize    = 1400     ;
    string   DataRate       = "300Mbps";
    uint32_t AllocationType = CBAP_ALLOCATION;

    string   MsduAggregationSize = "max";
    string   MpduAggregationSize = "max";
    bool     EnableRTS       = false;
    uint32_t RTSThreshold    = 0 ;
    string   PhyMode         = "EDMG_OFDM_MCS8";
    string   WifiStandard    = "802.11ay" ;
    // float    TimeOffset      = 0.5;
    string   InputBufferSize = "4000p";
    int T_BM = BM_enable ? ( UserNum==1 ? 3 : (UserNum<6 ? 2+(user_num-1)*3 : 15) ) : 500 ; /*$T(BM)*/

    bool  Verbose       = false;
    bool  PcapTrace     = true ;
    bool  EnableMACReTX = true;
    uint32_t SnapShotLength = std::numeric_limits<uint32_t>::max (); /* The maximum PCAP Snapshot Length. */
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



    /*----------------Init Parameters for measurement--------------------------------------------------------------------------------------------------------------------------------------*/
    uint64_t TotalRx          [user_num] = {0};
    double   Throughput       [user_num] = {0};
    Ptr<PacketSink> PacketSink[user_num];

    ApplicationContainer SinkAPP[user_num];
    ApplicationContainer SrcAPP[user_num] ;
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



    /*---------------Clean The Files-------------------------------------------------------------------------------------------------------------------------------------------------------*/
    ofstream Files;
    Files.open( "0_packet_latency.txt" ,std::ios::trunc );
    Files.close();
    Files.open( OutputFileName+"2_overall.csv" ,std::ios::trunc );
    Files.close();
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



    /*--------------TCPLayer + MACLayer Config------------------*/
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue   ("ns3::TcpCubic"));  //TCP
    Config::SetDefault ("ns3::TcpSocket::SegmentSize"   , UintegerValue (PayloadSize) );     //TCP
    Config::SetDefault ("ns3::TcpSocket::DelAckCount"   , UintegerValue (1));                //TCP
    Config::SetDefault ("ns3::TcpSocket::SndBufSize"    , UintegerValue (0.45*1024*1024) );  //TCP
    Config::SetDefault ("ns3::TcpSocket::RcvBufSize"    , UintegerValue (0.45*1024*1024) );  //TCP
    Config::SetDefault ("ns3::Ipv4GlobalRouting::RandomEcmpRouting", BooleanValue(true)  );  //TCP

    Config::SetDefault ("ns3::WifiMacQueue::MaxDelay"   , TimeValue(MilliSeconds (T_BM)) );  //MAC
    if(EnableMACReTX)
    {
        Config::SetDefault("ns3::WifiRemoteStationManager::MaxSlrc"        ,  UintegerValue (4)    );
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",  UintegerValue (65536));
    }
    else
    {
        Config::SetDefault("ns3::WifiRemoteStationManager::MaxSlrc"        ,  UintegerValue (0)   );
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold",  UintegerValue (1400));
    }
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



    /*-----------Command line argument parser setup----------------------------------------------------------------------------------------------------------------------------------------*/
    CommandLine cmd;
    cmd.AddValue ("PayloadSize", "Application payload size in bytes"               , PayloadSize    );
    cmd.AddValue ("dataRate"   , "Application data rate"                           , DataRate       );
    cmd.AddValue ("--amsdusize", "The maximum aggregation size for A-MSDU in Bytes", MsduAggregationSize);
    cmd.AddValue ("--ampdusize", "The maximum aggregation size for A-MPDU in Bytes", MpduAggregationSize);
    cmd.AddValue ("scheme"     , "The access scheme used for channel access (0: SP allocation, 1: CBAP allocation)", AllocationType);
    cmd.AddValue ("enableRts"  , "Enable or disable RTS/CTS handshake"             , EnableRTS      );
    cmd.AddValue ("rtsThreshold", "The RTS/CTS threshold value"                    , RTSThreshold   );
    cmd.AddValue ("queueSize"  , "The maximum size of the Wifi MAC Queue"          , InputBufferSize);
    cmd.AddValue ("PhyMode"    , "The WiGig PHY Mode"                              , PhyMode        );
    cmd.AddValue ("standard"   , "The WiGig standard being utilized (ad/ay)"       , WifiStandard   );
    cmd.AddValue ("verbose"    , "Turn on all WifiNetDevice log components"        , Verbose        );
    cmd.AddValue ("SimulationTime", "Simulation time in seconds"                   , SimulationTime );
    cmd.AddValue ("pcap"       , "Enable PCAP Tracing"                             , PcapTrace      );
    cmd.AddValue ("snapshotLength", "The maximum PCAP snapshot length"             , SnapShotLength );
    //cmd.AddValue ("the_seed"   , "Give the seed number to locate the dst file"     , seedwrite      );
    cmd.Parse (argc, argv);
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



    /*----------------Set OuputFile Path------------------------*/
    OutputFileName = "./2_rawdata/seed"+ to_string(int(seed)) + "/"+ ConditionFile +"/" + Method + Method1 + "/" + to_string(user_num) + "/";
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



    /*----------------WiFi Setting------------------------------------------------------------------------------------------------------------------------------------------------------*/
    WifiPhyStandard NetworkStandard = WifiStandard=="802.11ay" ? WIFI_PHY_STANDARD_80211ay : WIFI_PHY_STANDARD_80211ad;
    if (WifiStandard!="802.11ay" && WifiStandard!="802.11ad")
    {
        NS_FATAL_ERROR ("Wrong WiGig standard");
        exit(1);
    }

    cout << MsduAggregationSize << "||" << MpduAggregationSize << "||" << EnableRTS << "||" << RTSThreshold << "||" << NetworkStandard << endl;

    ValidateFrameAggregationAttributes (MsduAggregationSize, MpduAggregationSize, NetworkStandard);
    ConfigureRtsCtsAndFragmenatation   (EnableRTS , RTSThreshold);
    ChangeQueueSize                    (InputBufferSize);

    cout << MsduAggregationSize << "||" << MpduAggregationSize << "||" << EnableRTS << "||" << RTSThreshold << "||" << NetworkStandard << endl;

    DmgWifiHelper WiFi_DmgWifiHelper;
    WiFi_DmgWifiHelper.SetStandard(NetworkStandard);
    WiFi_DmgWifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager" , "DataMode" , StringValue(PhyMode));
    WiFi_DmgWifiHelper.SetCodebook(
        "ns3::CodebookAnalytical"    , 
        "CodebookType"               , 
        EnumValue(SIMPLE_CODEBOOK)   , 
        "Antennas", UintegerValue(1) , 
        "Sectors" , UintegerValue(8)
    );
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Create Nodes" << string(5, '-') << '\n';

    /*----------------Create Nodes------------------------------------------------------------------------------------------------------------------------------------------------------*/
    NodeContainer EdgeServer_NodeContainer;
    EdgeServer_NodeContainer.Create(1);
    Ptr<Node> EdgeServer_Node = EdgeServer_NodeContainer.Get(0);

    NodeContainer Gateways_NodeContainer;
    Gateways_NodeContainer.Create(2);
    Ptr<Node> Gateway1_Node = Gateways_NodeContainer.Get(0);
    Ptr<Node> Gateway2_Node = Gateways_NodeContainer.Get(1);

    NodeContainer APs_NodeContainer;
    APs_NodeContainer.Create(2);
    Ptr<Node> AP1_Node = APs_NodeContainer.Get(0);
    Ptr<Node> AP2_Node = APs_NodeContainer.Get(1);

    // NodeContainer Users_NodeContainer;
    // Users_NodeContainer.Create(user_num);
    // Ptr<Node>     Users_Node  [user_num];
    // for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    // {
    //     Users_Node[user_idx] = Users_NodeContainer.Get(user_idx);
    // }

    NodeContainer UsersAntenna1_NodeContainer;
    UsersAntenna1_NodeContainer.Create(user_num);
    Ptr<Node>     UsersAntenna1_Node  [user_num];
    for(int antenna1_idx=0 ; antenna1_idx<user_num ; antenna1_idx++)
    {
        UsersAntenna1_Node[antenna1_idx] = UsersAntenna1_NodeContainer.Get(antenna1_idx);
    }

    NodeContainer UsersAntenna2_NodeContainer;
    UsersAntenna2_NodeContainer.Create(user_num);
    Ptr<Node>     UsersAntenna2_Node  [user_num];
    for(int antenna2_idx=0 ; antenna2_idx<user_num ; antenna2_idx++)
    {
        UsersAntenna2_Node[antenna2_idx] = UsersAntenna2_NodeContainer.Get(antenna2_idx);
    }

    NodeContainer Users_NodeContainer;
    Users_NodeContainer.Create(user_num);
    Ptr<Node>     Users_Node  [user_num];
    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        Users_Node[user_idx] = Users_NodeContainer.Get(user_idx);
    }

    NodeContainer UsersAntennas_NodeContainer[user_num][2];
    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        UsersAntennas_NodeContainer[user_idx][0] = NodeContainer(UsersAntenna1_Node[user_idx] , Users_Node[user_idx]);
        UsersAntennas_NodeContainer[user_idx][1] = NodeContainer(UsersAntenna2_Node[user_idx] , Users_Node[user_idx]);
    }
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Create Ether Channel" << string(5, '-') << '\n';

    /*----------------Create Ether Channel--------------------------------------------------------------------------------------------------------------------------------------------------*/
    ns3_ClassSRNC00_PointToPointHelper Channel_PointToPointHelper; 
    Channel_PointToPointHelper.SetDeviceAttribute ("DataRate"           , StringValue("4Gbps")     );
    Channel_PointToPointHelper.SetChannelAttribute("Delay"              , TimeValue(Seconds(0.000)));
    Channel_PointToPointHelper.SetQueue           ("ns3::DropTailQueue" , "MaxSize" , QueueSizeValue(QueueSize("4294967295p")));

    PointToPointHelper ChannelByPass_PointToPointHelper; // This Ehter channel is for users send ack-pkt to edge server
    ChannelByPass_PointToPointHelper.SetDeviceAttribute ("DataRate"           , StringValue("3Gbps")     );
    ChannelByPass_PointToPointHelper.SetChannelAttribute("Delay"              , TimeValue(Seconds(0.005)));
    ChannelByPass_PointToPointHelper.SetQueue           ("ns3::DropTailQueue" , "MaxSize" , QueueSizeValue(QueueSize("4294967295p")));

    NodeContainer ServerConnectGateway1_NodeContainer = NodeContainer(EdgeServer_Node , Gateway1_Node);
    NetDeviceContainer ServerConnectGateway1_NetDeviceContainer   = 
    Channel_PointToPointHelper.Install(ServerConnectGateway1_NodeContainer);
    
    NodeContainer Gateway1ConnectGateway2_NodeContainer = NodeContainer(Gateway1_Node , Gateway2_Node);
    NetDeviceContainer Gateway1ConnectGateway2_NetDeviceContainer = 
    Channel_PointToPointHelper.Install(Gateway1ConnectGateway2_NodeContainer);

    NodeContainer Gateway2ConnectAP1_NodeContainer = NodeContainer(Gateway2_Node , AP1_Node);
    NetDeviceContainer Gateway2ConnectAP1_NetDeviceContainer      = 
    Channel_PointToPointHelper.Install(Gateway2ConnectAP1_NodeContainer);

    NodeContainer Gateway2ConnectAP2_NodeContainer = NodeContainer(Gateway2_Node , AP2_Node);
    NetDeviceContainer Gateway2ConnectAP2_NetDeviceContainer      = 
    Channel_PointToPointHelper.Install(Gateway2ConnectAP2_NodeContainer);  

    NetDeviceContainer Antenna1ConnectAntenna2_NetDeviceContainer[user_num][2];
    for(int user_containeridx=0 ; user_containeridx<user_num ; user_containeridx++)
    {
        Antenna1ConnectAntenna2_NetDeviceContainer[user_containeridx][0] = 
        Channel_PointToPointHelper.Install(UsersAntennas_NodeContainer[user_containeridx][0]);

        Antenna1ConnectAntenna2_NetDeviceContainer[user_containeridx][1] = 
        Channel_PointToPointHelper.Install(UsersAntennas_NodeContainer[user_containeridx][1]);
    }    
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Create Wireless Channel from antennas1 to AP1" << string(5, '-') << '\n';

    /*----------------Create Wireless Channel from antennas1 to AP1------------------------------------------------------------------------------------------------------------------------*/
    DmgWifiChannelHelper AP1Channel_DmgWifiChannelHelper;
    AP1Channel_DmgWifiChannelHelper.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    AP1Channel_DmgWifiChannelHelper.AddPropagationLoss ("ns3::FriisPropagationLossModel" , "Frequency" , DoubleValue (60.48e9));

    DmgWifiPhyHelper AP1WiFiPhy_DmgWifiPhyHelper = DmgWifiPhyHelper::Default();
    AP1WiFiPhy_DmgWifiPhyHelper.SetChannel(AP1Channel_DmgWifiChannelHelper.Create());
    AP1WiFiPhy_DmgWifiPhyHelper.Set("TxPowerStart"   , DoubleValue(10.0) );
    AP1WiFiPhy_DmgWifiPhyHelper.Set("TxPowerEnd"     , DoubleValue(10.0) );
    AP1WiFiPhy_DmgWifiPhyHelper.Set("TxPowerLevels"  , UintegerValue(1)  );
    AP1WiFiPhy_DmgWifiPhyHelper.Set("ChannelNumber"  , UintegerValue(2)  );
    AP1WiFiPhy_DmgWifiPhyHelper.Set("SupportOfdmPhy" , BooleanValue(true));
    if(WifiStandard == "802.11ay")
    {
        AP1WiFiPhy_DmgWifiPhyHelper.SetErrorRateModel(
            "ns3::DmgErrorModel","FileName", 
            StringValue("WigigFiles/ErrorModel/LookupTable_1458_ay.txt")
        );
    }

    Ssid AP1_Ssid = Ssid("AP1");
    DmgWifiMacHelper AP1Mac_DmgWifiMacHelper = DmgWifiMacHelper::Default();
    AP1Mac_DmgWifiMacHelper.SetType(
        "ns3::DmgApWifiMac" ,
        "Ssid"              , SsidValue(AP1_Ssid)              ,
        "BE_MaxAmpduSize"   , StringValue(MpduAggregationSize) ,
        "BE_MaxAmsduSize"   , StringValue(MsduAggregationSize) ,
        "SSSlotsPerABFT"    , UintegerValue(8)                 , 
        "SSFramesPerSlot"   , UintegerValue(8)                 ,
        "BeaconInterval"    , TimeValue(MicroSeconds (102400)) ,
        "EDMGSupported"     , BooleanValue((WifiStandard == "802.11ay"))
    );

    NetDeviceContainer AP1_NetDeviceContainer = WiFi_DmgWifiHelper.Install(
        AP1WiFiPhy_DmgWifiPhyHelper , AP1Mac_DmgWifiMacHelper , AP1_Node
    );

    AP1Mac_DmgWifiMacHelper.SetType(
        "ns3::DmgStaWifiMac" , 
        "Ssid"               , SsidValue(AP1_Ssid)                , 
        "ActiveProbing"      , BooleanValue(false)                ,
        "BE_MaxAmpduSize"    , StringValue(MpduAggregationSize)   ,
        "BE_MaxAmsduSize"    , StringValue(MsduAggregationSize)   ,
        "EDMGSupported"      , BooleanValue((WifiStandard == "802.11ay"))
    );

    NetDeviceContainer Antenna1ConnectAP1_NetDeviceContainer[user_num];
    for(int antenna1_idx=0 ; antenna1_idx<user_num ; antenna1_idx++)
    {
        Antenna1ConnectAP1_NetDeviceContainer[antenna1_idx] = WiFi_DmgWifiHelper.Install(
            AP1WiFiPhy_DmgWifiPhyHelper , AP1Mac_DmgWifiMacHelper , UsersAntenna1_Node[antenna1_idx]
        );
    }
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Create Wireless Channel from antennas2 to AP2" << string(5, '-') << '\n';

    /*----------------Create Wireless Channel from antennas2 to AP2------------------------------------------------------------------------------------------------------------------------*/
    /*Wifi Physical Setting for AP2*/
    DmgWifiChannelHelper AP2WiFiChannel_DmgWifiChannelHelper;
    AP2WiFiChannel_DmgWifiChannelHelper.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    AP2WiFiChannel_DmgWifiChannelHelper.AddPropagationLoss  ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (60.48e9));

    DmgWifiPhyHelper AP2WiFiPhy_DmgWifiPhyHelper = DmgWifiPhyHelper::Default ();
    AP2WiFiPhy_DmgWifiPhyHelper.SetChannel(AP2WiFiChannel_DmgWifiChannelHelper.Create());
    AP2WiFiPhy_DmgWifiPhyHelper.Set ("TxPowerStart"  , DoubleValue (10.0) );
    AP2WiFiPhy_DmgWifiPhyHelper.Set ("TxPowerEnd"    , DoubleValue (10.0) );
    AP2WiFiPhy_DmgWifiPhyHelper.Set ("TxPowerLevels" , UintegerValue (1)  );
    AP2WiFiPhy_DmgWifiPhyHelper.Set ("ChannelNumber" , UintegerValue (2)  );
    AP2WiFiPhy_DmgWifiPhyHelper.Set ("SupportOfdmPhy", BooleanValue (true));
    if (WifiStandard=="802.11ay")
    {
        AP2WiFiPhy_DmgWifiPhyHelper.SetErrorRateModel(
            "ns3::DmgErrorModel" , "FileName" , 
            StringValue("WigigFiles/ErrorModel/LookupTable_1458_ay.txt") 
        );
    }

    Ssid AP2_Ssid = Ssid("AP2");
    DmgWifiMacHelper AP2Mac_DmgWifiMacHelper = DmgWifiMacHelper::Default();
    AP2Mac_DmgWifiMacHelper.SetType(
        "ns3::DmgApWifiMac" ,
        "Ssid"              , SsidValue(AP2_Ssid)                ,
        "BE_MaxAmpduSize"   , StringValue(MpduAggregationSize)   ,
        "BE_MaxAmsduSize"   , StringValue(MsduAggregationSize)   ,
        "SSSlotsPerABFT"    , UintegerValue(8)                   , 
        "SSFramesPerSlot"   , UintegerValue(8)                   ,
        "BeaconInterval"    , TimeValue(MicroSeconds (102400))   ,
        "EDMGSupported"     , BooleanValue((WifiStandard == "802.11ay"))
    );

    NetDeviceContainer AP2_NetDeviceContainer = WiFi_DmgWifiHelper.Install(
        AP2WiFiPhy_DmgWifiPhyHelper , AP2Mac_DmgWifiMacHelper , AP2_Node
    );

    AP2Mac_DmgWifiMacHelper.SetType(
        "ns3::DmgStaWifiMac" , 
        "Ssid"               , SsidValue(AP2_Ssid)                , 
        "ActiveProbing"      , BooleanValue(false)                ,
        "BE_MaxAmpduSize"    , StringValue(MpduAggregationSize)   ,
        "BE_MaxAmsduSize"    , StringValue(MsduAggregationSize)   ,
        "EDMGSupported"      , BooleanValue((WifiStandard == "802.11ay"))
    );

    NetDeviceContainer Antenna2ConnectAP2_NetDeviceContainer[user_num];
    for(int antenna2_idx=0 ; antenna2_idx<user_num ; antenna2_idx++)
    {
        Antenna2ConnectAP2_NetDeviceContainer[antenna2_idx] = WiFi_DmgWifiHelper.Install(
            AP2WiFiPhy_DmgWifiPhyHelper , AP2Mac_DmgWifiMacHelper , UsersAntenna2_Node[antenna2_idx]
        );
    }
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Set Nodes' Location and mobility" << string(5, '-') << '\n';

    /*----------------Set Nodes' Location and mobility------------------------------------------------------------------------------------------------------------------------*/
    Ptr<ListPositionAllocator> PositionAlloc_ListPositionAllocator = CreateObject<ListPositionAllocator>();

    PositionAlloc_ListPositionAllocator->Add(Vector(0.0 , 0.0 , 10.0));  //EdgeServer_Node
    PositionAlloc_ListPositionAllocator->Add(Vector(0.0 , 0.0 ,  9.0));  //Gateway1_Node
    PositionAlloc_ListPositionAllocator->Add(Vector(0.0 , 0.0 ,  8.0));  //Gateway2_Node
    PositionAlloc_ListPositionAllocator->Add(Vector(0.0 , 0.0 ,  6.0));  //AP1_Node
    PositionAlloc_ListPositionAllocator->Add(Vector(0.0 , 0.0 ,  0.0));  //AP2_Node

    for(int character_idx=1 ; character_idx<=3 ; character_idx++)
    {
        /*first  time is for antenna1*/
        /*second time is for antenna2*/
        /*third  time is for userX   */

        double PositionStartPoint[3] = {0.0 , 0.0 , 3.0};
        double radius = 1.0;
        for(int user_idx = 0; user_idx < user_num; user_idx++)
        {
            double angle = user_idx * 2 * 3.14159 / user_num;
            double x = PositionStartPoint[0] + radius * cos(angle);
            double y = PositionStartPoint[1] + radius * sin(angle);
            double z = PositionStartPoint[2];

            PositionAlloc_ListPositionAllocator->Add(Vector(x , y , z)); // this location is for userX atanna1
        }
    }
    
    MobilityHelper Mobile_MobilityHelper;
    Mobile_MobilityHelper.SetPositionAllocator(PositionAlloc_ListPositionAllocator);
    Mobile_MobilityHelper.SetMobilityModel("ns3::ConstantVelocityMobilityModel"   );
    Mobile_MobilityHelper.Install(EdgeServer_NodeContainer   );
    Mobile_MobilityHelper.Install(Gateways_NodeContainer     );
    Mobile_MobilityHelper.Install(APs_NodeContainer          );
    Mobile_MobilityHelper.Install(UsersAntenna1_NodeContainer);
    Mobile_MobilityHelper.Install(UsersAntenna2_NodeContainer);
    Mobile_MobilityHelper.Install(Users_NodeContainer        );

    ns3_PrintNodePositions_FunctionPhysicalInfo1(EdgeServer_NodeContainer    , "EdgeServer");
    cout << string(25 , '_') << endl;
    ns3_PrintNodePositions_FunctionPhysicalInfo1(Gateways_NodeContainer      , "Gateways"  );
    cout << string(25 , '_') << endl;
    ns3_PrintNodePositions_FunctionPhysicalInfo1(APs_NodeContainer           , "APs"       );
    cout << string(25 , '_') << endl;
    ns3_PrintNodePositions_FunctionPhysicalInfo1(Users_NodeContainer         , "Users"     );
    cout << string(25 , '_') << endl;
    ns3_PrintNodePositions_FunctionPhysicalInfo1(UsersAntenna1_NodeContainer , "Antenna1"  );
    cout << string(25 , '_') << endl;
    ns3_PrintNodePositions_FunctionPhysicalInfo1(UsersAntenna2_NodeContainer , "Antenna2"  );
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "IPv4 address assign" << string(5, '-') << '\n';

    /*----------------IPv4 address assign--------------------------------------------------------------------------------------------------------------------------------------------------*/
    InternetStackHelper Stack_InternetStackHelper;
    Stack_InternetStackHelper.Install(EdgeServer_NodeContainer   );
    Stack_InternetStackHelper.Install(Gateways_NodeContainer     );
    Stack_InternetStackHelper.Install(APs_NodeContainer          );
    Stack_InternetStackHelper.Install(Users_NodeContainer        );
    Stack_InternetStackHelper.Install(UsersAntenna1_NodeContainer);
    Stack_InternetStackHelper.Install(UsersAntenna2_NodeContainer);

    Ipv4AddressHelper Address_IPv4AddressHelper;
    Address_IPv4AddressHelper.SetBase("192.168.1.0" , "255.255.255.0");
    Ipv4InterfaceContainer ServerConnectGateway1_Ipv4InterfaceContainer   = Address_IPv4AddressHelper.Assign(
        ServerConnectGateway1_NetDeviceContainer
    );
    cout << "Build Network 192.168.1.0 Successfully\n";

    Address_IPv4AddressHelper.SetBase("192.168.2.0" , "255.255.255.0");
    Ipv4InterfaceContainer Gateway1ConnectGateway2_Ipv4InterfaceContainer = Address_IPv4AddressHelper.Assign(
        Gateway1ConnectGateway2_NetDeviceContainer
    );
    cout << "Build Network 192.168.2.0 Successfully\n";

    Address_IPv4AddressHelper.SetBase("192.168.3.0" , "255.255.255.0");
    Ipv4InterfaceContainer Gateway2ConnectAPs_NetDeviceContainer = Address_IPv4AddressHelper.Assign(
        Gateway2ConnectAP1_NetDeviceContainer
    );
    Address_IPv4AddressHelper.Assign(
        Gateway2ConnectAP2_NetDeviceContainer
    );
    cout << "Build Network 192.168.3.0 Successfully\n";

    Address_IPv4AddressHelper.SetBase("192.168.4.0" , "255.255.255.0");
    Ipv4InterfaceContainer AP1_Ipv4InterfaceContainer = Address_IPv4AddressHelper.Assign(
        AP1_NetDeviceContainer
    );
    Ipv4InterfaceContainer UsersAntenna1_Ipv4InterfaceContainer[user_num];
    for(int antenna1_idx=0 ; antenna1_idx<user_num ; antenna1_idx++)
    {
        UsersAntenna1_Ipv4InterfaceContainer[antenna1_idx] = Address_IPv4AddressHelper.Assign(
            Antenna1ConnectAP1_NetDeviceContainer[antenna1_idx]
        );
    }
    cout << "Build Network 192.168.4.0 Successfully\n";

    Address_IPv4AddressHelper.SetBase("192.168.5.0" , "255.255.255.0");
    Ipv4InterfaceContainer AP2_Ipv4InterfaceContainer = Address_IPv4AddressHelper.Assign(
        AP2_NetDeviceContainer
    );
    Ipv4InterfaceContainer UsersAntenna2_Ipv4InterfaceContainer[user_num];
    for(int antenna2_idx=0 ; antenna2_idx<user_num ; antenna2_idx++)
    {
        UsersAntenna2_Ipv4InterfaceContainer[antenna2_idx] = Address_IPv4AddressHelper.Assign(
            Antenna2ConnectAP2_NetDeviceContainer[antenna2_idx]
        );
    }
    cout << "Build Network 192.168.5.0 Successfully\n";

    Address_IPv4AddressHelper.SetBase("192.168.6.0" , "255.255.255.0");
    Ipv4InterfaceContainer UsersConnectAntennas_Ipv4InterfaceContainer[user_num][2];
    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        UsersConnectAntennas_Ipv4InterfaceContainer[user_idx][0] = Address_IPv4AddressHelper.Assign(
            Antenna1ConnectAntenna2_NetDeviceContainer[user_idx][0]
        );
        UsersConnectAntennas_Ipv4InterfaceContainer[user_idx][1] = Address_IPv4AddressHelper.Assign(
            Antenna1ConnectAntenna2_NetDeviceContainer[user_idx][1]
        );
    }
    cout << "Build Network 192.168.6.0 Successfully\n";

    /* Populate routing table */
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    cout << "Build Each Network-Area RouteTable Successfully\n";
    /* We do not want any ARP packets */
    PopulateArpCache ();
    cout << "ARP Disable Successfully\n";

    Address_IPv4AddressHelper.SetBase("192.168.7.0" , "255.255.255.0");
    NodeContainer      UsersConnectServer_NodeContainer     [user_num];
    NetDeviceContainer UsersConnectServer_NetDeviceContainer[user_num];
    Ipv4InterfaceContainer UsersConnectServer_Ipv4InterfaceContainer[user_num];
    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        UsersConnectServer_NodeContainer     [user_idx] = NodeContainer(EdgeServer_Node , Users_Node[user_idx]);
        UsersConnectServer_NetDeviceContainer[user_idx] = ChannelByPass_PointToPointHelper.Install(
            UsersConnectServer_NodeContainer [user_idx]
        );
        UsersConnectServer_Ipv4InterfaceContainer[user_idx] = Address_IPv4AddressHelper.Assign(
            UsersConnectServer_NetDeviceContainer[user_idx]
        );
    }
    cout << "Build Network 192.168.7.0 Successfully\n";
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Setting up static routing" << string(5, '-') << '\n';

    /*----------Setting up static routing From 192.168.1.0 to 192.168.4.0 & 192.168.5.0----------------------------------------------------------------------------------------------------*/
    Ptr<SimpleNetDevice> EdgeServer_NetworkDevice = CreateObject<SimpleNetDevice>();
    EdgeServer_NetworkDevice->SetAddress(Mac48Address::Allocate());
    EdgeServer_Node         ->AddDevice(EdgeServer_NetworkDevice );
    Ptr<Ipv4> EdgeServer_Ipv4                        = EdgeServer_Node->GetObject<Ipv4>();
    uint32_t  EdgeServer_InterfaceIDX                = EdgeServer_Ipv4->AddInterface(EdgeServer_NetworkDevice);
    Ipv4InterfaceAddress EdgeServer_InterfaceAddress = Ipv4InterfaceAddress(Ipv4Address("192.168.1.0") , Ipv4Mask("255.255.255.0"));
    EdgeServer_Ipv4->AddAddress(EdgeServer_InterfaceIDX , EdgeServer_InterfaceAddress);
    EdgeServer_Ipv4->SetMetric (EdgeServer_InterfaceIDX , 1);
    EdgeServer_Ipv4->SetUp     (EdgeServer_InterfaceIDX);
    
    // Server (192.168.1.0) -> Gateway1 (192.168.6.X)
    Ipv4StaticRoutingHelper EdgeServer_ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> staticRouting_Server = EdgeServer_ipv4RoutingHelper.GetStaticRouting(EdgeServer_Ipv4);
    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        staticRouting_Server->AddHostRouteTo(
            UsersConnectAntennas_Ipv4InterfaceContainer[user_idx][0].GetAddress(1) , 1
        );
    }
    
    // Server (192.168.1.0) -> Gateway1 (192.168.6.X)
    Ptr<SimpleNetDevice>    Users_NetworkDevice    [user_num];
    Ptr<Ipv4>               Users_Ipv4             [user_num];
    int32_t                 Users_InterfaceIDX     [user_num];
    Ipv4InterfaceAddress    Users_InterfaceAddress [user_num];
    Ipv4StaticRoutingHelper Users_ipv4RoutingHelper[user_num];
    Ptr<Ipv4StaticRouting>  staticRouting_Users    [user_num];
    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        Users_NetworkDevice[user_idx] = CreateObject<SimpleNetDevice>();
        Users_NetworkDevice[user_idx] ->SetAddress(Mac48Address::Allocate());
        Users_Node         [user_idx] ->AddDevice(Users_NetworkDevice[user_idx]);
        Users_Ipv4         [user_idx] = Users_Node[user_idx]->GetObject<Ipv4>();
        Users_InterfaceIDX [user_idx] = Users_Ipv4[user_idx]->AddInterface(Users_NetworkDevice[user_idx]);
        Users_InterfaceAddress[user_idx] = Ipv4InterfaceAddress(
            Ipv4Address("192.168.6.0") , Ipv4Mask("255.255.255.0")
        );
        
        Users_Ipv4[user_idx]->AddAddress(
            Users_InterfaceIDX[user_idx] , Users_InterfaceAddress[user_idx]
        );
        Users_Ipv4[user_idx]->SetMetric(
            Users_InterfaceIDX[user_idx] , 1
        );
        Users_Ipv4[user_idx]->SetUp(
            Users_InterfaceIDX[user_idx]
        );

        staticRouting_Users[user_idx] = Users_ipv4RoutingHelper[user_idx].GetStaticRouting(Users_Ipv4[user_idx]);
        staticRouting_Users[user_idx] ->AddHostRouteTo(Ipv4Address("192.168.1.1") , 3);
    }

    cout << EdgeServer_NetworkDevice << '\n';
    for(int i=0 ; i<UserNum ; i++)
    {
        cout << Users_NetworkDevice[i] << '\n';
    }

    cout << string(10,'~') << "Server Node" << string(10,'~') << endl;
    ns3_PrintRoutingTable_FunctionRouteTable1(EdgeServer_Node);
    cout << "\n\n";

    cout << string(10,'~') << "Gateway1" << string(10,'~') << endl;
    ns3_PrintRoutingTable_FunctionRouteTable1(Gateway1_Node);
    cout << "\n\n";

    cout << string(10,'~') << "Gateway2" << string(10,'~') << endl;
    ns3_PrintRoutingTable_FunctionRouteTable1(Gateway2_Node);
    cout << "\n\n";

    cout << string(10,'~') << "AP1" << string(10,'~') << endl;
    ns3_PrintRoutingTable_FunctionRouteTable1(AP1_Node);
    cout << "\n\n";

    cout << string(10,'~') << "AP2" << string(10,'~') << endl;
    ns3_PrintRoutingTable_FunctionRouteTable1(AP2_Node);
    cout << "\n\n";

    cout << string(10,'~') << "UsersAntennas" << string(10,'~') << endl;
    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        cout << "User" << user_idx << endl;
        ns3_PrintRoutingTable_FunctionRouteTable1(UsersAntenna1_Node[user_idx]);
        cout << string(25,'_') <<endl;
        ns3_PrintRoutingTable_FunctionRouteTable1(UsersAntenna2_Node[user_idx]);
        cout << string(25,'_') <<endl;
        ns3_PrintRoutingTable_FunctionRouteTable1(Users_Node[user_idx]);
        cout << string(25,'_') <<endl;
    }
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Get MAC Info" << string(5, '-') << '\n';

    /*----------------Get MAC48Address & Ipv4Address of all nodes----------------------------------------------------------------------------------------------------------------------------------------*/
    cout << string(10,'~') << "Server Node" << string(10,'~') << endl;
    ns3_PrintIpv4Address_FunctionLayer3Info2     (EdgeServer_Node);
    ns3_PrintNodeMacAddresses_FunctionLayer2Info1(EdgeServer_Node);
    cout << string(10,'~') << "Gateway1" << string(10,'~') << endl;
    ns3_PrintIpv4Address_FunctionLayer3Info2     (Gateway1_Node  );
    ns3_PrintNodeMacAddresses_FunctionLayer2Info1(Gateway1_Node  );
    cout << string(10,'~') << "Gateway2" << string(10,'~') << endl;
    ns3_PrintIpv4Address_FunctionLayer3Info2     (Gateway2_Node  );
    ns3_PrintNodeMacAddresses_FunctionLayer2Info1(Gateway2_Node  );
    cout << string(10,'~') << "AP1     " << string(10,'~') << endl;
    ns3_PrintIpv4Address_FunctionLayer3Info2     (AP1_Node       );
    ns3_PrintNodeMacAddresses_FunctionLayer2Info1(AP1_Node       );
    cout << string(10,'~') << "AP2     " << string(10,'~') << endl;
    ns3_PrintIpv4Address_FunctionLayer3Info2     (AP2_Node       );
    ns3_PrintNodeMacAddresses_FunctionLayer2Info1(AP2_Node       );

    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        cout << string(10,'~') << "User<" << user_idx << ">" << string(10,'~') << endl;
        ns3_PrintIpv4Address_FunctionLayer3Info2     (Users_Node        [user_idx]);
        ns3_PrintNodeMacAddresses_FunctionLayer2Info1(Users_Node        [user_idx]);
        ns3_PrintIpv4Address_FunctionLayer3Info2     (UsersAntenna1_Node[user_idx]);
        ns3_PrintNodeMacAddresses_FunctionLayer2Info1(UsersAntenna1_Node[user_idx]);
        ns3_PrintIpv4Address_FunctionLayer3Info2     (UsersAntenna2_Node[user_idx]);
        ns3_PrintNodeMacAddresses_FunctionLayer2Info1(UsersAntenna2_Node[user_idx]);
    }
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    
    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Install TCP Sink" << string(5, '-') << '\n';

    /*----------------Install TCP Sink----------------------------------------------------------------------------------------------------------------------------------------------------*/
    ApplicationContainer UsersSink_ApplicationContainer    [user_num];
    // Ptr<PacketSink>      Users_PacketSink               [user_num];
    std::vector<ns3::Ptr<ns3::PacketSink>> Users_PacketSink(user_num);
    ApplicationContainer ServerSink_ApplicationContainer   [user_num];

    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        ns3_ForUsers_InstallTCPRxSink_TCPApp1(
            &(UsersSink_ApplicationContainer[user_idx]) , 
            &(Users_PacketSink              [user_idx]) , 
            Users_Node[user_idx]    ,    (uint32_t)port , 
            0.0 , SimulationTime
        );

        ns3_ForServer_InstallTCPTxSink_TCPApp2(
            &(ServerSink_ApplicationContainer[user_idx])  , 
            EdgeServer_Node , (uint32_t)((int)port)          , 
            PayloadSize , DataRate , 1.4 , SimulationTime    , 
            UsersConnectAntennas_Ipv4InterfaceContainer[user_idx][0].GetAddress(1)
        );

        port--;
    }
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Put Trace NetworkPerformance software into nodes" << string(5, '-') << '\n';

    /*----------------Put Trace NetworkPerformance software into nodes---------------------------------------------------------------------------------------------------------------------*/
    Ptr<WifiNetDevice> AP1_WiFiNetDevice              = StaticCast<WifiNetDevice>(AP1_NetDeviceContainer.Get(0));
    Ptr<DmgApWifiMac> AP1Mac_DmgApWifiMac_ForTraceLog = StaticCast<DmgApWifiMac> (AP1_WiFiNetDevice->GetMac()  );

    Ptr<WifiNetDevice> AP2_WiFiNetDevice              = StaticCast<WifiNetDevice>(AP2_NetDeviceContainer.Get(0));
    Ptr<DmgApWifiMac> AP2Mac_DmgApWifiMac_ForTraceLog = StaticCast<DmgApWifiMac> (AP2_WiFiNetDevice->GetMac()  );    
    
    Ptr<WifiNetDevice> Antenna1ConnectAP1_WiFiNetDevice            [user_num];
    Ptr<DmgStaWifiMac> Antenna1ConnectAP1_DmgStaWifiMac_ForTraceLog[user_num];
    Ptr<WifiNetDevice> Antenna2ConnectAP2_WiFiNetDevice            [user_num];
    Ptr<DmgStaWifiMac> Antenna2ConnectAP2_DmgStaWifiMac_ForTraceLog[user_num];
    
    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        Antenna1ConnectAP1_WiFiNetDevice[user_idx] = StaticCast<WifiNetDevice>(
            Antenna1ConnectAP1_NetDeviceContainer[user_idx].Get(0)
        );
        Antenna1ConnectAP1_DmgStaWifiMac_ForTraceLog[user_idx] = StaticCast<DmgStaWifiMac>(
            Antenna1ConnectAP1_WiFiNetDevice[user_idx]->GetMac()
        );
        auto DictForAntenna1 = dict<string , int>(
            {
                {"user-idx"   , user_idx}  , 
                {"antenna-id" , 1       }  ,
                {"ap-id"      , 1       }  ,
                {"AllocType"  , (int)AllocationType}
            }
        );
        Antenna1ConnectAP1_DmgStaWifiMac_ForTraceLog[user_idx]->TraceConnectWithoutContext(
            "Assoc"        , 
            MakeBoundCallback(
                &ns3_ForAntennas_AssociationHappened_FunctionAPtoUsers1, 
                Antenna1ConnectAP1_DmgStaWifiMac_ForTraceLog[user_idx] , 
                AP1Mac_DmgApWifiMac_ForTraceLog    , DictForAntenna1
            )
        );
        Antenna1ConnectAP1_DmgStaWifiMac_ForTraceLog[user_idx]->TraceConnectWithoutContext(
            "SLSCompleted" ,
            MakeBoundCallback(
                &ns3_ForAntennas_SLSCompleted_FunctionAPtoUsers2 , 
                Antenna1ConnectAP1_DmgStaWifiMac_ForTraceLog[user_idx]
            )
        );

        Antenna2ConnectAP2_WiFiNetDevice[user_idx] = StaticCast<WifiNetDevice>(
            Antenna2ConnectAP2_NetDeviceContainer[user_idx].Get(0)
        );
        Antenna2ConnectAP2_DmgStaWifiMac_ForTraceLog[user_idx] = StaticCast<DmgStaWifiMac>(
            Antenna2ConnectAP2_WiFiNetDevice[user_idx]->GetMac()
        );
        auto DictForAntenna2 = dict<string , int>(
            {
                {"user-idx"   , user_idx}  , 
                {"antenna-id" , 2       }  ,
                {"ap-id"      , 2       }  ,
                {"AllocType"  , (int)AllocationType}
            }
        );
        Antenna2ConnectAP2_DmgStaWifiMac_ForTraceLog[user_idx]->TraceConnectWithoutContext(
            "Assoc" , 
            MakeBoundCallback(
                &ns3_ForAntennas_AssociationHappened_FunctionAPtoUsers1  , 
                Antenna2ConnectAP2_DmgStaWifiMac_ForTraceLog[user_idx]   , 
                AP2Mac_DmgApWifiMac_ForTraceLog    , DictForAntenna2
            )
        );
        Antenna2ConnectAP2_DmgStaWifiMac_ForTraceLog[user_idx]->TraceConnectWithoutContext(
            "SLSCompleted" , 
            MakeBoundCallback(
                &ns3_ForAntennas_SLSCompleted_FunctionAPtoUsers2 , 
                Antenna2ConnectAP2_DmgStaWifiMac_ForTraceLog[user_idx]
            )
        );
    }
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Scheduling" << string(5, '-') << '\n';

    /*----------------Scheduling-----------------------------------------------------------------------------------------------------------------------------------------------------------*/
    for(int user_idx=0 ; user_idx<user_num ; user_idx++)
    {
        Simulator::Schedule(
            Seconds(1.4) , 
            &ns3_ForEveryone_TCPThroughputMeasure_FunctionTCPMeasure1    , 
            UsersAntenna1_Ipv4InterfaceContainer[user_idx].GetAddress(0) , 
            UsersAntenna2_Ipv4InterfaceContainer[user_idx].GetAddress(0) , 
            &(Users_PacketSink[user_idx])  ,  &TotalRx[user_idx]  ,  &Throughput[user_idx] 
        );

        Simulator::Schedule(
            Seconds(1.4) , 
            &ns3_ForEveryone_SNRMeasure_FunctionTCPMeasure3        , 
            Antenna1ConnectAP1_DmgStaWifiMac_ForTraceLog[user_idx] , 
            AP1Mac_DmgApWifiMac_ForTraceLog
        );

        Simulator::Schedule(
            Seconds(1.4) , 
            &ns3_ForEveryone_SNRMeasure_FunctionTCPMeasure3        , 
            Antenna2ConnectAP2_DmgStaWifiMac_ForTraceLog[user_idx] , 
            AP2Mac_DmgApWifiMac_ForTraceLog 
        );

        Simulator::Schedule(
            Seconds(1.38) , 
            &ns3_ForAPandAntenna_SectorSweep_FunctionAPtoUsers3   , 
            Antenna1ConnectAP1_DmgStaWifiMac_ForTraceLog[user_idx] , 
            AP1Mac_DmgApWifiMac_ForTraceLog
        );

        Simulator::Schedule(
            Seconds(1.38) , 
            &ns3_ForAPandAntenna_SectorSweep_FunctionAPtoUsers3   , 
            Antenna2ConnectAP2_DmgStaWifiMac_ForTraceLog[user_idx] , 
            AP2Mac_DmgApWifiMac_ForTraceLog
        );
    }

    FlowMonitorHelper FlowMonitorProgram;
    Ptr<FlowMonitor>  MonitorDevice = FlowMonitorProgram.InstallAll();
    // MonitorDevice->SerializeToXmlFile("Traces/FlowMonitorResults.xml", true, true);
    string TraceFolderName = "Traces/";

    if(PcapTrace)
    {
        Channel_PointToPointHelper.EnablePcap(TraceFolderName+"Upper/Server"                , ServerConnectGateway1_NetDeviceContainer.Get(0)   , true);
        Channel_PointToPointHelper.EnablePcap(TraceFolderName+"Upper/Gateway1"              , Gateway1ConnectGateway2_NetDeviceContainer.Get(0) , true);
        Channel_PointToPointHelper.EnablePcap(TraceFolderName+"Upper/Gateway2ToAP1-Gateway" , Gateway2ConnectAP1_NetDeviceContainer.Get(0)      , true);
        Channel_PointToPointHelper.EnablePcap(TraceFolderName+"Upper/Gateway2ToAP1-AP1"     , Gateway2ConnectAP1_NetDeviceContainer.Get(1)      , true);
        Channel_PointToPointHelper.EnablePcap(TraceFolderName+"Upper/Gateway2ToAP2-Gateway" , Gateway2ConnectAP2_NetDeviceContainer.Get(0)      , true);
        Channel_PointToPointHelper.EnablePcap(TraceFolderName+"Upper/Gateway2ToAP2-AP2"     , Gateway2ConnectAP2_NetDeviceContainer.Get(1)      , true);

        AP1WiFiPhy_DmgWifiPhyHelper.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
        AP2WiFiPhy_DmgWifiPhyHelper.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
        AP1WiFiPhy_DmgWifiPhyHelper.EnablePcap(TraceFolderName+"Lower/AP1"   , AP1_NetDeviceContainer   , true);
        AP2WiFiPhy_DmgWifiPhyHelper.EnablePcap(TraceFolderName+"Lower/AP2"   , AP2_NetDeviceContainer   , true);
        for(int user_idx=0 ; user_idx<user_num ; user_idx++)
        {
            string userx_antenna1_pcap = TraceFolderName + "Lower/user" + to_string(user_idx+1) + "_antenna1";
            string userx_antenna2_pcap = TraceFolderName + "Lower/user" + to_string(user_idx+1) + "_antenna2";
            AP1WiFiPhy_DmgWifiPhyHelper.EnablePcap(userx_antenna1_pcap , Antenna1ConnectAP1_NetDeviceContainer[user_idx] , false);
            AP2WiFiPhy_DmgWifiPhyHelper.EnablePcap(userx_antenna2_pcap , Antenna2ConnectAP2_NetDeviceContainer[user_idx] , false);
        }

        for(int user_idx=0 ; user_idx<user_num ; user_idx++)
        {
            string ByPassPcap = TraceFolderName + "ByPass/user" + to_string(user_idx+1);
            Channel_PointToPointHelper.EnablePcap(ByPassPcap , UsersConnectServer_NetDeviceContainer[user_idx].Get(0) , true);
        }
    }
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    cout << "\n\n" << string(5, '-') << left << setfill('-') << setw(45) << "Start to run..." << string(5, '-') << '\n';

    /*----------------Start to run---------------------------------------------------------------------------------------------------------------------------------------------------------*/
    Simulator::Stop(Seconds(SimulationTime+0.001));
    Simulator::Run    ();
    Simulator::Destroy();


    ns3_ForEveryone_FlowMonitorStatistics_FunctionTCPMeasure5(
        FlowMonitorProgram , MonitorDevice , SimulationTime-1.0
    );

    return 0;
}
