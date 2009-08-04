/*
 * Thread.cpp
 *
 *  Created on: Apr 26, 2009
 *      Author: Mattias Runge
 */

#include "Thread.h"

using namespace std;

namespace atom {
namespace thread {

Thread::Thread()
{
	this->myIsRunning = false;
}

Thread::~Thread()
{
	//cout << "Thread destroy" << endl;
	this->stop();
}

void Thread::start()
{
	if (!this->myIsRunning)
	{
		boost::thread localThread(boost::bind(&Thread::runBase, this));
		this->myThread = localThread.move();
		this->myIsRunning = true;
	}
}

void Thread::stop()
{
	this->myThread.interrupt();
	this->myThread.join();
	this->myIsRunning = false;
}

boost::thread::id Thread::getId()
{
	return this->myThread.get_id();
}

bool Thread::isRunning()
{
	return this->myIsRunning;
}

void Thread::runBase()
{
	this->myIsRunning = true;
	this->run();
	this->myIsRunning = false;
}

void Thread::run()
{
}

}
}
