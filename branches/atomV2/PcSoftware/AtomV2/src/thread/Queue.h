/*
 * Queue.h
 *
 *  Created on: Apr 27, 2009
 *      Author: Mattias Runge
 */

#ifndef QUEUE_H_
#define	QUEUE_H_

#include <queue>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

namespace atom {
namespace thread {

using namespace std;

template<typename T>
class Queue
{
	typedef boost::mutex::scoped_lock lock;

public:
	Queue();
	~Queue();

	void push(T item);
	T pop();
	unsigned int size();

private:
	std::queue<T> myQueue;
	boost::mutex myMutex;
};

}
}

#endif /* QUEUE_H_ */

