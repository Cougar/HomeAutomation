/*
 * Broker.h
 *
 *  Created on: Apr 25, 2009
 *      Author: Mattias Runge
 */

#ifndef BROKER_H_
#define BROKER_H_

#include <boost/signal.hpp>
#include <boost/make_shared.hpp>
#include "message/Message.h"
#include "utils/Logger.h"

namespace atom {
namespace broker {

using namespace std;
using namespace utils;
using namespace message;

class Broker
{
	typedef boost::signal<void(Message::pointer message)> OnMessageSignal;

public:
	typedef boost::shared_ptr<Broker> pointer;

	~Broker();

	void connect(const OnMessageSignal::slot_type & slot);
	void put(Message::pointer message);

	static pointer getInstance();

private:
	Broker();

	static pointer Instance;

	Logger LOG;

	OnMessageSignal onNewMessage;
};

}
}
#endif /* BROKER_H_ */
