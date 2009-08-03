/*
 * TSQueue.hpp
 *
 *  Created on: Apr 27, 2009
 *      Author: Mattias Runge
 */

#ifndef TSQUEUE_HPP_
#define	TSQUEUE_HPP_

#include <queue>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

namespace atom {
namespace utils {

using namespace std;

template<class T>
class TSQueue
{
	typedef boost::mutex::scoped_lock lock;

public:
	TSQueue()
	{
	}

	~TSQueue()
	{
	}

	void push(T item)
	{
		lock guard(this->mutex);
		this->_queue.push(item);
	}

	T pop()
	{
		lock guard(this->mutex);
		T item = this->_queue.front();
		this->_queue.pop();
		return item;
	}

	int size()
	{
		lock guard(this->mutex);
		return this->_queue.size();
	}

private:
	queue<T> _queue;
	boost::mutex mutex;
};

}
}

#endif /* TSQUEUE_HPP_ */

