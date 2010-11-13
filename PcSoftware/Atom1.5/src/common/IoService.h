/*
 * 
 *  Copyright (C) 2010  Mattias Runge
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#ifndef COMMON_IOSERVICE_H
#define COMMON_IOSERVICE_H

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace atom {
namespace common {

class IoService
{
public:
    typedef boost::shared_ptr<IoService> Pointer;
    
    IoService();
    virtual ~IoService();
    
protected:
    typedef boost::shared_ptr<char> TrackerPointer;
    
    boost::asio::io_service io_service_;
    TrackerPointer tracker_;
    
private:
    boost::thread thread_;
    boost::asio::io_service::work io_service_work_;
};

}; // namespace common
}; // namespace atom

#endif // COMMON_IOSERVICE_H
