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
#include "Broker.h"
#include "message/Message.h"
#include "log/Logger.h"
#include "thread/Thread.h"
#include "thread/Queue.hpp"
#include <boost/make_shared.hpp>

namespace atom {
namespace broker {

using namespace message;

class Subscriber : public boost::signals::trackable, public thread::Thread
{
	typedef boost::mutex::scoped_lock lock;

public:
	typedef boost::shared_ptr<Subscriber> pointer;

	Subscriber(boost::shared_ptr<Broker> broker, bool receiveFromMyself);
	virtual ~Subscriber();

protected:
	boost::shared_ptr<Broker> myBroker;

	void put(Message::pointer message);
	virtual void onNewMessage(Message::pointer message);

private:
	log::Logger LOG;
	thread::Queue<Message::pointer> myQueue;
	bool myReceiveFromMyself;
	boost::condition myCondition;
	boost::mutex myMutex;

	void newMessageHandler(Message::pointer message);
	void run();
};

}
}

#endif /* SUBSCRIBER_H_ */
