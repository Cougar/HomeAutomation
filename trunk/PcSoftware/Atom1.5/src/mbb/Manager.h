/*
 * 
 *  Copyright (C) 2012  Mattias Runge
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

#ifndef MBB_MANAGER_H
#define MBB_MANAGER_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "logging/Logger.h"
#include "net/Subscriber.h"
#include "net/types.h"
#include "common/Byteset.h"

namespace atom {
namespace mbb {

class Manager : public net::Subscriber
{
public:
  typedef boost::shared_ptr<Manager> Pointer;
  
  virtual ~Manager();
  
  static Pointer Instance();
  static void Create(unsigned int port);
  static void Delete();
    
private:
  static Pointer instance_;
  
  static logging::Logger LOG;
  
  net::ServerId server_id_;
  
  void SlotOnNewStateHandler(net::ClientId client_id, net::ServerId server_id, net::ClientState client_state);
  void SlotOnNewDataHandler(net::ClientId client_id, net::ServerId server_id, common::Byteset data);
  
  Manager(unsigned int port);
};

}; // namespace mbb
}; // namespace atom

#endif // MBB_MANAGER_H
