/*
 * Subscriber.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Mattias Runge
 */

#include "Subscriber.h"

#include "broker/Broker.h"

namespace atom {

Subscriber::Subscriber(bool receive_from_self)
{
	LOG.setName("Subscriber");

	this->receive_from_self_ = receive_from_self;

	Broker::GetInstance()->Connect(boost::bind(&Subscriber::Slot_OnMessage, this, _1));
}

Subscriber::~Subscriber()
{
	this->Stop();
}

void Subscriber::Put(Message::Pointer message)
{
	Broker::GetInstance()->Put(message);
}

Message::Pointer CreateMessage()
{
	return Message::Pointer(new Message(static_cast<void *>this));
}

void Subscriber::Slot_OnMessage(Message::Åointer message)
{
	if (this->receive_from_self_ || message->GetOrigin() != static_cast<void *>this)
	{
		this->queue_.push(message);
		this->on_message_condition_.notify_all();
	}
}

void Subscriber::Run()
{
	while (true)
	{
		boost::mutex::scoped_lock guard(this->guard_mutex_);

		try
		{
			while (this->queue_.size() > 0)
			{
				this->OnMessage(this->queue_.pop());
			}

			this->on_message_condition_.wait(guard);
		}
		catch (boost::thread_interrupted e)
		{
			guard.unlock();
			LOG.warn("Thread interrupted");
			break;
		}
	}
}

} // namespace atom
