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

#ifndef VM_PLUGIN_MYSQL_H
#define VM_PLUGIN_XORG_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <mysql.h>

#include "vm/Plugin.h"

namespace atom {
namespace vm {
namespace plugin {
  
class MySql : public Plugin
{
public:
  typedef boost::shared_ptr<MySql> Pointer;
  
  MySql(boost::asio::io_service& io_service);
  virtual ~MySql();
  
  void InitializeDone();
  void CallOutput(unsigned int request_id, std::string output);
  
private:
  typedef unsigned int ResourceId;
  typedef std::map<ResourceId, MYSQL*> Resources;
  
  static Resources resources_;
  
  static ResourceId GetFreeResourceId();
  
  static Value Export_Connect(const v8::Arguments& args);
  static Value Export_Close(const v8::Arguments& args);
  static Value Export_Query(const v8::Arguments& args);
  static Value Export_SelectDb(const v8::Arguments& args);
  static Value Export_AffectedRows(const v8::Arguments& args);
  static Value Export_InsertId(const v8::Arguments& args);
};
  
}; // namespace plugin
}; // namespace vm
}; // namespace atom

#endif // VM_PLUGIN_MYSQL_H
