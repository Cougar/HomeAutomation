/*
 * Broker.h
 *
 *  Created on: Apr 25, 2009
 *      Author: Mattias Runge
 */

#ifndef BROKER_H_
#define BROKER_H_

#include <boost/signal.hpp>
#include "../message/Message.h"
#include "../utils/Logger.h"

namespace atom {
namespace broker {

using namespace std;
using namespace atom::utils;
using namespace atom::message;

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

	OnMessageSignal onNewMessage;
	Logger LOG;
};

}
}
#endif /* BROKER_H_ */
