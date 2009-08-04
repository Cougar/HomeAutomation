/*
 * Queue.hpp
 *
 *  Created on: Apr 27, 2009
 *      Author: Mattias Runge
 */

#ifndef QUEUE_HPP_
#define	QUEUE_HPP_

#include <queue>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

/* Note on templates
 * Can not have h/cpp structure because we will get linker problems
 * Read more here: http://www.parashift.com/c++-faq-lite/templates.html#faq-35.13
*/

namespace atom {
namespace thread {

template<typename T>
class Queue
{
	typedef boost::mutex::scoped_lock lock;

public:
	Queue()
	{
	}

	~Queue()
	{
	}

	void push(T item)
	{
		lock guard(this->myMutex);
		this->myQueue.push(item);
	}

	T pop()
	{
		lock guard(this->myMutex);
		T item = this->myQueue.front();
		this->myQueue.pop();
		return item;
	}

	unsigned int size()
	{
		lock guard(this->myMutex);
		return this->myQueue.size();
	}

private:
	std::queue<T> myQueue;
	boost::mutex myMutex;
};

}
}

#endif /* QUEUE_H_ */

