/*
 * Subscriber.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#include "Subscriber.h"

namespace atom {
namespace broker {

Subscriber::Subscriber(boost::shared_ptr<Broker> broker, bool receiveFromMyself)
{
	LOG.setName("Subscriber");
	this->myBroker = broker;
	this->myReceiveFromMyself = receiveFromMyself;
	this->myBroker->connect(boost::bind(&Subscriber::newMessageHandler, this, _1));
}

Subscriber::~Subscriber()
{
	this->stop();
}

void Subscriber::put(Message::pointer message)
{
	message->setOrigin(this);
	this->myBroker->put(message);
}

void Subscriber::onNewMessage(Message::pointer message)
{
}

void Subscriber::newMessageHandler(Message::pointer message)
{
	if (this->myReceiveFromMyself || !message->isOrigin(this))
	{
		this->myQueue.push(message);

		this->myCondition.notify_all();
	}
}

void Subscriber::run()
{
	while (true)
	{
		lock guard(this->myMutex);

		try
		{
			while (this->myQueue.size() > 0)
			{
				this->onNewMessage(this->myQueue.pop());
			}

			this->myCondition.wait(guard);
		}
		catch (boost::thread_interrupted e)
		{
			guard.unlock();
			LOG.warn("Thread interrupted");
			break;
		}
	}
}

}
}
