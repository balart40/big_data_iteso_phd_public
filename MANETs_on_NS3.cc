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
 * Author: Francisco Eduardo Balart Sanchez <balart40@hotmail.com> 
 * This is a extended documentation of the original file manet-2015.cc provided
 * by Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 * At: http://www.uio.no/studier/emner/matnat/ifi/INF5090/h15/timeplan/handouts/
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/stats-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
//#include "ns3/point-to-point-module.h"
#include "ns3/olsr-module.h"
#include "ns3/netanim-module.h"
#include <iostream>

// declares a logging component called febalart_manet_v1 that allows you to enable and 
// disable console message logging by reference to the name
NS_LOG_COMPONENT_DEFINE ("febalart_manet_v1");

// This groups all ns-3-related declarations in a scope outside the global namespace
using namespace ns3;

// Simulation parameters
int nodeSpacing = 100; // meters
int cols = 5;
int numNodes = 25;
int sourceNode = 0;
int destinationNode = 24;
int packetRate = 20; // packet per second
int packetSize = 500; // bytes
bool enablePcap = true;// Generate pcap file containing link-level data 
                       // stored as prefix-nodeid-deviceid.pcap
bool showSimTime = true;
int duration = 600; // seconds
int seed = 1;
int run = 1;

// To show simulated time. Sent to STDERR to separation
// from other output (e.g., visualization trace)
void PrintSeconds(void) {
	std::cerr << Simulator::Now() << std::endl;
	Simulator::Schedule(Seconds(1), &PrintSeconds);
}

void RouteChange(std::string source, uint32_t size) {
	std::cout << "Routechange at " << source << ", new size: " << size << std::endl;
}

int main (int argc, char *argv[])
{
  // Default time resolution is in nano seconds NS but also is available
  // Y, D, H, MIN, S, MS, US, NS, PS, FS
  // year, day, hours, minutes, seconds, miliseconds, nano seconds, pico seconds, femto seconds
  Time::SetResolution (Time::NS);

  // setting verbosity for logs
  //  Level           asssociated macro          enable with
  // LOG_ERROR     -> NS_LOG_ERROR           -> LOG_LEVEL_ERROR
  // LOG_WARN      -> NS_LOG_WARN            -> LOG_LEVEL_WARN
  // LOG_DEBUG     -> NS_LOG_DEBUG           -> LOG_LEVEL_DEBUG
  // LOG_INFO      -> NS_LOG_INFO            -> LOG_LEVEL_INFO
  // LOG_FUNCTION  -> NS_LOG_FUNCTION        -> LOG_LEVEL_FUNCTION
  //                  NS_LOG_FUNCTION_NOARGS                       
  // LOG_LOGIC     -> NS_LOG_LOGIC           -> LOG_LEVEL_LOGIC
  // LOG_ALL       -> no associated macro    -> LOG_LEVEL_ALL
  // EXAMPLE: LOG_LEVEL_INFO will enable msgs provyded by NS_LOG DEBUG, WARN and ERROR macros
  //LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  //LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("febalart_manet_v1", LOG_LEVEL_INFO);

  // Obtain command line arguments
  CommandLine cmd;
  cmd.AddValue ("cols", "Columns of nodes", cols);
  cmd.AddValue ("numnodes", "Number of nodes", numNodes);
  cmd.AddValue ("spacing", "Spacing between neighbouring nodes", nodeSpacing);
  cmd.AddValue ("duration", "Duration of simulation", duration);
  cmd.AddValue ("seed", "Random seed for simulation", seed);
  cmd.AddValue ("run", "Simulation run", run);
  cmd.AddValue ("packetrate", "Packets transmitted per second", packetRate);
  cmd.AddValue ("packetsize", "Packet size", packetSize);
  cmd.AddValue ("sourcenode", "Number of source node", sourceNode);
  cmd.AddValue ("destinationnode", "Number of destination node", destinationNode);
  cmd.AddValue ("showtime", "Whether or not to show simulation as simulation progresses (default = true)", showSimTime);
  cmd.Parse (argc,argv);
  int rows = ((int) numNodes / cols) + 1;

  // Set default parameter values
  // If the size of the PSDU is bigger than this value, we fragment it such that the size of the fragments are equal or smaller
  Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  // If the size of the PSDU is bigger than this value, we use an RTS/CTS handshake before sending the data frame
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));

  // Set random seed and run number
  SeedManager::SetSeed (seed);
  SeedManager::SetRun(run);

  // create instance of nodecontainer called "nodes"
  // originally NodeContainer c 
  NodeContainer nodes;
  // create numNodes nodes
  nodes.Create (numNodes);

  // Set up physical and mac layers
  WifiHelper wifi = WifiHelper::Default ();
  // Function receives enum which is mapped according below
  // WIFI_PHY_STANDARD_80211a         -> OFDM PHY for the 5 GHz band (Clause 17)
  // WIFI_PHY_STANDARD_80211b         -> DSSS PHY (Clause 15) and HR/DSSS PHY (Clause 18
  // WIFI_PHY_STANDARD_80211g         -> ERP-OFDM PHY (Clause 19, Section 19.5)
  // WIFI_PHY_STANDARD_80211_10MHZ    -> OFDM PHY for the 5 GHz band (Clause 17 with 10 MHz channel bandwidth)
  // WIFI_PHY_STANDARD_80211_5MHZ     -> OFDM PHY for the 5 GHz band (Clause 17 with 5 MHz channel bandwidth)
  // WIFI_PHY_STANDARD_holland        -> This is intended to be the configuration used in this paper: Gavin Holland, Nitin Vaidya and Paramvir 
  //                                     Bahl, "A Rate-Adaptive MAC Protocol for Multi-Hop Wireless Networks", in Proc.
  // WIFI_PHY_STANDARD_80211n_2_4GHZ  -> HT OFDM PHY for the 2.4 GHz band (clause 20)
  // WIFI_PHY_STANDARD_80211n_5GHZ    -> HT OFDM PHY for the 2.4 GHz band (clause 20)
  // WIFI_PHY_STANDARD_80211ac        -> VHT OFDM PHY (clause 22)
  wifi.SetStandard (WIFI_PHY_STANDARD_80211g);
  // Also available ns3::MinstrelHtWifiManager, 
  //                ns3::MinstrelWifiManager, 
  //                ns3::AparfWifiManager, 
  //                ns3::ArfWifiManager,    ->  ARF rate control algorithm (Automatic Rate Fallback)
  //                ns3::AarfcdWifiManager, 
  //                ns3::RraaWifiManager, 
  //                ns3::ParfWifiManager,   
  //                ns3::ArfWifiManager, 
  //                ns3::IdealWifiManager, 
  //                ns3::OnoeWifiManager, 
  //                ns3::AmrrWifiManager, 
  //                ns3::CaraWifiManager, 
  //                ns3::AarfWifiManager,  
  //                ns3::ConstantRateWifiManager.
  wifi.SetRemoteStationManager ("ns3::ArfWifiManager");
  // Create non QoS-enabled MAC layer for a ns3::WifiNetDevice
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  // This class can create MACs of type: with QosSupported attribute set to False
  // ns3::ApWifiMac
  // ns3::StaWifiMac
  // ns3::AdhocWifiMac
  wifiMac.SetType ("ns3::AdhocWifiMac");
  // Create and manage PHY objets for the Yans model
  // YAN: Yet Another Network at http://cutebugs.net/files/wns2-yans.pdf
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  // Manage and create wifi channel objects for tha yans model
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  // Create another manager for PHY objects of Yans model called phy equal to the already created wifiphy
  YansWifiPhyHelper phy = wifiPhy;
  // Every PHY created by a call to install is associated with this channel
  // wifichannel create a new channel
  phy.SetChannel (wifiChannel.Create ());
  // Create wifi network with PHY helper for PHY objects, MAC helper for MAC objects and Nodes on which a wifi device will be created
  // install used is the one that use c nodes 
  NetDeviceContainer devices = wifi.Install (phy, wifiMac, nodes);

  if(enablePcap)
	  wifiPhy.EnablePcap ("febalart_manet_v1", devices);

  // Set up routing and Internet stack
  // Also available: OlsrHelper, AodvHelper,  
  ns3::OlsrHelper olsr;
  // aggregate IP/TCP/UDP functionality to existing Nodes
  InternetStackHelper internet;
  // routing helper during install, create object of tyep  ns3::Ipv4RoutingProtocol per Node
  internet.SetRoutingHelper(olsr);
  // for each node the input container aggregate implementatoin  of ns3::Ipv4,Ipv6,Udp,Tcp classes
  internet.Install (nodes);

  // Assign addresses
  Ipv4AddressHelper address;
  //               Ipv4Adress network, Ipv4Mask Maskm Ipv4Address base = 0.0.0.1
  address.SetBase ("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // Server/Receiver
  // create udpserverhelper number is supposed to be the port the server will wait on for incomming packets
  UdpServerHelper server (4000);
  ApplicationContainer apps = server.Install (nodes.Get(destinationNode));
  // start at second 1
  apps.Start (Seconds (1));
  // stop at the duration specified
  apps.Stop (Seconds (duration - 1));

  // Client/Sender
  UdpClientHelper client (interfaces.GetAddress (destinationNode), 4000);
  client.SetAttribute ("MaxPackets", UintegerValue (100000000));
  // interval of packets sent
  client.SetAttribute ("Interval", TimeValue (Seconds(1 / ((double) packetRate))));
  client.SetAttribute ("PacketSize", UintegerValue (packetSize));
  apps = client.Install (nodes.Get (sourceNode));
  apps.Start (Seconds (1));
  apps.Stop (Seconds (duration - 1));

  // Set up mobility
  MobilityHelper mobility;
  // There is also available ns3::GridPositionAllocator, ns3::UniformDiscPositionAllocator, ns3::RandomDiscPositionAllocator,
  // ns3::RandomBoxPositionAllocator, ns3::RandomRectanglePositionAllocator, ns3::FixedRoomPositionAllocator, ns3::GridPositionAllocator,
  // ns3::SameRoomPositionAllocator, ns3::RandomRoomPositionAllocator, ns3::ListPositionAllocator, and ns3::RandomBuildingPositionAllocator.
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator","MinX", DoubleValue (1.0),"MinY", DoubleValue (1.0),
                                 "DeltaX", DoubleValue(nodeSpacing),"DeltaY", DoubleValue (nodeSpacing),"GridWidth", UintegerValue (cols));
 
  // There is also vailable ns3: ConstantAccelerationMobilityModel ConstantPositionMobilityModel ConstantVelocityHelper
  // ConstantVelocityMobilityModel GaussMarkovMobilityModel GeographicPositions GridPositionAllocator HierarchicalMobilityModel
  // ListPositionAllocator MobilityHelper MobilityModel Ns2MobilityHelper
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel","Bounds", 
                            RectangleValue (Rectangle (0,(cols * nodeSpacing) + 1, 0,(rows * nodeSpacing) + 1)),
		                        "Speed", StringValue("ns3::UniformRandomVariable[Min=5.0,Max=10.0]"),
		                        "Distance", DoubleValue(30));

  mobility.Install (nodes);

  // Schedule final events and start simulation

  // Print simulated time
  if(showSimTime)
    Simulator::Schedule(Seconds(1), &PrintSeconds);


  Simulator::Stop(Seconds(duration));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;


}
