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
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/stats-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/olsr-module.h"
#include "ns3/netanim-module.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// DEfine logging component
NS_LOG_COMPONENT_DEFINE ("WifiSimpleAdhoc");

using namespace ns3;

static void GenerateTraffic (Ptr<WifiNetDevice> wifinetdevice, uint32_t pktSize, Time pktInterval )
{
  NS_LOG_UNCOND ("%INFO: Sending packet! I am node "<<wifinetdevice->GetNode ()->GetId ()<<" my MAC is: "<<wifinetdevice->GetAddress());
  static Mac48Address broadcast = Mac48Address ("ff:ff:ff:ff:ff:ff");
  wifinetdevice->Send (Create<Packet> (pktSize), broadcast,1);
  //Simulator::Schedule (pktInterval, &GenerateTraffic, socket, pktSize, pktInterval);
}

void ReceivePacket (Ptr<WifiNetDevice> wifinetdevice)
{
  uint32_t pktSize = 1000;
  NS_LOG_UNCOND ("%INFO: Received one packet! I am node "<<wifinetdevice->GetNode ()->GetId ()<<" my MAC is: "<<wifinetdevice->GetAddress());
  static Mac48Address broadcast = Mac48Address ("ff:ff:ff:ff:ff:ff");
  wifinetdevice->Send (Create<Packet> (pktSize), broadcast, 1);
  //Simulator::Schedule (Seconds (1.0), &GenerateTraffic, 
    //                       socket, pktSize,1, Seconds (1.0));

}

int main (int argc, char *argv[])
{
  uint32_t packetSize = 1000; // bytes
  //uint32_t numPackets = 1;
  double interval = 1.0; // seconds
  double rss = -80;  // -dBm

  // Convert to time object
  Time interPacketInterval = Seconds (interval);

  // Enable verbosity for debug which includes
  // NS_LOG_DEBUG_, NS_LOG_WARN and LOG_ERROR
  LogComponentEnable ("WifiSimpleAdhoc", LOG_LEVEL_INFO);
 
  // Message to terminal console for debug
  NS_LOG_UNCOND ("%INFO: Starting Test now...");

  // Create complete empty "hulls" nodes
  NS_LOG_UNCOND ("%INFO: Creating Nodes...");
  NodeContainer c;
  // creating 2 nodes
  c.Create(2);

  NS_LOG_UNCOND ("%INFO: Configuring PHY STD and RSM...");
  // Create WifiHelper to be able to setup the PHY
  WifiHelper wifi;// = WifiHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211g);
  //wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode","OfdmRate12Mbps","ControlMode","OfdmRate12Mbps");
  // Later test: ns3::IdealWifiManager
  wifi.SetRemoteStationManager ("ns3::ArfWifiManager");
  // Control the Rate via Remote Station Manager
  

  NS_LOG_UNCOND ("%INFO: Configuring PHY Loss model and connecting to PHY...");
  // Create PHY and Channel
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel;
  NS_LOG_UNCOND ("%INFO: Configuring PHY Loss model...");
  //wifiChannel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel","Exponent", DoubleValue (3.0));
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FixedRssLossModel","Rss",DoubleValue (rss));  
  // Connect PHY with the Channel
  NS_LOG_UNCOND ("%INFO: Connecting PHY with Channel...");
  wifiPhy.SetChannel (wifiChannel.Create ());
  
  NS_LOG_UNCOND ("%INFO: Configuring MAC...");
  // MAC layer configuration
  WifiMacHelper wifiMac;
  // Setting the type with Adhoc we will wrap and inherit the RegularMac and WifiMac classes properties
  wifiMac.SetType ("ns3::AdhocWifiMac","QosSupported", BooleanValue (false));

  // Make a device nodes with phy, mac and nodes already configured
  // Below will create WifiNetDevices ***********************************************
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, c);
  // enable packet capture tracing
  wifiPhy.EnablePcap ("WifiSimpleAdhoc", devices);

  NS_LOG_UNCOND ("%INFO: Assign Mac48Address Addresses.");
  //devices->SetAddress(Mac48Address::Allocate ());
  uint32_t nDevices = devices.GetN ();
  for (uint32_t i = 0;i < nDevices; ++i)
  {
    Ptr<WifiNetDevice> p = DynamicCast<WifiNetDevice> (devices.Get (i)); 
    //Ptr<WifiNetDevice> p = devices.Get (i);
    p->SetAddress(Mac48Address::Allocate ());  
  }

  NS_LOG_UNCOND ("%INFO: configuring mobility.");
  // Configure mobility
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (5.0, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c);

  Ptr<WifiNetDevice> wifinetdeviceA = DynamicCast<WifiNetDevice> (devices.Get(0));
  Ptr<WifiNetDevice> wifinetdeviceB = DynamicCast<WifiNetDevice> (devices.Get(1));
  //wifinetdeviceA->SetReceiveCallback (MakeCallback (&ReceivePacket));
  //Ptr<NetDevice> netdeviceA = devices.Get(0);
  //netdeviceA->SetReceiveCallback (MakeCallback (&ReceivePacket));
  //wifinetdeviceB->SetReceiveCallback (MakeCallback (&ReceivePacket));
  //wifinetdeviceA->SetReceiveCallback (NetDevice::ReceiveCallback (&ReceivePacket));


  NS_LOG_UNCOND ("%INFO: Generate traffic.");
  Simulator::ScheduleWithContext (wifinetdeviceA->GetNode ()->GetId (),Seconds (1.0), &GenerateTraffic, wifinetdeviceA, packetSize, interPacketInterval);

  AnimationInterface anim ("WifiSimpleAdhoc.xml");

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;

}//END of Main function
