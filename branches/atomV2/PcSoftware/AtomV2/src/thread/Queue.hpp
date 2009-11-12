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
public:
	Queue() { }
	~Queue() { }

	void Push(T item)
	{
		boost::mutex::scoped_lock guard(this->guard_mutex_);

		this->queue_.push(item);
	}

	T pop()
	{
		boost::mutex::scoped_lock guard(this->guard_mutex_);

		T item = this->queue_.front();
		this->queue_.pop();
		return item;
	}

	unsigned int size()
	{
		boost::mutex::scoped_lock guard(this->guard_mutex_);

		return this->queue_.size();
	}

private:
	std::queue<T> queue_;
	boost::mutex guard_mutex_;
};

} // namespace thread
} // namespace atom

#endif /* QUEUE_HPP_ */

