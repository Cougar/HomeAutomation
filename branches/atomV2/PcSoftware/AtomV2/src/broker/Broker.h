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
#include "log/Logger.h"

namespace atom {

class Broker
{
public:
	typedef boost::shared_ptr<Broker> Pointer;

	~Broker();

	void Connect(const Signal_OnMessage::slot_type &slot);
	void Put(Message::Pointer message);

	static Pointer GetInstance();

private:
	typedef boost::signal<void(Message::pointer message)> Signal_OnMessage;

	Broker();

	Signal_OnMessage OnMessage;

	log::Logger LOG;

	static Pointer instance_;


};

} // namespace atom

#endif /* BROKER_H_ */
