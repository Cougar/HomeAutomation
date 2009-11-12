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
namespace thread {

class Thread
{
public:
	Thread();
	virtual ~Thread();

	void Start();
	void Stop();
	boost::thread::id GetId();
	bool IsRunning();

protected:
	virtual void Run() = 0;

private:
	boost::thread thread_;
	bool is_running_;

	void RunBase();
};

} // namespace thread
} // namespace atom

#endif /* THREAD_H_ */
