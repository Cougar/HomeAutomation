############################################################################
#    Copyright (C) 2008 by Mattias Runge   #
#    mattias@runge.se   #
#                                                                          #
#    This program is free software; you can redistribute it and#or modify  #
#    it under the terms of the GNU General Public License as published by  #
#    the Free Software Foundation; either version 2 of the License, or     #
#    (at your option) any later version.                                   #
#                                                                          #
#    This program is distributed in the hope that it will be useful,       #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#    GNU General Public License for more details.                          #
#                                                                          #
#    You should have received a copy of the GNU General Public License     #
#    along with this program; if not, write to the                         #
#    Free Software Foundation, Inc.,                                       #
#    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
############################################################################

import ServiceBase
import BackendCan
import CanModuleTypes

class Service (ServiceBase.ServiceBase):
	Width = 20
	Height = 4
	Backlight = 255
	
	def __init__(self, id):
		ServiceBase.ServiceBase.__init__(self, id, "LCD")
		
		self.events = {"onGetSize":[], "onGetBacklight":[]}

		self.canBackend = BackendCan.GetBackendCan()
		self.canBackend.AddCallback(self.HandleMsg)
		
	def SetOnline(self):
		if ServiceBase.ServiceBase.SetOnline(self):
			self.RequestSize()
			self.Clear()
			self.SetBacklight(self.Backlight)
		
	def HandleMsg(self, canMsg):
		if not self.Online:
			return False

		try:
			if self.ModuleType == canMsg.ModuleType and self.ModuleId == canMsg.ModuleId:
				if canMsg.Command == 4: # Size
					if len(canMsg.Data) >= 2:
						self.Width = canMsg.Data[0]
						self.Height = canMsg.Data[1]
						self.Log("Size received: " + str(self.Width) + "x" + str(self.Height))
						self.CallEvent("onGetSize", 0)
					else:
						self.Log("Malformed data received")
				elif canMsg.Command == 5: # Backlight
					if len(canMsg.Data) >= 1:
						self.Backlight = canMsg.Data[0]
						self.Log("Backlight level received: " + str(self.Backlight))
						self.CallEvent("onGetBacklight", 0)
					else:
						self.Log("Malformed data received")
				else:
					self.Log("Unknown command " + str(canMsg.Command))
		except KeyError:
			pass
	
	def SetCursorPosition(self, x, y):
		if not self.Online:
			return False

		canMsg = BackendCan.CanMessage()
		canMsg.ClassName = 12;
		canMsg.ModuleType = self.ModuleType
		canMsg.ModuleId = self.ModuleId
		canMsg.Direction = 0
		canMsg.Command = 2
		
		canMsg.Data.append(x)
		canMsg.Data.append(y)
		
		self.canBackend.QueueMsg(canMsg.Compile())
	
	def Clear(self):
		if not self.Online:
			return False

		canMsg = BackendCan.CanMessage()
		canMsg.ClassName = 12;
		canMsg.ModuleType = self.ModuleType
		canMsg.ModuleId = self.ModuleId
		canMsg.Direction = 0
		canMsg.Command = 1
		
		self.canBackend.QueueMsg(canMsg.Compile())
	
	def Print(self, text):
		if not self.Online:
			return False

		canMsg = BackendCan.CanMessage()
		canMsg.ClassName = 12;
		canMsg.ModuleType = self.ModuleType
		canMsg.ModuleId = self.ModuleId
		canMsg.Direction = 0
		canMsg.Command = 3
		
		for char in text:
			canMsg.Data.append(ord(char))
			
			if len(canMsg.Data) == 8:
				self.canBackend.QueueMsg(canMsg.Compile())
				del canMsg.Data
				canMsg.Data = []
				
		if len(canMsg.Data) > 1:
			self.canBackend.QueueMsg(canMsg.Compile())
	
	def SetBacklight(self, level):
		if not self.Online:
			return False

		canMsg = BackendCan.CanMessage()
		canMsg.ClassName = 12;
		canMsg.ModuleType = self.ModuleType
		canMsg.ModuleId = self.ModuleId
		canMsg.Direction = 0
		canMsg.Command = 5
	
		canMsg.Data.append(level)
		
		self.canBackend.QueueMsg(canMsg.Compile())

		self.Backlight = level
	
	def RequestSize(self):
		if not self.Online:
			return False

		canMsg = BackendCan.CanMessage()
		canMsg.ClassName = 12;
		canMsg.ModuleType = self.ModuleType
		canMsg.ModuleId = self.ModuleId
		canMsg.Direction = 0
		canMsg.Command = 4
		
		self.canBackend.QueueMsg(canMsg.Compile())
		
	def RequestBacklight(self):
		if not self.Online:
			return False

		canMsg = BackendCan.CanMessage()
		canMsg.ClassName = 12;
		canMsg.ModuleType = self.ModuleType
		canMsg.ModuleId = self.ModuleId
		canMsg.Direction = 0
		canMsg.Command = 5

		self.canBackend.QueueMsg(canMsg.Compile())

