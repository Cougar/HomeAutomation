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

void BitBuffer::readDecimal(unsigned int length, unsigned int scaling, double & value)
{
}

void BitBuffer::writeDecimal(unsigned int length, unsigned int scaling, double value)
{
}


void BitBuffer::readBasicType(unsigned int length, unsigned long & value)
{
}

void BitBuffer::writeBasicType(unsigned int length, unsigned long value)
{
}

void BitBuffer::readBasicType(unsigned int length, long & value)
{
}

void BitBuffer::writeBasicType(unsigned int length, long  value)
{
}

void BitBuffer::readBasicType(unsigned int length, bool & value)
{
}

void BitBuffer::writeBasicType(unsigned int length, bool value)
{
}


}
}
