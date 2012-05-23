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


#ifndef URL_H
#define URL_H

#include <string>
#include "types.h"

namespace atom {
namespace net {
  
class Url
{
public:
  Url();
  Url(std::string url);
  
  void                SetUrl(std::string url);
  
  std::string         GetRaw();
  ApplicationProtocol GetProtocol();
  std::string         GetHostname();
  unsigned int        GetPort();
  std::string         GetPath();
  
private:
  std::string         raw_;
  ApplicationProtocol protocol_;
  std::string         hostname_;
  unsigned int        port_;
  std::string         path_;
};


}; // namespace net
}; // namespace atom

#endif // URL_H
