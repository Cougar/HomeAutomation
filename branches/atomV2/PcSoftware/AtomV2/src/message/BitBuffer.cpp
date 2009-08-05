/*
 * BitBuffer.cpp
 *
 *  Created on: Jul 17, 2009
 *      Author: Mattias Runge
 */

#include "BitBuffer.h"

namespace atom {
namespace message {

BitBuffer::BitBuffer(byte_list bytes) {
	this->setFromBytes(bytes);
}

BitBuffer::BitBuffer() {
	this->myPointer = 0;
}

BitBuffer::~BitBuffer() {
}


byte_list BitBuffer::getAsBytes()
{
	byte_list bytes;

	// TODO implement

	return bytes;
}

void BitBuffer::setFromBytes(byte_list bytes)
{
	for (unsigned int n = 0; n < bytes.size(); n++)
	{
		this->myBits.push_back((bytes[n] >> 7 & 1) > 0);
		this->myBits.push_back((bytes[n] >> 6 & 1) > 0);
		this->myBits.push_back((bytes[n] >> 5 & 1) > 0);
		this->myBits.push_back((bytes[n] >> 4 & 1) > 0);
		this->myBits.push_back((bytes[n] >> 3 & 1) > 0);
		this->myBits.push_back((bytes[n] >> 2 & 1) > 0);
		this->myBits.push_back((bytes[n] >> 1 & 1) > 0);
		this->myBits.push_back((bytes[n] >> 0 & 1) > 0);
	}

	this->myPointer = 0;
}

unsigned int BitBuffer::getSize()
{
	return this->myBits.size();
}

unsigned int BitBuffer::getPointer()
{
	return this->myPointer;
}

void BitBuffer::incrementPointer(unsigned int steps)
{
	this->myPointer += steps;
}

void BitBuffer::clear()
{
	this->myBits.clear();
	this->myPointer = 0;
}


void BitBuffer::read(unsigned int length, unsigned long & value)
{
	value = 0;

	for (unsigned int n = 0; n < length; n++)
	{
		value = (value << n) | this->readBit();
	}
}

void BitBuffer::read(unsigned int length, long & value)
{
	// TODO Implement
}

void BitBuffer::read(unsigned int length, bool & value)
{
	value = this->readBit();
}



void BitBuffer::write(unsigned int length, unsigned long value)
{
	for (unsigned int n = length-1; n >= 0; n--)
	{
		this->writeBit((value >> n) & 1);
	}
}

void BitBuffer::write(unsigned int length, long value)
{
	// TODO Implement
}

void BitBuffer::write(unsigned int length, bool value)
{
	this->writeBit(value);
}


bool BitBuffer::readBit()
{
	this->myPointer++;

	if (this->myPointer > this->myBits.size())
	{
		return false;
	}

	return this->myBits[this->myPointer-1];
}

void BitBuffer::writeBit(bool value)
{
	this->myBits.push_back(value);
}

}
}
