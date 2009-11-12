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
#include "log/Logger.h"

namespace atom {
namespace subscribers {

class Monitor : public Subscriber
{
public:
	typedef boost::shared_ptr<Monitor> Pointer;

	Monitor();
	virtual ~Monitor();

protected:
	void OnMessage(Message::Pointer message);

private:
	log::Logger LOG;
};

} // namespace subscribers
} // namesapce atom

#endif /* MONITOR_HPP_ */
