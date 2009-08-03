/*
 * Monitor.cpp
 *
 *  Created on: Jul 22, 2009
 *      Author: Mattias Runge
 */

#include "Monitor.h"

namespace atom {
namespace subscribers {

Monitor::Monitor(Broker::pointer broker) : Subscriber(broker, true)
{
	LOG.setName("MonitorSubscriber");
	this->start();
}

Monitor::~Monitor()
{
}

void Monitor::onNewMessage(Message::pointer message)
{
	LOG.info("Got message");
	// TODO Convert message to readable data with the help of the definitions in the xml-file
}

}
}
