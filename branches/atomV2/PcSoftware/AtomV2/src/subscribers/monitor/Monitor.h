/*
 * Monitor.h
 *
 *  Created on: Apr 25, 2009
 *      Author: Mattias Runge
 */

#ifndef MONITOR_H_
#define MONITOR_H_

#include <iostream>
#include "../../broker/Subscriber.h"
#include "../../message/Message.h"
#include "../../utils/Logger.h"
#include <boost/make_shared.hpp>

namespace atom {
namespace subscribers {

using namespace atom::broker;
using namespace atom::utils;
using namespace atom::message;

class Monitor : public Subscriber
{
public:
	typedef boost::shared_ptr<Monitor> pointer;

	Monitor(Broker::pointer broker);
	virtual ~Monitor();

protected:
	void onNewMessage(Message::pointer message);

private:
	Logger LOG;
};

}
}

#endif /* MONITOR_HPP_ */
