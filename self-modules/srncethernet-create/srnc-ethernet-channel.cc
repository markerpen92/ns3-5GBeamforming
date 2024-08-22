/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007, 2008 University of Washington
 *
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
 */

// #include "point-to-point-channel.h"
// #include "point-to-point-net-device.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/srnc-ethernet-device.h"
#include "ns3/srnc-ethernet-channel.h"
#include "ns3/srnc-ethernet-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ns3_ClassSRNC03_PointToPointChannel");

NS_OBJECT_ENSURE_REGISTERED (ns3_ClassSRNC03_PointToPointChannel);

TypeId 
ns3_ClassSRNC03_PointToPointChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ns3_ClassSRNC03_PointToPointChannel")
    .SetParent<Channel> ()
    .SetGroupName ("PointToPoint")
    .AddConstructor<ns3_ClassSRNC03_PointToPointChannel> ()
    .AddAttribute ("Delay", "Propagation delay through the channel",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&ns3_ClassSRNC03_PointToPointChannel::m_delay),
                   MakeTimeChecker ())
    .AddTraceSource ("TxRxPointToPoint",
                     "Trace source indicating transmission of packet "
                     "from the ns3_ClassSRNC03_PointToPointChannel, used by the Animation "
                     "interface.",
                     MakeTraceSourceAccessor (&ns3_ClassSRNC03_PointToPointChannel::m_txrxPointToPoint),
                     "ns3::ns3_ClassSRNC03_PointToPointChannel::TxRxAnimationCallback")
  ;
  return tid;
}

//
// By default, you get a channel that 
// has an "infitely" fast transmission speed and zero delay.
ns3_ClassSRNC03_PointToPointChannel::ns3_ClassSRNC03_PointToPointChannel()
  :
    Channel (),
    m_delay (Seconds (0.)),
    m_nDevices (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
ns3_ClassSRNC03_PointToPointChannel::Attach (Ptr<ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet> device)
{
  NS_LOG_FUNCTION (this << device);
  NS_ASSERT_MSG (m_nDevices < N_DEVICES, "Only two devices permitted");
  NS_ASSERT (device != 0);

  m_link[m_nDevices++].m_src = device;
//
// If we have both devices connected to the channel, then finish introducing
// the two halves and set the links to IDLE.
//
  if (m_nDevices == N_DEVICES)
    {
      m_link[0].m_dst = m_link[1].m_src;
      m_link[1].m_dst = m_link[0].m_src;
      m_link[0].m_state = IDLE;
      m_link[1].m_state = IDLE;
    }
}

bool
ns3_ClassSRNC03_PointToPointChannel::TransmitStart (
  Ptr<const Packet> p,
  Ptr<ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet> src,
  Time txTime)
{
  NS_LOG_FUNCTION (this << p << src);
  NS_LOG_LOGIC ("UID is " << p->GetUid () << ")");

  NS_ASSERT (m_link[0].m_state != INITIALIZING);
  NS_ASSERT (m_link[1].m_state != INITIALIZING);

  uint32_t wire = src == m_link[0].m_src ? 0 : 1;

  Simulator::ScheduleWithContext (m_link[wire].m_dst->GetNode ()->GetId (),
                                  txTime + m_delay, &ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet::Receive,
                                  m_link[wire].m_dst, p->Copy ());

  // Call the tx anim callback on the net device
  m_txrxPointToPoint (p, src, m_link[wire].m_dst, txTime, txTime + m_delay);
  return true;
}

std::size_t
ns3_ClassSRNC03_PointToPointChannel::GetNDevices (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_nDevices;
}

Ptr<ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet>
ns3_ClassSRNC03_PointToPointChannel::GetPointToPointDevice (std::size_t i) const
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (i < 2);
  return m_link[i].m_src;
}

Ptr<NetDevice>
ns3_ClassSRNC03_PointToPointChannel::GetDevice (std::size_t i) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return GetPointToPointDevice (i);
}

Time
ns3_ClassSRNC03_PointToPointChannel::GetDelay (void) const
{
  return m_delay;
}

Ptr<ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet>
ns3_ClassSRNC03_PointToPointChannel::GetSource (uint32_t i) const
{
  return m_link[i].m_src;
}

Ptr<ns3_ClassSRNC01_PointToPointNetDevice_SRNCEthernet>
ns3_ClassSRNC03_PointToPointChannel::GetDestination (uint32_t i) const
{
  return m_link[i].m_dst;
}

bool
ns3_ClassSRNC03_PointToPointChannel::IsInitialized (void) const
{
  NS_ASSERT (m_link[0].m_state != INITIALIZING);
  NS_ASSERT (m_link[1].m_state != INITIALIZING);
  return true;
}

} // namespace ns3
