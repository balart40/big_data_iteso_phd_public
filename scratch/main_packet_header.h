/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
// modified by Francisco Eduardo Balart Sanchez balart40@gmail.com
// Original source from: src/network/examples/main-packet-header.cc
#ifndef ESSOAPACKET_H
#define ESSOAPACKET_H

#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/enum.h"
#include "ns3/header.h"
#include <iostream>

namespace ns3{

enum MessageType
{
  ESSOATYPE_RREQ     = 1,  // <- ESSOA_RREQ
  ESSOATYPE_RREP     = 2,  // <- ESSOA_RREP
  ESSOATYPE_RERR     = 3,  // <- ESSOA_RERR
  ESSOATYPE_RREP_ACK = 4   // <- ESSOA_RREP_ACK
};

enum ESSOA_MSG_TYPE
{
  ESSOA_RRESPB,   // ESSOA REQUEST RESPONSE TO BROADCAST PACKET
  ESSOA_RRESPANS, // ESSOA ANSWER PACKET WITH REQUEST RESPONSE
  ESSOA_NRESP     // ESSOA NO RESPONSE PACKET
};

/* A sample Header implementation */
class HelloHeader : public Header 
{
public:

  HelloHeader ();
  virtual ~HelloHeader ();

  void SetSourceNodeId (uint16_t srcNodeId);
  void SetDestinationNodeId (uint16_t destNodeId);
  void SetSentHandShakeUid (uint16_t Uid);
  void SetReceivedHandShakeUid (uint16_t Uid);
  uint16_t GetSourceNodeId () const;
  uint16_t GetDestinationNodeId () const;
  uint16_t GetSentHandShakeUid () const;
  uint16_t GetReceivedHandShakeUid () const;

  // Header Serialization / De-serialization
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Print (std::ostream &os) const;

private:
  // 2 bytes * 4 datas  => 8 bytes
  uint16_t m_sourceNodeId;
  uint16_t m_destinationNodeId;
  uint16_t m_SentHandShakeUid;
  uint16_t m_ReceivedHandShakeUid;
};

HelloHeader::HelloHeader ()
{
  // we must provide a public default constructor, 
  // implicit or explicit, but never private.
}
HelloHeader::~HelloHeader ()
{
}

TypeId HelloHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::HelloHeader")
    .SetParent<Header> ()
    .AddConstructor<HelloHeader> ();
  return tid;
}

TypeId HelloHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void HelloHeader::Print (std::ostream &os) const
{
  // This method is invoked by the packet printing
  // routines to print the content of my header.
  //os << "data=" << m_data << std::endl;
  os << "data: \n" <<"source id: "<< m_sourceNodeId << "\n"<<"Destination Id: "<< m_destinationNodeId<<"\n"<<"Sent Uid: "<<m_SentHandShakeUid<<"\n"<<"Received Id: "<<m_ReceivedHandShakeUid;
}

uint32_t HelloHeader::GetSerializedSize (void) const
{
  // we reserve 2 bytes for our header.
  return 8;
}

void HelloHeader::Serialize (Buffer::Iterator start) const
{
  // we can serialize two bytes at the start of the buffer.
  // we write them in network byte order.
  start.WriteHtonU16 (m_sourceNodeId);
  start.WriteHtonU16 (m_destinationNodeId);
  start.WriteHtonU16 (m_SentHandShakeUid);
  start.WriteHtonU16 (m_ReceivedHandShakeUid);
}

uint32_t HelloHeader::Deserialize (Buffer::Iterator start)
{
  // we can deserialize two bytes from the start of the buffer.
  // we read them in network byte order and store them
  // in host byte order.
  m_sourceNodeId = start.ReadNtohU16 ();
  m_destinationNodeId = start.ReadNtohU16 ();
  m_SentHandShakeUid = start.ReadNtohU16 ();
  m_ReceivedHandShakeUid = start.ReadNtohU16 ();
  // we return the number of bytes effectively read.
  return 8;
}

void HelloHeader::SetSourceNodeId (uint16_t srcNodeId) 
{
  m_sourceNodeId = srcNodeId;
}

void HelloHeader::SetDestinationNodeId (uint16_t destNodeId) 
{
  m_destinationNodeId = destNodeId;
}

void HelloHeader::SetSentHandShakeUid (uint16_t Uid) 
{
  m_SentHandShakeUid = Uid;
}

void HelloHeader::SetReceivedHandShakeUid (uint16_t Uid) 
{
  m_ReceivedHandShakeUid= Uid;
}

uint16_t HelloHeader::GetSourceNodeId () const
{
  return m_sourceNodeId;
}

uint16_t HelloHeader::GetDestinationNodeId () const
{
  return m_destinationNodeId;
}

uint16_t HelloHeader::GetSentHandShakeUid () const
{
  return m_SentHandShakeUid;
}

uint16_t HelloHeader::GetReceivedHandShakeUid () const
{
  return m_ReceivedHandShakeUid;
}

}// end bracket namesapce ns3

// %INFO: We removed the main function original source is at: src/network/examples/main-packet-header.cc
#endif 
