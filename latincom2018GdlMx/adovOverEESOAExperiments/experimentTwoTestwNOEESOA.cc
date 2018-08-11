/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:  Luis Fernando Gutierrez Preciado <lgutierrez@iteso.mx>
 *          Francisco Eduardo Balart Sanchez <balart40@hotmail.com> 
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/energy-module.h"
#include "ns3/netanim-module.h"
#include "ns3/eesoa.h"
#include "ns3/aodv-module.h"
#include "ns3/v4ping-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

NS_LOG_COMPONENT_DEFINE ("testEESOA_with_Routing_aug_08_2018");

using namespace ns3;
using namespace std;

// ****************GLOBAL IDENTIFICATION OF NODE ID, THESE ARE NOT REQUIRED TO BE  GLOBAL*******************
NodeContainer nodes;                    // nodes container
NetDeviceContainer devices;             // devices container
Ipv4InterfaceContainer interfaces;      // Ipv4 Interface container
YansWifiPhyHelper wifiPhy;
// below the path were the images of the 4 Hierarchies of Nodes are placed (Leader, Gateway, Bridge, Member)
string path = "/home/balart40/Desktop/balart40/ns/ns-allinone-3.25/ns-3.25/src/eesoa/model/eesoaImages/";
uint32_t type[4]; 
#define numOfNodes  100
#define numOfNodesDouble (double) numOfNodes
#define max_x 50.0
#define max_y 50.0
#define distance 12.0
#define percentage 5.0
#define percentageComp numOfNodesDouble*percentage/100
#define simTime 61
#define percentageOfNodesTx 10
#define numOfNodesTx (uint32_t) numOfNodes*percentageOfNodesTx/100
#define nodesDeltaSeparation 4.0
#define pingStarttime 10.0
#define aodvHelloEnabled true
#define defaultSeed 266

uint16_t prev_role[numOfNodes];
uint16_t prev_nNodes[numOfNodes];

// ***************** NET ANIM PART ****************************
/*static void modify(Ptr<WifiNetDevice> wifiNetDevicePointer, AnimationInterface * anim)
{
    uint16_t nodeId = wifiNetDevicePointer->GetNode ()->GetId();
    Ptr<EESOA> mac = DynamicCast<EESOA> (wifiNetDevicePointer->GetMac());
    uint16_t role = mac->GetRole();
    uint16_t nNodes = mac->GetNumNeighbors();
    std::ostringstream temp;
    temp<<nodeId;
    //temp<<"[Id:"<<nodeId<<"]"<<mac->GetNeighbors()<<"[R:"<<role<<"]";
    if(prev_role[nodeId]!=role)
    {
      anim->UpdateNodeImage(nodeId,type[role]);
	    anim->UpdateNodeDescription(nodeId,temp.str());
    	anim->UpdateNodeSize(nodeId,1,1);
	    prev_role[nodeId]= role;
    }
    if(prev_nNodes[nodeId]!=nNodes)
    {	
      anim->UpdateNodeDescription(nodeId,temp.str());
	    prev_nNodes[nodeId] =nNodes;
    }
    Simulator::ScheduleWithContext (wifiNetDevicePointer->GetNode ()->GetId (), MilliSeconds(15), &modify, wifiNetDevicePointer,anim);
}*/

static void printNonEESOAtx(Ptr<WifiNetDevice> wifiNetDevicePointer)
{
  uint16_t nodeId = wifiNetDevicePointer->GetNode ()->GetId();
  Ptr<AdhocWifiMac> nDmac = DynamicCast<AdhocWifiMac> (wifiNetDevicePointer->GetMac());
  //Ptr<NeighborDiscovery> nDmac = DynamicCast<NeighborDiscovery> (wifiNetDevicePointer->GetMac());
  NS_LOG_UNCOND ("postprocessing time "<<Simulator::Now().GetSeconds()<<" Node "<<nodeId<<" has transmitted "<<nDmac->m_nonEesoaTxPacketCounter<<" Non eesoa packets");
  NS_LOG_UNCOND ("postprocessing time "<<Simulator::Now().GetSeconds()<<" Node "<<nodeId<<" has transmitted "<<nDmac->m_eesoaTxPacketCounter<<" eesoa packets");
  Simulator::ScheduleWithContext (wifiNetDevicePointer->GetNode ()->GetId (), Seconds(1), &printNonEESOAtx, wifiNetDevicePointer);
}

//remainingEnergy 
void RemainingEnergyTrace (std::string context, double oldValue, double remainingEnergy)
{
  NS_LOG_UNCOND ("%INFO: TimeStamp: "<<Simulator::Now ().GetSeconds ()<<" segs Node: "<<context<<" Current Remaining energy = "<<remainingEnergy<< " Joules");        
}

// Total energy
void TotalEnergy (std::string context, double oldValue, double totalEnergy)
{
  NS_LOG_UNCOND ("%INFO TimeStamp: "<<Simulator::Now ().GetSeconds ()<<" segs Total energy consumed Node: "<<context<<" "<<totalEnergy<< " Joules");
}

void PhyTxBegin (std::string context, Ptr<const Packet> p)
{
  NS_LOG_UNCOND (Simulator::Now().GetSeconds() << ": Node "<<context<<"Start sending packet");//<<*p<<"\n";
}

static inline std::string PrintID (Ptr<Node> n)
{
    uint32_t id = n->GetId();
    std::ostringstream oss;
    oss << "ID: " << id << "  ";
    return oss.str ();
}

//************************* MAIN FUNCTION ********************************************************************
int main (int argc, char *argv[])
{
  // Seed MANAGER 
  SeedManager::SetSeed (defaultSeed);
  //SeedManager::SetRun (run);

  bool   verbose = false;  

  CommandLine cmd;
  cmd.Parse (argc, argv);

  // Nodes creation
  nodes.Create (numOfNodes);

  //********************* CREATE NODES *************************************************************************
  LogComponentEnable ("testEESOA_with_Routing_aug_08_2018", LOG_LEVEL_ALL);
  NS_LOG_UNCOND ("%INFO: Creating Nodes...");

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable> ();
  // 1- STEP 1: Generate RANDOM POSITIONS FOR THE NODES
  double x = 0.0;
  double y = 0.0;
  for(int i = 0; i < numOfNodes; i++)
  {
    x = uv->GetValue(1.0, max_x);
    y = uv->GetValue(1.0, max_y);
    positionAlloc->Add (Vector (x, y, 10.0));
  }

  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  //mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds", RectangleValue (Rectangle (0, 1000, 0, 1000)));
  mobility.Install (nodes);
  double connectionsCounter = 0.0;
  double euclideanDistance = 0.0;
  // 2- STEP 2 ITERATE THROUGH EACH NODE POSITION
  NodeContainer::Iterator i = nodes.End ();
  while(i != nodes.Begin())
  {
    --i;
    Ptr<Node> nodePtr = *i;
    Ptr<MobilityModel> mob = nodePtr->GetObject<MobilityModel> (); 
    Vector pos = mob->GetPosition ();
    connectionsCounter = 0.0;
    // COMPARE POSITION AGAINST ALL OTHER NODES
    for (NodeContainer::Iterator j = nodes.Begin (); j != nodes.End (); ++j)
    {
      pos = mob->GetPosition ();
      Ptr<Node> nodePtrTwo = *j;
      Ptr<MobilityModel> mobTwo = nodePtrTwo->GetObject<MobilityModel> ();
      Vector posTwo = mobTwo->GetPosition ();
      // NOT ITERATE ON SAME NODE
      if(nodePtr->GetId() != nodePtrTwo->GetId())
      {
        euclideanDistance = sqrt(abs(pos.x - posTwo.x)*abs(pos.x - posTwo.x) + abs(pos.y - posTwo.y)*abs(pos.y - posTwo.y));
        if((euclideanDistance <= distance) && (euclideanDistance > nodesDeltaSeparation))
        {
          connectionsCounter++;
        } 
        // 3- STEP 3 CHECK THE POSITION IS NOT REPEATED
        if((pos.x == posTwo.x) || (pos.y == posTwo.y))
        {
          //NS_LOG_UNCOND("REPEATED POSITION  ");
          mob->SetPosition(Vector(uv->GetValue(1.0, max_x), uv->GetValue(1.0, max_y), 10.0));
          j = nodes.Begin ();
          connectionsCounter = 0.0;
          continue;
        }
        if( nodePtrTwo->GetId() == (numOfNodes-1))
        { // 4- STEP 4 CHECK NODE CONNECTS AT LEAST WITH SPECIFIED % OF WHOLE GRAPH
          //NS_LOG_UNCOND("Reached end of iterator ");
          if(connectionsCounter < percentageComp)
          {
            mob->SetPosition(Vector(uv->GetValue(1.0, max_x), uv->GetValue(1.0, max_y), 10.0));
            j = nodes.Begin ();
            connectionsCounter = 0.0;
            continue;
          }
        }
      }
    }// end nested for
  } 


  // *********************************** CREATE DEVICES *******************************************************
  NS_LOG_UNCOND ("%INFO: Creating Devices...");
  // Add a non-QoS upper mac, and disable rate control
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  // *********************************** CHANGE MAC HERE ****************************************************** 
  // Set it to EESOA mode
  //wifiMac.SetType ("ns3::EESOA","CtsToSelfSupported", BooleanValue (true),"QosSupported", BooleanValue (false));
  wifiMac.SetType ("ns3::AdhocWifiMac","QosSupported", BooleanValue (false));
  //
  // *********************************** CHANGE MAC HERE ****************************************************** 

  //  DsssRate1Mbps, DsssRate2Mbps, DsssRate5_5Mbps, DsssRate11Mbps
  //  OfdmRate6Mbps, OfdmRate9Mbps, OfdmRate12Mbps, OfdmRate18Mbps
  //  OfdmRate24Mbps, OfdmRate36Mbps, OfdmRate48Mbps , OfdmRate54Mbps
  std::string dataphyMode ("DsssRate11Mbps");
  std::string controlphyMode ("DsssRate11Mbps");

  // disable fragmentation for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  // turn off RTS/CTS for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
  // Fix non-unicast data rate to be the same as that of unicast  
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (dataphyMode));

  wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel; // = YansWifiChannelHelper::Default ();

  // This is parameter that matters when using FixedRssLossModel set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (0) ); 
  //wifiPhy.Set("TxGain", DoubleValue(0));
  
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b
  // Enumeration of PCAP Data Link Types (DLTs) 
  // DLT_IEEE802_11, DLT_PRISM_HEADER, DLT_IEEE802_11_RADIO Include Radiotap link layer information
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 

  //wifiChannel.AddPropagationLoss ("ns3::TwoRayGroundPropagationLossModel","SystemLoss", DoubleValue(14), "HeightAboveZ", DoubleValue(5));
  wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel","MaxRange", DoubleValue (distance));

  // "ns3::RandomPropagationDelayModel" , "ns3::ConstantSpeedPropagationDelayModel"
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");  

  wifiPhy.SetChannel (wifiChannel.Create ());

  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  if (verbose)
  {
    wifi.EnableLogComponents ();  // Turn on all Wifi logging
  }
  // Function receives enum which is mapped according below
  // WIFI_PHY_STANDARD_80211a         -> OFDM PHY for the 5 GHz band (Clause 17)
  // WIFI_PHY_STANDARD_80211b         -> DSSS PHY (Clause 15) and HR/DSSS PHY (Clause 18
  // WIFI_PHY_STANDARD_80211g         -> ERP-OFDM PHY (Clause 19, Section 19.5)
  // WIFI_PHY_STANDARD_80211_10MHZ    -> OFDM PHY for the 5 GHz band (Clause 17 with 10 MHz channel bandwidth)
  // WIFI_PHY_STANDARD_80211_5MHZ     -> OFDM PHY for the 5 GHz band (Clause 17 with 5 MHz channel bandwidth)
  // WIFI_PHY_STANDARD_holland        -> configuration used in a paper: Gavin Holland, Nitin Vaidya and Paramvir 
  //                                     Bahl, "A Rate-Adaptive MAC Protocol for Multi-Hop Wireless Networks", in Proc.
  // WIFI_PHY_STANDARD_80211n_2_4GHZ  -> HT OFDM PHY for the 2.4 GHz band (clause 20)
  // WIFI_PHY_STANDARD_80211n_5GHZ    -> HT OFDM PHY for the 2.4 GHz band (clause 20)
  // WIFI_PHY_STANDARD_80211ac        -> VHT OFDM PHY (clause 22)
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);  

  // "ns3::ConstantRateWifiManager"
  // "ns3::ArfWifiManager"   - Automatic Rate Fallback default in WifiHelper - 
  // "ns3::IdealWifiManager" - Ideal rate control algorithm, similar to RBAR
  // "ns3::CaraWifiManager"  - Collision Aware Rate Adaptation
  // "ns3::AarfWifiManager"	 - Adaptation ARF
  //wifi.SetRemoteStationManager ("ns3::IdealWifiManager", "RtsCtsThreshold", UintegerValue (0), "ProtectionMode",StringValue("Rts-Cts"),"NonUnicastMode", StringValue (dataphyMode)); 
  //wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue (dataphyMode), "ControlMode", StringValue (controlphyMode), "RtsCtsThreshold", UintegerValue (0),"ProtectionMode",StringValue("Rts-Cts"));
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue (dataphyMode), "ControlMode", StringValue (controlphyMode));
  //If the size of the PSDU is bigger than this value, we use an RTS/CTS handshake before sending the data frame.This value will not have any effect on some rate control algorithms.  - RtsCtsThreshold
  // maximum number of packets accepted by this queue  
  Config::SetDefault ("ns3::WifiMacQueue::MaxPacketNumber", UintegerValue(5000));
  // (Number of Retransmissions allowed for an RTS packet)
  // The maximum number of retransmission attempts for an RTS. This value will not have any effect on some rate control algorithms.
  Config::SetDefault ("ns3::WifiRemoteStationManager::MaxSsrc", UintegerValue(10000));
  // (Number of Retransmissions allowed for an RTS packet)
  // The maximum number of retransmission attempts for a DATA packet. This value will not have any effect on some rate control algorithms.
  Config::SetDefault ("ns3::WifiRemoteStationManager::MaxSlrc", UintegerValue(10000));
 
  // Create wifiNetDevices
  devices = wifi.Install (wifiPhy, wifiMac, nodes);

  // ************************************** ENERGY  **************************************************
  NS_LOG_UNCOND ("%INFO: configuring Energy...");

  Time t1 = Seconds (1.0);
  int nodeIndexxx = 0;
  for (uint32_t i = 0; i < numOfNodes; ++i)
  {
    Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
    Ptr<WifiRadioEnergyModel> radioenergyModel = CreateObject<WifiRadioEnergyModel>();
    energySource->SetInitialEnergy (double (100));
    energySource->SetEnergyUpdateInterval (t1);
    radioenergyModel->SetEnergySource (energySource);
    energySource->AppendDeviceEnergyModel (radioenergyModel);
    // IDEAL NODE NO LEAKAGE
    // IdleCurrentA:      The default radio Idle current in Ampere.
    // CcaBusyCurrentA:   The default radio CCA Busy State current in Ampere.
    // TxCurrentA:        The radio Tx current in Ampere.
    // RxCurrentA:        The radio Rx current in Ampere.
    // SwitchingCurrentA: The default radio Channel Switch current in Ampere.
    // SleepCurrentA:     The radio Sleep current in Ampere.
    // TxCurrentModel:    A pointer to the attached tx current model.
    radioenergyModel->SetTxCurrentA (double (0.0174));
    radioenergyModel->SetRxCurrentA (double (0.0174));
    //radioenergyModel->SetIdleCurrentA (double (0.0));
    //radioenergyModel->SetSwitchingCurrentA (double (0.0));
    //radioenergyModel->SetSleepCurrentA (double (0.0));
    //radioenergyModel->SetCcaBusyCurrentA (double (0.0));
    nodes.Get (nodeIndexxx)->AggregateObject (energySource);
    nodeIndexxx++;
  }

  //*******************************INSTALL INTERNET STACK ******************************************
  NS_LOG_UNCOND ("%INFO: Installing internet stack...");
  AodvHelper aodv;
  // hello default is  1 seconds
  Config::SetDefault("ns3::aodv::RoutingProtocol::EnableHello", BooleanValue (aodvHelloEnabled));
  // Time ActiveRouteTimeout;          ///< Period of time during which the route is considered to be valid. default 3 segs
  //Config::SetDefault("ns3::aodv::RoutingProtocol::ActiveRouteTimeout", TimeValue (Seconds(1.0)));
  // DeletePeriod (Time (5 * std::max (ActiveRouteTimeout, HelloInterval)))
  // DeletePeriod", "DeletePeriod is intended to provide an upper bound on the time for which an upstream node A "
  //                 "can have a neighbor B as an active next hop for destination D, while B has invalidated the route to D."
  //                " = 5 * max (HelloInterval, ActiveRouteTimeout)
  //Config::SetDefault("ns3::aodv::RoutingProtocol::DeletePeriod", TimeValue (Seconds(5.0)));
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv); // has effect on the next Install ()
  stack.Install (nodes);
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.0.0.0");
  interfaces = address.Assign (devices);
  // Print Routes
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("aodv.routesAt8Seconds", std::ios::out);
  aodv.PrintRoutingTableAllAt (Seconds (8), routingStream);
  //routingStream = Create<OutputStreamWrapper> ("aodv.routesAt10Seconds", std::ios::out);
  //aodv.PrintRoutingTableAllAt (Seconds (15), routingStream);
  
  AnimationInterface anim ("testEESOA_with_Routing_aug_08_2018.xml");
  /*type[0]= anim.AddResource(path+"eesoaMember100x100pixels.png");
  type[1]= anim.AddResource(path+"eesoaBridge100x100pixels.png");
  type[2]= anim.AddResource(path+"eesoaGateway100x100pixels.png");
  type[3]= anim.AddResource(path+"eesoaLeader100x100pixels.png");*/                    
  /*******************************************************************************************************/

  uint32_t nDevices = devices.GetN ();
  for (uint32_t i = 0;i < nDevices; ++i)
  {
    Ptr<WifiNetDevice> p = DynamicCast<WifiNetDevice> (devices.Get (i)); 
   
    p->SetAddress(Mac48Address::Allocate ());  
    Ptr<AdhocWifiMac> macN = DynamicCast<AdhocWifiMac> (p->GetMac());
    //macN->SetNodeId(p->GetNode ()->GetId ());
    prev_role[i]= 100; //this role does not exists.
    prev_nNodes[i]= 0;
    //Simulator::ScheduleWithContext (p->GetNode ()->GetId (), MilliSeconds(90), &modify, p, &anim);
    Simulator::ScheduleWithContext (p->GetNode ()->GetId (), Seconds(1), &printNonEESOAtx, p);
    p->GetPhy()->TraceConnect ("PhyTxBegin",PrintID(p->GetNode ()), MakeCallback (&PhyTxBegin));
    // energy model
    Ptr<BasicEnergySource>  basicSourcePtr  =  p->GetNode ()->GetObject<BasicEnergySource>();
    NS_ASSERT (basicSourcePtr != NULL);
    basicSourcePtr->TraceConnect ("RemainingEnergy",PrintID(p->GetNode ()), MakeCallback (&RemainingEnergyTrace)); 
    Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
    NS_ASSERT (basicRadioModelPtr != NULL);
    basicRadioModelPtr->TraceConnect ("TotalEnergyConsumption",PrintID(p->GetNode ()), MakeCallback (&TotalEnergy));
  }
  /*******************************************/

  NS_LOG_UNCOND ("%INFO: Install Applications...");
  //************************** INSTALL  APPLICATIONS **********************************************
  
  uint32_t selectedNode;
  uint32_t randomNodeToPing;
  uint32_t packetSize = 256;
  set<uint32_t> selectedNodesSet;
  // SELECT NODES NON REPEATED
  while(selectedNodesSet.size() != numOfNodesTx)
  {
    selectedNode = uv->GetInteger(0,nDevices-1);
    selectedNodesSet.insert(selectedNode);
  }
  //NS_LOG_UNCOND ("%INFO: Nodes for ping selected...");
  // SELECT NODE TO PING AVOIDING NODE PING ITSELF
  std::set<uint32_t>::iterator nodesIt;
  for(nodesIt = selectedNodesSet.begin(); nodesIt != selectedNodesSet.end(); ++nodesIt)
  {
    uint32_t currentNode =  *nodesIt;
    randomNodeToPing = uv->GetInteger(0,nDevices-1);
    while(randomNodeToPing == currentNode)
    {
      randomNodeToPing = uv->GetInteger(0,nDevices-1);
    }
    V4PingHelper ping (interfaces.GetAddress (randomNodeToPing));
    //NS_LOG_UNCOND ("%INFO: Address of node "<<randomNodeToPing<<" is "<<interfaces.GetAddress (randomNodeToPing));
    ping.SetAttribute ("Verbose", BooleanValue (true));
    ping.SetAttribute ("Interval", TimeValue (Seconds (0.27)));
    ping.SetAttribute("Size", UintegerValue (packetSize));
    NS_LOG_UNCOND ("%INFO: Node "<<currentNode<<" ping Node "<<randomNodeToPing);
    ApplicationContainer p = ping.Install (nodes.Get(currentNode));
    p.Start (Seconds (uv->GetValue(pingStarttime,pingStarttime+1)));
    p.Stop (Seconds (simTime) - Seconds (0.001));
  }

  /*******************************************/
  //wifiPhy.EnablePcap ("testEESOA_with_Routing_aug_08_2018", devices);
  // default max num of packets is 100000
  anim.SetMaxPktsPerTraceFile(900000);
  anim.EnablePacketMetadata (); // Optional
  anim.EnableWifiMacCounters (Seconds (0), Seconds (simTime-1)); //Optional
  anim.EnableWifiPhyCounters (Seconds (0), Seconds (simTime-1)); //Optional
  anim.EnableIpv4L3ProtocolCounters (Seconds (0), Seconds (simTime-1)); // Optional
  anim.EnableIpv4RouteTracking ("testEESOA_with_Routing_aug_08_2018_routing.xml",Seconds(0), Seconds(simTime-1), Seconds(1)); 
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
