/*
 * Monitor.h
 *
 *  Created on: Apr 25, 2009
 *      Author: Mattias Runge
 */

#ifndef MONITOR_H_
#define MONITOR_H_

#include <boost/make_shared.hpp>
#include "broker/Subscriber.h"
#include "message/Message.h"
#include "utils/Logger.h"

namespace atom {
namespace subscribers {

using namespace broker;
using namespace utils;
using namespace message;

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
