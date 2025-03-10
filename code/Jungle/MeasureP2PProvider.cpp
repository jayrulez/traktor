/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Timer/Timer.h"
#include "Jungle/MeasureP2PProvider.h"

namespace traktor::jungle
{
	namespace
	{

Timer s_timer;

	}

T_IMPLEMENT_RTTI_CLASS(L"traktor.jungle.MeasureP2PProvider", MeasureP2PProvider, IPeer2PeerProvider)

MeasureP2PProvider::MeasureP2PProvider(IPeer2PeerProvider* provider)
:	m_provider(provider)
,	m_time(0.0)
,	m_sentBytes(0)
,	m_recvBytes(0)
,	m_sentBps(0.0)
,	m_recvBps(0.0)
{
	m_time = s_timer.getElapsedTime();
}

bool MeasureP2PProvider::update()
{
	const double time = s_timer.getElapsedTime();
	const double duration = time - m_time;

	const double sentBps = (m_sentBytes * 8.0) / duration;
	const double recvBps = (m_recvBytes * 8.0) / duration;

	if (m_sentBps > 0.0)
		m_sentBps = (m_sentBps * 0.9 + sentBps * 0.1);
	else
		m_sentBps = sentBps;

	if (m_recvBps > 0.0)
		m_recvBps = (m_recvBps * 0.9 + recvBps * 0.1);
	else
		m_recvBps = recvBps;

	m_time = time;

	return m_provider->update();
}

net_handle_t MeasureP2PProvider::getLocalHandle() const
{
	return m_provider->getLocalHandle();
}

int32_t MeasureP2PProvider::getPeerCount() const
{
	return m_provider->getPeerCount();
}

net_handle_t MeasureP2PProvider::getPeerHandle(int32_t index) const
{
	return m_provider->getPeerHandle(index);
}

std::wstring MeasureP2PProvider::getPeerName(int32_t index) const
{
	return m_provider->getPeerName(index);
}

Object* MeasureP2PProvider::getPeerUser(int32_t index) const
{
	return m_provider->getPeerUser(index);
}

bool MeasureP2PProvider::setPrimaryPeerHandle(net_handle_t node)
{
	return m_provider->setPrimaryPeerHandle(node);
}

net_handle_t MeasureP2PProvider::getPrimaryPeerHandle() const
{
	return m_provider->getPrimaryPeerHandle();
}

bool MeasureP2PProvider::send(net_handle_t node, const void* data, int32_t size)
{
	m_sentBytes += size;
	return m_provider->send(node, data, size);
}

int32_t MeasureP2PProvider::recv(void* data, int32_t size, net_handle_t& outNode)
{
	const int32_t nbytes = m_provider->recv(data, size, outNode);
	if (nbytes > 0)
		m_recvBytes += nbytes;

	return nbytes;
}

float MeasureP2PProvider::getSendBitsPerSecond() const
{
	return float(m_sentBps);
}

float MeasureP2PProvider::getRecvBitsPerSecond() const
{
	return float(m_recvBps);
}

}
