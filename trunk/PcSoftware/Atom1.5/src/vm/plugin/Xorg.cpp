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

#include "Xorg.h"

#include <boost/lexical_cast.hpp>

#include <X11/Xlib.h>

#include "vm/Manager.h"

namespace atom {
namespace vm {
namespace plugin {

logging::Logger Xorg::LOG("vm::plugin::xorg");
    
Xorg::Xorg(boost::asio::io_service& io_service) : Plugin(io_service)
{
    this->name_ = "xorg";
    
    this->ExportFunction("XorgExport_SendKey",       Xorg::Export_SendKey);
}

Xorg::~Xorg()
{
}

void Xorg::InitializeDone()
{
    Plugin::InitializeDone();
}

void Xorg::CallOutput(unsigned int request_id, std::string output)
{
    LOG.Info(output);
}

Value Xorg::Export_SendKey(const v8::Arguments& args)
{
    v8::Locker lock;
    v8::Context::Scope context_scope(vm::Manager::Instance()->GetContext());
    v8::HandleScope handle_scope;
    
    LOG.Debug(std::string(__FUNCTION__) + " called!");
    
    if (args.Length() < 1)
    {
        LOG.Error(std::string(__FUNCTION__) + ": To few arguments.");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    Display* display = XOpenDisplay(":0");
    
    if (display == NULL)
    {
        LOG.Warning("Could not locate display :0");
        return handle_scope.Close(v8::Boolean::New(false));
    }
    
    // Get the root window for the current display.
    Window winRoot = XDefaultRootWindow(display);
    
    // Find the window which has the current keyboard focus.
    Window winFocus;
    int    revert;
    XGetInputFocus(display, &winFocus, &revert);
    
    XKeyEvent event;
    
    event.display     = display;
    event.window      = winFocus;
    event.root        = winRoot;
    event.subwindow   = None;
    event.time        = CurrentTime;
    event.x           = 1;
    event.y           = 1;
    event.x_root      = 1;
    event.y_root      = 1;
    event.same_screen = True;
    event.keycode     = XKeysymToKeycode(display, args[0]->Int32Value());
    event.state       = 0;
    event.type        = KeyPress;

    LOG.Debug("Sent code " + boost::lexical_cast<std::string>(args[0]->Int32Value()));
    
    XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
    
    event.type = KeyRelease;
    
    XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
    
    XCloseDisplay(display);
    
    return handle_scope.Close(v8::Boolean::New(true));
}
    
}; // namespace plugin
}; // namespace vm
}; // namespace atom
