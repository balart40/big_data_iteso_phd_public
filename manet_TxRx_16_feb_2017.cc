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
 * Author: Francisco Eduardo Balart Sanchez <balart40@hotmail.com 
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
// Added for the energy model
#include "ns3/basic-energy-source.h"
#include "ns3/wifi-radio-energy-model.h"
#include "ns3/basic-energy-source-helper.h"
#include "ns3/wifi-radio-energy-model-helper.h"
#include "ns3/energy-source-container.h"
#include "ns3/device-energy-model-container.h"
#include "ns3/energy-module.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
// include header we will use for the packets header & payload
#include "main_packet_header.h"

// DEfine logging component
NS_LOG_COMPONENT_DEFINE ("WifiSimpleAdhoc");

using namespace ns3;
//using namespace ESSOA;

void SetPosition (Ptr<Node> node, Vector position)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  mobility->SetPosition (position);
}
 
Vector GetPosition (Ptr<Node> node)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  return mobility->GetPosition ();
}

struct neighbor
{
    int nodeId;
    int timeRemaining;
};

typedef std::vector<neighbor> nodeIdNeighbors;
std::vector<nodeIdNeighbors> networkNodeIdNeighbors;

std::vector<neighbor> checkAddNeighbor(int nodeId, std::vector<neighbor> neighborsOfNodeId)
{
  bool foundInNeighbors = false;
  if (neighborsOfNodeId.size()>0)
  {
    for(std::vector<int>::size_type i = 0; i != neighborsOfNodeId.size(); i++)
    {
      if (nodeId == neighborsOfNodeId[i].nodeId)
      { 
        foundInNeighbors = true;
        neighborsOfNodeId[i].timeRemaining = 4;
      }
    }
  }
  if ((foundInNeighbors == false) or ((neighborsOfNodeId.size()==0) and nodeId))
  {
    neighbor tempNeighbor;
    tempNeighbor.nodeId = nodeId;
    tempNeighbor.timeRemaining = 4;
    neighborsOfNodeId.push_back(tempNeighbor);
  }
return neighborsOfNodeId;
}

std::vector<neighbor> decrementUpdateTimeNeighbors(std::vector<neighbor> nodeIdNeighborsvector)
{
  NS_LOG_UNCOND ("%INFO: Decrementing timers");
  if (nodeIdNeighborsvector.size()>0)
  {
    for(std::vector<int>::size_type i = 0; i != nodeIdNeighborsvector.size(); i++)
    {
      nodeIdNeighborsvector[i].timeRemaining-=1;
    }
    // we check if the node id time is up to erase it from the neighbor table
    std::vector<int> indexes;
    for(std::vector<int>::size_type i = 0; i != nodeIdNeighborsvector.size(); i++)
    {
      if (nodeIdNeighborsvector[i].timeRemaining<=0)
      {
        indexes.push_back(i);
      }
    }
    for(std::vector<int>::size_type i = 0; i != indexes.size(); i++)
    { 
      indexes[i]-=i;
    }
    for(std::vector<int>::size_type i = 0; i != indexes.size(); i++)
    { 
      nodeIdNeighborsvector.erase(nodeIdNeighborsvector.begin()+indexes[i]);
    }
  } 
  else
  {
  }
return nodeIdNeighborsvector;
}

void printNeighborsOfZero(std::vector<neighbor> nodeIdNeighborsvector, int nodeId)
{
  NS_LOG_UNCOND ("*******************************************************************************************************************");
  NS_LOG_UNCOND ("%INFO: Node Id: "<<nodeId);
  NS_LOG_UNCOND ("%INFO: Printing "<<nodeIdNeighborsvector.size()<<" neighbor(s)");
  for(std::vector<int>::size_type i = 0; i != nodeIdNeighborsvector.size(); i++)
  {
    NS_LOG_UNCOND ("%INFO: Neighbor Node id: "<<nodeIdNeighborsvector[i].nodeId<<" timeremaining: "<<nodeIdNeighborsvector[i].timeRemaining);
  }
}

static void GenerateHelloBroadcast(Ptr<WifiNetDevice> wifinetdevice, uint32_t pktSize, Time pktInterval, uint32_t numPackets)
{
  if (numPackets>0)
  {
    NS_LOG_UNCOND ("*******************************************************************************************************************");
    NS_LOG_UNCOND ("%INFO: Start of Recursive broadcast traffic "<<numPackets<<" PACKETS REMAINING");
    NS_LOG_UNCOND ("%INFO: Sending packet! I am node "<<wifinetdevice->GetNode ()->GetId ()<<" my MAC is: "<<wifinetdevice->GetAddress());
    // Create packet
    Packet::EnablePrinting ();
    Ptr<Packet> packet = Create<Packet> (pktSize);
    // Preparing header for first packet from node
    HelloHeader helloheaderToSend;
    helloheaderToSend.SetSourceNodeId(wifinetdevice->GetNode ()->GetId ());
    helloheaderToSend.SetDestinationNodeId(255);
    helloheaderToSend.SetSentHandShakeUid(ESSOA_RRESPB);
    helloheaderToSend.SetReceivedHandShakeUid(0);
    packet->AddHeader (helloheaderToSend);
    // the last argument in the send function is the protocol number we will use 1
    // http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
    // packet created
    static Mac48Address broadcast = Mac48Address ("ff:ff:ff:ff:ff:ff");
    wifinetdevice->Send (packet, broadcast,1);
    NS_LOG_UNCOND ("%INFO: Packet sent Generate traffic call done");
    networkNodeIdNeighbors[wifinetdevice->GetNode ()->GetId ()] = decrementUpdateTimeNeighbors(networkNodeIdNeighbors[wifinetdevice->GetNode ()->GetId ()]);
    printNeighborsOfZero(networkNodeIdNeighbors[wifinetdevice->GetNode ()->GetId ()],wifinetdevice->GetNode ()->GetId ());
    Simulator::ScheduleWithContext(wifinetdevice->GetNode ()->GetId (),pktInterval, &GenerateHelloBroadcast, wifinetdevice, pktSize, pktInterval,numPackets-1);
  }
}

static void GenerateTraffic (Ptr<WifiNetDevice> wifinetdevice, uint32_t pktSize, Time pktInterval, HelloHeader helloheaderToSend, const Address& destAddress)
{
  HelloHeader tempheader;
  tempheader = helloheaderToSend;
  NS_LOG_UNCOND ("*******************************************************************************************************************");
  NS_LOG_UNCOND ("%INFO: Sending packet! I am node "<<wifinetdevice->GetNode ()->GetId ()<<" my MAC is: "<<wifinetdevice->GetAddress());
  NS_LOG_UNCOND ("%INFO: source address: "<<destAddress<< " to node Id: "<<tempheader.GetDestinationNodeId());
  // Create packet
  Ptr<Packet> packet = Create<Packet> (pktSize);
  // Add header to packet
  packet->AddHeader (helloheaderToSend);
  //static Mac48Address broadcast = Mac48Address ("ff:ff:ff:ff:ff:ff");
  // the last argument in the send function is the protocol number we will use 1
  // http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
  // packet created
  wifinetdevice->Send (packet, destAddress,1);
  NS_LOG_UNCOND ("%INFO: Packet sent Generate traffic call done");
}

void ReceivePacketWithRss (Ptr< const Packet > packet, uint16_t channelFreqMhz, uint16_t channelNumber, uint32_t rate, WifiPreamble preamble,WifiTxVector txVector, struct mpduInfo aMpdu, struct signalNoiseDbm signalNoise)
{
  // callback to get the rss received signal strength using MonitorSnifferRx callback from wifiphy
  NS_LOG_UNCOND ("*******************************************************************************************************************");
  NS_LOG_UNCOND ("%INFO: Received one packet! With "<<signalNoise.signal<<" dbm");
}

bool ReceivePacket( Ptr<NetDevice> netdevice, Ptr<const Packet> packet, uint16_t protocol, const Address& sourceAddress)
{
  Packet::EnablePrinting ();
  Ptr<WifiNetDevice> wifinetdevice = DynamicCast<WifiNetDevice> (netdevice);
  uint32_t pktSize = 10;
  NS_LOG_UNCOND ("*******************************************************************************************************************");
  NS_LOG_UNCOND ("%INFO: Rx packet for Node: "<<wifinetdevice->GetNode ()->GetId ()<<" my MAC: "<<wifinetdevice->GetAddress()<<" RX packet from MAC: "<< sourceAddress);
  // Reading Header
  HelloHeader headerRx;
  packet->PeekHeader(headerRx);
  // Building new Header
  HelloHeader header;
  double r =1.0+ ((double) rand() / (RAND_MAX));
  // This part of the header is the same regardless the type of message
  header.SetSourceNodeId(wifinetdevice->GetNode ()->GetId ());
  header.SetDestinationNodeId(headerRx.GetSourceNodeId());
  // if message is broadcast
  if (headerRx.GetSentHandShakeUid() == ESSOA_RRESPB) 
  {  
    header.SetSentHandShakeUid(ESSOA_RRESPANS);
    header.SetReceivedHandShakeUid(ESSOA_RRESPB);
    // Reading header to send
    HelloHeader tempHeader = header;
    NS_LOG_UNCOND ("*******************************************************************************************************************");
    NS_LOG_UNCOND ("%INFO: Tx Packet Info, Source Node Id: " <<tempHeader.GetSourceNodeId());
    NS_LOG_UNCOND ("%INFO: Tx Packet Info, Destination Node Id: " <<tempHeader.GetDestinationNodeId());
    NS_LOG_UNCOND ("%INFO: Tx Packet Info, Sent Unique ID: " <<tempHeader.GetSentHandShakeUid());
    NS_LOG_UNCOND ("%INFO: Tx Packet Info, Rx Unique ID: " <<tempHeader.GetReceivedHandShakeUid());
    // *** PROTOCOL NUMBER IS MAPPED TO A SPECIFIC L3 PAYLOAD FORMAT SEE LINK BELOW
    //http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
    NS_LOG_UNCOND ("%INFO: sending packet response due to callback of BROADCAST REQUEST ANSWER with protocol: " << protocol);
    Simulator::Schedule (Seconds (r), &GenerateTraffic, wifinetdevice, pktSize, Seconds(r),header,sourceAddress);
  }
  else if (headerRx.GetDestinationNodeId() ==  wifinetdevice->GetNode ()->GetId ())
  {
    // IF WE GET A RESPONSE WHICH NOT REQUIRE A RESPONSE FROM OUR PART
    if (headerRx.GetSentHandShakeUid() == ESSOA_NRESP)
    {
      // Do nothing, future this will be for ack of neighbors
      NS_LOG_UNCOND ("%INFO: DO NOTHING");
      return true;
    }
    else if ((headerRx.GetSentHandShakeUid() == ESSOA_RRESPANS) and (headerRx.GetReceivedHandShakeUid() == ESSOA_RRESPB))
    {
      header.SetSentHandShakeUid(ESSOA_NRESP);
      header.SetReceivedHandShakeUid(ESSOA_RRESPB);
      networkNodeIdNeighbors[wifinetdevice->GetNode ()->GetId ()] = checkAddNeighbor(headerRx.GetSourceNodeId(), networkNodeIdNeighbors[wifinetdevice->GetNode ()->GetId ()]);
      // Reading header to send
      HelloHeader tempHeader = header;
      NS_LOG_UNCOND ("*******************************************************************************************************************");
      NS_LOG_UNCOND ("%INFO: Tx Packet Info, Source Node Id: " <<tempHeader.GetSourceNodeId());
      NS_LOG_UNCOND ("%INFO: Tx Packet Info, Destination Node Id: " <<tempHeader.GetDestinationNodeId());
      NS_LOG_UNCOND ("%INFO: Tx Packet Info, Sent Unique ID: " <<tempHeader.GetSentHandShakeUid());
      NS_LOG_UNCOND ("%INFO: Tx Packet Info, Rx Unique ID: " <<tempHeader.GetReceivedHandShakeUid());
      // *** PROTOCOL NUMBER IS MAPPED TO A SPECIFIC L3 PAYLOAD FORMAT SEE LINK BELOW
      //http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
      NS_LOG_UNCOND ("%INFO: sending packet response due to callback RESPONSE TO RESPONSE of BROADCAST NON REQUEST RESP with protocol: " << protocol);
      Simulator::Schedule (Seconds (r), &GenerateTraffic, wifinetdevice, pktSize, Seconds(r),header,sourceAddress);
    }
  }
  return true;
}

int main (int argc, char *argv[])
{
  int      numOfNodes = 3;  // number of nodes
  uint32_t packetSize = 10; // packet size in bytes 
  uint32_t numPackets = 10; // number of packets of hello world to send
  double interval = 1.0;    // seconds
  double stopTime = 30.0;
  std::srand(time(0));

  CommandLine cmd;
  cmd.Parse (argc, argv);

  // Convert to time object
  Time interPacketInterval = Seconds (interval);
  Time stopTimeSimulation = Seconds (stopTime);
  Simulator::Stop (stopTimeSimulation);

  // Enable verbosity for debug which includes
  // NS_LOG_DEBUG_, NS_LOG_WARN and LOG_ERROR
  LogComponentEnable ("WifiSimpleAdhoc", LOG_LEVEL_INFO);
 
  // Message to terminal console for debug
  NS_LOG_UNCOND ("%INFO: Starting Test now...");

  // Create complete empty "hulls" nodes
  NS_LOG_UNCOND ("%INFO: Creating Nodes...");
  NodeContainer c;
  // creating nodes
  c.Create(numOfNodes);
  networkNodeIdNeighbors.resize(numOfNodes);
  NS_LOG_UNCOND ("%INFO: Size of vector of vectors "<<networkNodeIdNeighbors.size());

  NS_LOG_UNCOND ("%INFO: Configuring PHY Loss model and connecting to PHY...");
  // Create PHY and Channel
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel;
  NS_LOG_UNCOND ("%INFO: Configuring PHY Loss model...");
  //Config::SetDefault ("ns3::RangePropagationLossModel::MaxRange", DoubleValue (10));
  //wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel");
  wifiChannel.AddPropagationLoss ("ns3::TwoRayGroundPropagationLossModel","SystemLoss", DoubleValue(10), "HeightAboveZ", DoubleValue(5));
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
   
  // Connect PHY with the Channel
  NS_LOG_UNCOND ("%INFO: Connecting PHY with Channel...");
  wifiPhy.SetChannel (wifiChannel.Create ());
  wifiPhy.Set("TxGain", DoubleValue(-1.0));
  wifiPhy.Set("RxGain", DoubleValue(-10.0));
  
  NS_LOG_UNCOND ("%INFO: Configuring PHY STD and RSM...");
  // Create WifiHelper to be able to setup the PHY
  WifiHelper wifi;// = WifiHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  //wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode","OfdmRate12Mbps","ControlMode","OfdmRate12Mbps");
  // Later test: ns3::IdealWifiManager
  wifi.SetRemoteStationManager ("ns3::ArfWifiManager");
  // Control the Rate via Remote Station Manager

  NS_LOG_UNCOND ("%INFO: Configuring MAC...");
  // MAC layer configuration
  WifiMacHelper wifiMac;
  // Setting the type with Adhoc we will wrap and inherit the RegularMac and WifiMac classes properties
  wifiMac.SetType ("ns3::AdhocWifiMac","QosSupported", BooleanValue (false));

  // Make a device nodes with phy, mac and nodes already configured
  // Below will create WifiNetDevices ***********************************************
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, c);

  NS_LOG_UNCOND ("%INFO: configuring mobility.");
  // Configure mobility
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  double randomNumX;
  double randomNumY;
  uint32_t nDevices = devices.GetN ();
  for (uint32_t i = 0;i < nDevices; ++i)
  {
    // X and Y random position
    randomNumX = rand() % 51 + (-50);
    randomNumY = rand() % 101 + (-100);
    positionAlloc->Add (Vector (randomNumX, randomNumY, 5.0));
  }
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds", RectangleValue (Rectangle (-500, 500, -500, 500)));
  mobility.Install (c);

  // ENERGY
  // There are avaiable also: 
  //  Basic Energy Source Helper -> BasicEnergySourceHelper
  //  Rv Battert Model Helper    -> RvBatteryModelHelper
  //  LiIon Energy Source Helper -> LiIonEnergySourceHelper
  // All the aforementioned inherit from EnergySourceHelper
  BasicEnergySourceHelper basicSourceHelper;
  // This one will associate the source energy as follows
  // The other methods will do the same accordingly
  // m_basicEnergySource.SetTypeId ("ns3::BasicEnergySource");
  // m_liIonEnergySource.SetTypeId ("ns3::LiIonEnergySource");
  // m_rvBatteryModel.SetTypeId ("ns3::RvBatteryModel");
  // Which all inherit from ns3:EnergySource
  // *** Configure the energy source  ***
  // BasicEnergySourceInitialEnergyJ: Initial energy stored in basic energy source. in joules
  // BasicEnergySupplyVoltageV:       Initial supply voltage for basic energy source.
  // PeriodicEnergyUpdateInterval:    Time between two consecutive periodic energy updates.
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (0.1));
  // install source
  EnergySourceContainer sources = basicSourceHelper.Install (c);
  // device energy model 
  // Now every time the wifi net device transmits the energy source of that device->node will be notified via callback
  WifiRadioEnergyModelHelper WifiradioEnergyHelper;
  // **** configure radio energy model*****
  // IdleCurrentA:      The default radio Idle current in Ampere.
  // CcaBusyCurrentA:   The default radio CCA Busy State current in Ampere.
  // TxCurrentA:        The radio Tx current in Ampere.
  // RxCurrentA:        The radio Rx current in Ampere.
  // SwitchingCurrentA: The default radio Channel Switch current in Ampere.
  // SleepCurrentA:     The radio Sleep current in Ampere.
  // TxCurrentModel:    A pointer to the attached tx current model.
  WifiradioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174));
  // install device model
  DeviceEnergyModelContainer deviceModels = WifiradioEnergyHelper.Install (devices, sources);


  NS_LOG_UNCOND ("%INFO: Assign Mac48Address Addresses & Generating Traffic hello world broadcast");
  for (uint32_t i = 0;i < nDevices; ++i)
  {
    Ptr<WifiNetDevice> wifiNetDevicePointer = DynamicCast<WifiNetDevice> (devices.Get (i)); 
    wifiNetDevicePointer->SetAddress(Mac48Address::Allocate ());  
    devices.Get(i)->SetReceiveCallback(MakeCallback(&ReceivePacket));
    wifiNetDevicePointer->GetPhy()->TraceConnectWithoutContext ("MonitorSnifferRx", MakeCallback (&ReceivePacketWithRss));
    // This is temporal since at end Nodes will have its own broadcast function below can lead to dead lock, need to see Mutual Exlusion features
    double r =1.0+ ((double) rand() / (RAND_MAX));
    Simulator::ScheduleWithContext (wifiNetDevicePointer->GetNode ()->GetId (),Seconds (r), &GenerateHelloBroadcast, wifiNetDevicePointer, packetSize, interPacketInterval, numPackets);
  }

  // enable packet capture tracing and xml
  wifiPhy.EnablePcap ("WifiSimpleAdhoc", devices);
  AnimationInterface anim ("WifiSimpleAdhoc.xml");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}//END of Main function
