/*
 * Broker.cpp
 *
 *  Created on: Jul 20, 2009
 *      Author: Mattias Runge
 */

#include "Broker.h"

namespace atom {
namespace broker {

Broker::pointer Broker::Instance(new Broker());

void Broker::connect(const OnMessageSignal::slot_type & slot)
{
	LOG.info("New subscriber connected.");
	this->onNewMessage.connect(slot);
}

void Broker::put(Message::pointer message)
{
	LOG.info("New message received.");
	this->onNewMessage(message);
}

Broker::pointer Broker::getInstance()
{
	return Instance;
}

Broker::Broker()
{
	LOG.setName("Broker");
}

Broker::~Broker()
{
}

}
}
