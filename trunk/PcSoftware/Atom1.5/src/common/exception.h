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

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <iostream>
#include <stdexcept>
#include <exception>
#include <stdio.h>
#include <stdint.h>

namespace atom {
namespace exception {

#define CREATE_EXCEPTION(name, string)                                        \
  class name##_class : public std::exception                                  \
  {                                                                           \
    virtual const char* what() const throw()                                  \
    {                                                                         \
      return "string";                                                        \
    }                                                                         \
  } name;
  

CREATE_EXCEPTION(invalid_in_param,            "Inparameter failure");
CREATE_EXCEPTION(missing_in_param,            "Missing inparameters");
CREATE_EXCEPTION(connect_failed,              "Connection failed");
CREATE_EXCEPTION(initialization_failed,       "Initialization failed");
CREATE_EXCEPTION(missing_resource,            "Missing resource");
CREATE_EXCEPTION(action_failed,               "Action failed");

  
}; // namespace exception
}; // namespace atom

#endif // EXCEPTION_H

