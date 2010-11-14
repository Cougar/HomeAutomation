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

#ifndef VM_TYPES_H
#define VM_TYPES_H

#include <map>
#include <string>
#include <vector>
#include <v8.h>
#include <boost/shared_ptr.hpp>

namespace atom {
namespace vm {

typedef v8::Persistent<v8::Function> Function;
typedef v8::Handle<v8::Value> Value;
typedef std::map<std::string, Function> ImportFunctionList;
typedef std::map<std::string, v8::InvocationCallback> ExportFunctionList;
typedef std::vector<std::string> FunctionNameList;
typedef std::vector<Value> ArgumentList;
typedef boost::shared_ptr<ArgumentList> ArgumentListPointer;
}; // namespace vm
}; // namespace atom

#endif // VM_TYPES_H
