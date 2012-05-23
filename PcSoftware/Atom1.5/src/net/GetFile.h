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

#ifndef NET_GETFILE_H
#define NET_GETFILE_H

#include "net/Manager.h"

#include "net/types.h"
#include "net/Url.h"
#include "net/Subscriber.h"

namespace atom {
namespace net {

class GetFile : public net::Subscriber
{
public:
  typedef boost::shared_ptr<GetFile> Pointer;
  typedef boost::signals2::signal<void(bool success, net::Url url, std::string temporary_filepath)> SignalOnComplete;

  GetFile();
  virtual ~GetFile();
  
  void Start(Url url, const SignalOnComplete::slot_type& slot_on_complete);
  
protected:
  typedef boost::shared_ptr<char> TrackerPointer;
  
  net::Url          url_;
  net::SocketId     socket_id_;
  std::string       filename_;
  SignalOnComplete  signal_on_complete_;
  TrackerPointer    tracker_;
  bool              header_received_;
  bool              status_;
  
  void SlotOnNewStateHandler(net::SocketId id, net::ClientState client_state);
  void SlotOnNewClientHandler(net::SocketId id, net::SocketId server_id);
  void SlotOnNewDataHandler(net::SocketId id, common::Byteset data);
};

}; // namespace net
}; // namespace atom

#endif // NET_GETFILE_H
