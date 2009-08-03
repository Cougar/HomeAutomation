/*
 * Thread.h
 *
 *  Created on: Apr 26, 2009
 *      Author: Mattias Runge
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <boost/thread.hpp>

namespace atom {
namespace utils {

class Thread
{
public:
	Thread();
	virtual ~Thread();

	void start();
	void stop();
	boost::thread::id getId();
	bool isRunning();


protected:
	virtual void run();

private:
	boost::thread _thread;
	bool _running;

	void runBase();
};

}
}

#endif /* THREAD_H_ */
