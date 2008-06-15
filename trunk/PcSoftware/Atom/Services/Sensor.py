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
import Util

class Service (ServiceBase.ServiceBase):
	Values = {}
	
	def __init__(self, id):
		ServiceBase.ServiceBase.__init__(self, id, "Sensor")
		
		self.events = {"onValueUpdate":[]}

		self.canBackend = BackendCan.GetBackendCan()
		self.canBackend.AddCallback(self.HandleMsg)
	
	def HandleMsg(self, canMsg):
		if not self.Online:
			return False

		try:
			if self.ModuleType == canMsg.ModuleType and self.ModuleId == canMsg.ModuleId:
				if canMsg.Command == 4: # Temp in Celsius
					if len(canMsg.Data) >= 3:
						sensorId = canMsg.Data[0]

						self.Values[sensorId] = float(str(canMsg.Data[1]) + "." + str(canMsg.Data[2]/16)) # FIXME: Is this correct?

						self.Log(str(self.Values[sensorId]) + " Celsius reported by sensor " + str(sensorId))

						self.CallEvent("onValueUpdate", sensorId)
					else:
						self.Log("Malformed data received")
				elif canMsg.Command == 2: # Bus voltage
					if len(canMsg.Data) >= 3:
						sensorId = canMsg.Data[0]

						self.Values[sensorId] = float(str(canMsg.Data[1]) + str(canMsg.Data[2]))/16 # FIXME: Is this correct?

						self.Log(str(self.Values[sensorId]) + " Volts reported by sensor " + str(sensorId))

						self.CallEvent("onValueUpdate", sensorId)
					else:
						self.Log("Malformed data received")
				else:
					self.Log("Unknown command " + str(canMsg.Command))
		except KeyError:
			pass
