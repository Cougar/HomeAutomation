/*
 * Queue.cpp
 *
 *  Created on: Aug 04, 2009
 *      Author: Mattias Runge
 */

#include "Queue.h"

namespace atom {
namespace thread {


template<typename T>
Queue<T>::Queue()
{
}

template<typename T>
Queue<T>::~Queue()
{
}

template<typename T>
void Queue<T>::push(T item)
{
	lock guard(this->myMutex);
	this->myQueue.push(item);
}

template<typename T>
T Queue<T>::pop()
{
	lock guard(this->myMutex);
	T item = this->myQueue.front();
	this->myQueue.pop();
	return item;
}

template<typename T>
unsigned int Queue<T>::size()
{
	lock guard(this->myMutex);
	return this->myQueue.size();
}


}
}
