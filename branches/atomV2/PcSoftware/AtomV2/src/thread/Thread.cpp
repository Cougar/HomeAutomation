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
	this->is_running_ = false;
}

Thread::~Thread()
{
	//cout << "Thread destroy" << endl;
	this->Stop();
}

void Thread::Start()
{
	if (!this->is_running_)
	{
		boost::thread localThread(boost::bind(&Thread::RunBase, this));
		this->thread_ = localThread.move();
		this->is_running_ = true;
	}
}

void Thread::Stop()
{
	this->thread_.interrupt();
	this->thread_.join();
	this->is_running_ = false;
}

boost::thread::id Thread::GetId()
{
	return this->thread_.get_id();
}

bool Thread::IsRunning()
{
	return this->is_running_;
}

void Thread::RunBase()
{
	this->is_running_ = true;
	this->Run();
	this->is_running_ = false;
}

} // namespace thread
} // namespace atom
