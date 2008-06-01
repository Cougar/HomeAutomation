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

import os
import sys
import ServiceBase
import Settings

gServiceMan = False

def GetServiceManager():
	global gServiceMan
	if gServiceMan == False:
		gServiceMan = ServiceManager()
	return gServiceMan

class ServiceManager ():
	services = {}
	modules = {}
	
	def __init__(self):
		self.ImportAvailableServices()
	
	def ImportAvailableServices(self):
		servicesPath = Settings.Settings["PATH"] + "/Services"
		sys.path.append(servicesPath)

		for serviceName in os.listdir(servicesPath):
			if serviceName[0] != '.' and serviceName.endswith(".py"):
				serviceName = serviceName[:serviceName.find(".")]
				self.modules[serviceName] = __import__(serviceName)
				print "Loaded service: " + serviceName

		print ""
	
	def HasService(self, serviceId, serviceType):
		return self.services.has_key(str(serviceType) + "_" + str(serviceId))
	
	def AddService(self, serviceId, serviceType):
		if not self.HasService(serviceId, serviceType):
			
			if self.modules[serviceType]:
				self.services[str(serviceType) + "_" + str(serviceId)] = self.modules[serviceType].Service(serviceId)
			else:
				print "Warning: Unknown service: Type: " + serviceType
				return False

		return self.services[str(serviceType) + "_" + str(serviceId)]
		
	def GetService(self, serviceId, serviceType):
		return self.services[str(serviceType) + "_" + str(serviceId)]
	
