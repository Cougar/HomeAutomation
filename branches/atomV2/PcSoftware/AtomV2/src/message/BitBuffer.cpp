/*
 * BitBuffer.cpp
 *
 *  Created on: Jul 17, 2009
 *      Author: Mattias Runge
 */

#include "BitBuffer.h"

namespace atom {
namespace message {

BitBuffer::BitBuffer(unsigned char bytes[], unsigned int length)
{
	for (unsigned int n = 0; n < length; n++)
	{
		this->myBuffer.push_back(bytes[n]);
	}
}

BitBuffer::BitBuffer(byte_list bytes) {
	this->setFromBytes(bytes);
}


BitBuffer::BitBuffer() {
}

BitBuffer::~BitBuffer() {
}


byte_list BitBuffer::getAsBytes()
{
	return this->myBuffer;
}

void BitBuffer::setFromBytes(byte_list bytes)
{
	this->myBuffer = bytes;
}

void BitBuffer::incrementPointer(unsigned int steps)
{

}

void BitBuffer::clear()
{
	this->myBuffer.clear();
	// TÒDO resent pointers
}


void BitBuffer::read(unsigned int length, unsigned long & value)
{
}

void BitBuffer::read(unsigned int length, long & value)
{
}

void BitBuffer::read(unsigned int length, bool & value)
{
}



void BitBuffer::write(unsigned int length, unsigned long value)
{
}

void BitBuffer::write(unsigned int length, long  value)
{
}

void BitBuffer::write(unsigned int length, bool value)
{
}


}
}
