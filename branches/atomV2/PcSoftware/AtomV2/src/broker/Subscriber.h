/*
 * Subscriber.h
 *
 *  Created on: Apr 25, 2009
 *      Author: Mattias Runge
 */

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include <iostream>

#include <boost/bind.hpp>
#include <boost/signal.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/locks.hpp>
#include "message/Message.h"
#include "log/Logger.h"
#include "thread/Thread.h"
#include "thread/Queue.hpp"
#include <boost/make_shared.hpp>

namespace atom {

using namespace message;

class Subscriber : public boost::signals::trackable, public thread::Thread
{
public:
	typedef boost::shared_ptr<Subscriber> Pointer;

	Subscriber(bool receive_from_self);
	virtual ~Subscriber();

protected:
	void PutMessage(Message::pointer message);
	Message::pointer CreateMessage() const;

	virtual void OnMessage(Message::pointer message) = 0;

private:
	log::Logger LOG;

	thread::Queue<Message::pointer> queue_;
	bool receive_from_self_;

	boost::condition on_message_condition_;
	boost::mutex guard_mutex_;

	void Slot_OnMessage(Message::pointer message);

	void Run();
};

}
}

#endif /* SUBSCRIBER_H_ */
