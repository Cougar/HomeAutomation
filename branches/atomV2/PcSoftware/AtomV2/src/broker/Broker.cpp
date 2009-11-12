/*
 * Broker.cpp
 *
 *  Created on: Jul 20, 2009
 *      Author: Mattias Runge
 */

#include "Broker.h"

namespace atom {

Broker::Pointer Broker::instance_(new Broker());

Broker::Broker()
{
	LOG.setName("Broker");
}

Broker::~Broker()
{
}

void Broker::Connect(const Signal_OnMessage::slot_type &slot)
{
	LOG.info("New subscriber connected.");
	this->OnMessage.connect(slot);
}

void Broker::Put(Message::Pointer message)
{
	LOG.info("New message received.");
	this->OnMessage(message);
}

Broker::Pointer Broker::GetInstance()
{
	return Broker::instance_;
}

} // namespace atom
