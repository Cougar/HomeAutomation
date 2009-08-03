/*
 * Thread.cpp
 *
 *  Created on: Apr 26, 2009
 *      Author: Mattias Runge
 */

#include "Thread.h"
#include <iostream>

using namespace std;

namespace atom {
namespace utils {

Thread::Thread()
{
	this->_running = false;
}

Thread::~Thread()
{
	//cout << "Thread destroy" << endl;
	this->stop();
}

void Thread::start()
{
	if (!this->_running)
	{
		boost::thread localThread(boost::bind(&Thread::runBase, this));
		this->_thread = localThread.move();
		this->_running = true;
	}
}

void Thread::stop()
{
	this->_thread.interrupt();
	this->_thread.join();
	this->_running = false;
}

boost::thread::id Thread::getId()
{
	return this->_thread.get_id();
}

bool Thread::isRunning()
{
	return this->_running;
}

void Thread::runBase()
{
	this->_running = true;
	this->run();
	this->_running = false;
}

void Thread::run()
{
}

}
}
