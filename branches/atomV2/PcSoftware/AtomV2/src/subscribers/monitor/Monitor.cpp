/*
 * Monitor.cpp
 *
 *  Created on: Jul 22, 2009
 *      Author: Mattias Runge
 */

#include "Monitor.h"

namespace atom {
namespace subscribers {

Monitor::Monitor() : Subscriber(true)
{
	LOG.setName("MonitorSubscriber");
	this->Start();
}

Monitor::~Monitor()
{
	this->Stop();
}

void Monitor::OnMessage(Message::Pointer message)
{
	LOG.info(message->ToString());

	// TODO print origin of message also
}

} // namespace subscribers
} // namespace atom
