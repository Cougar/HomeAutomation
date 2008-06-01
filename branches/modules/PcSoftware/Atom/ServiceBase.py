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

class ServiceBase:
	Id = 0
	Type = ''
	Online = False
	events = {}
	timers = {}
	timerArgs = {}

	def __init__(self, id, type):
		self.Id = id
		self.Type = type
		
	#def CreateInterval(self, interval, name, callback, args):
	#	self.timersArgs[name] = args
	#	self.timers[name] = threading.Timer(interval, callback, args)
	#	self.timers[name].start()
		

	def SetOnline(self):
		if not self.Online:
			print self.Type + " Service - " + str(self.Id) + ": Online" 
			self.Online = True
			return True
		return False

	def SetOffline(self):
		if self.Online:
			print self.Type + " Service - " + str(self.Id) + ": Offline" 
			self.Online = False
			return True
		return False

	#def RegisterIntervalHandler(self, event, interval, args, callback)
	#	self.serviceImap.RegisterIntervalHandler("onNewMail", 10, ["runge.se", "mattias@runge.se", "getur82", "Inbox"], self.onCheckImapHandler)
	
	def RegisterEventHandler(self, event, callback):
		self.events[event].append(callback)
		
	def CallEvent(self, event, data):
		for func in self.events[event]:
			func(data)
	
	def Log(self, text):
		print self.Type + " Service - " + str(self.Id) + ": " + text
