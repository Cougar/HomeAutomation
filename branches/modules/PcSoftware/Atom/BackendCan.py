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
import asyncore
import string
import socket
import Queue
import threading
import time
import ServiceManager
import CanModuleTypes
import Util
import Settings

gBackendCan = False

def GetBackendCan():
	global gBackendCan
	if gBackendCan == False:
		gBackendCan = BackendCan(Settings.Settings["CANDAEMON_ADDRESS"], int(Settings.Settings["CANDAEMON_PORT"]))
	return gBackendCan



def ConvertHardwareIdToString(hwid):
	return str("%X" % hwid[0]).zfill(2) + str("%X" % hwid[1]).zfill(2) + str("%X" % hwid[2]).zfill(2) + str("%X" % hwid[3]).zfill(2)

class CanMessage:
	def __init__(self):
		self.ClassName = 0
		self.Direction = 0
		self.ModuleType = 0
		self.ModuleId = 0
		self.Command = 0
		self.Data = []
		
	def Parse(self, msg):
		parts = msg.split(' ')

		id_bits = Util.hex2bin(parts[1]);
		
		self.ClassName = int(id_bits[4:7], 2)
		self.Direction = int(id_bits[7:8], 2)
		self.ModuleType = int(id_bits[8:16], 2)
		self.ModuleId = int(id_bits[16:24], 2)
		self.Command = int(id_bits[24:32], 2)

		count = 4
		while count < len(parts):
			self.Data.append(int(Util.hex2bin(parts[count]), 2))
			count += 1
			
	def Compile(self):
		bClassName = Util.int2bin(int(self.ClassName)).zfill(4)
		bDirection = Util.int2bin(int(self.Direction)).zfill(1)
		bModuleType = Util.int2bin(int(self.ModuleType)).zfill(8)
		bModuleId = Util.int2bin(int(self.ModuleId)).zfill(8)
		bCommand = Util.int2bin(int(self.Command)).zfill(8)
		
		id_bits = "000" + bClassName + bDirection + bModuleType + bModuleId + bCommand
		
		id = Util.bin2hex(id_bits)
		
		bytes = ""
		for byte in self.Data:
			bytes += Util.bin2hex(Util.int2bin(byte).zfill(8)) + " "
		
		cmd = "PKT " + id + " 1 0 " + bytes
		return cmd.strip(" ")

class CanNode ():
	HardwareId = [0,0,0,0]
	LastOnline = 0
	Services = {}
	
	def __init__(self, hwid):
		self.Services = {}
		self.HardwareId = hwid
		self.UpdateLastOnline()
		self.canBackend = GetBackendCan()
		self.serviceMan = ServiceManager.GetServiceManager()
		self.AskForModules()
	
	def AskForModules(self):
		canMsg = CanMessage()
		canMsg.ClassName = 11 # 0x0B CAN_CLASS_MODULE_NMT
		canMsg.ModuleType = 0
		canMsg.ModuleId = 0
		canMsg.Direction = 0
		canMsg.Command = 0 # 0x00 CAN_CMD_MODULE_NMT_LIST
		
		canMsg.Data = self.HardwareId
		
		print "CAN: " + self.GetHardwareIdString() + ": Asking for modules..."
		
		self.canBackend.QueueMsg(canMsg.Compile())
	
	def AddModule(self, ModuleType, ModuleId):

		try:
			serviceType = CanModuleTypes.Types[ModuleType]
			serviceId = "CAN_" + str(ModuleType) + "_" + str(ModuleId)

			if not self.serviceMan.HasService(serviceId, serviceType):
				self.serviceMan.AddService(serviceId, serviceType)
				
				print "CAN: " + self.GetHardwareIdString() + ": Added " + serviceType + " service with module id " + hex(ModuleId)
			else:
				print "CAN: " + self.GetHardwareIdString() + ": Service " + serviceType + " already started, module id " + hex(ModuleId)

			service = self.serviceMan.GetService(serviceId, serviceType)
			
			service.ModuleType = ModuleType
			service.ModuleId = ModuleId
			service.SetOnline()

			self.Services[str(ModuleType) + "_" + str(ModuleId)] = service;
		except (KeyboardInterrupt, SystemExit):
			raise
		except KeyError:
			print "CAN: " + self.GetHardwareIdString() + ": Unknown module with id " + hex(ModuleId) + ", type " + hex(ModuleType)
			
	
	def UpdateLastOnline(self):
		self.LastOnline = time.time()
		for key in self.Services:
			self.Services[key].SetOnline()

	def CheckOnline(self):
		if self.LastOnline+20 < time.time():
			self.SetServicesOffline()

	def SetServicesOffline(self):
		print "CAN: " + self.GetHardwareIdString() + ": Node is offline, stopping services"
		for key in self.Services:
			self.Services[key].SetOffline()

	def GetHardwareIdString(self):
		return ConvertHardwareIdToString(self.HardwareId)
		


class BackendCan (asyncore.dispatcher, threading.Thread):
	
	nodes = {}
	msgCallbacks = []
	
	def __init__(self, serverHost, serverPort):
		self.nodes = {}
		self.msgCallbacks = []

		threading.Thread.__init__(self)
		
		asyncore.dispatcher.__init__(self)
		
		self.serviceMan = ServiceManager.GetServiceManager()
		
		self.nodesLock = threading.Lock()

		self.queue = Queue.Queue()
		self.queueLock = threading.Lock()
		self.connected = False
		
		self.serverHost = serverHost;
		self.serverPort = serverPort;

		self.timer = threading.Timer(20.0, self.CheckNodes)

		self.Connect()

		self.setDaemon(True)

		self.start()

	def Connect(self):
		try:
			print "CAN: Trying to connect to canDaemon at " + self.serverHost + ":" + str(self.serverPort)

			self.create_socket(socket.AF_INET, socket.SOCK_STREAM)

			self.connect((self.serverHost, self.serverPort))
		
		except (KeyboardInterrupt, SystemExit):
			raise
		except socket.gaierror, e:
			print "CAN: Address-related error connecting to server: %s" % e
			self.handle_close()
		except socket.error, e:
			print "CAN: Connection error: %s" % e
			self.handle_close()

	def CheckNodes(self):
		try:
			print "CAN: Checking if nodes are offline"

			self.nodesLock.acquire()

			for key in self.nodes:
				self.nodes[key].CheckOnline()

			self.nodesLock.release()

			self.StartTimer()
		except (KeyboardInterrupt, SystemExit):
			raise

	def SetServicesOffline(self):
		self.nodesLock.acquire()

		for key in self.nodes:
			self.nodes[key].SetServicesOffline()

		self.nodesLock.release()

	def StartTimer(self):
		
		self.timer.start()

	def handle_connect(self):
		pass

	def handle_close(self):
		self.connected = False
		print "CAN: " + "Connection to canDaemon closed"
		self.close()
		self.SetServicesOffline()
		self.timer.cancel()

		print "CAN: Waiting for 10 seconds..."
		timer = threading.Timer(10.0, self.Connect)
		timer.start()
		timer.join()
		
	def run(self):
		#self.log('run')
		asyncore.loop(1)
		
	def writable(self):
		self.queueLock.acquire()
		state = not self.queue.empty()
		#self.log("writable: " + str(state))
		self.queueLock.release()
		return state
	
	def handle_write(self):
		self.queueLock.acquire()
		
		while not self.queue.empty():
			msg = self.queue.get()
			self.send(msg + "\n")
			#self.log("SENT: " + msg)
			self.queue.task_done()
			time.sleep(0.0001)
			
		self.queueLock.release()
		
	def handle_read(self):
		try:
			more = self.recv(1024)

			if not self.connected:
				self.connected = True
				print "CAN: " + "Connected to canDaemon"

				self.StartTimer()

			if not more:
				self.handle_close()
			
			recvList = more.split("\n")
		
			for msg in recvList:
				self.handle_msg(msg)

		except (KeyboardInterrupt, SystemExit):
			raise
		except socket.error, e:
			print "CAN: Error receiving data: %s" % e
			self.handle_close()
			
	def handle_msg(self, msg):
		if len(msg) > 0:
			#self.log("RECV: " + msg)
			
			canMsg = CanMessage()
			canMsg.Parse(msg)
			
			if canMsg.ClassName == 0: # 0x00 CAN_NMT
				if canMsg.ModuleType == 44: # 0x2C CAN_NMT_HEARTBEAT
					self.ProcessHeartbeat(canMsg.Data)
			else:
				if canMsg.Command == 00: # 0x00 CAN_CMD_MODULE_NMT_LIST
					self.ProcessModuleList(canMsg)
				else:
					for callback in self.msgCallbacks:
						callback(canMsg)
			
	def ProcessModuleList(self, canMsg):
		HardwareIdString = ConvertHardwareIdToString(canMsg.Data)
		
		self.nodesLock.acquire()

		if self.nodes.has_key(HardwareIdString):
			#print "CAN: " + "Received module info from " + HardwareIdString
			self.nodes[HardwareIdString].AddModule(canMsg.ModuleType, canMsg.ModuleId)
		else:
			print "CAN: " + "Received module info from " + HardwareIdString + " but that node is not online, ignoring"

		self.nodesLock.release()
	
	def ProcessHeartbeat(self, HardwareId):
		HardwareIdString = ConvertHardwareIdToString(HardwareId)
		
		print "CAN: " + HardwareIdString + ": Received heartbeat"

		self.nodesLock.acquire()

		if self.nodes.has_key(HardwareIdString):
			self.nodes[HardwareIdString].UpdateLastOnline()
		else:
			self.nodes[HardwareIdString] = CanNode(HardwareId)

		self.nodesLock.release()
	
	def QueueMsg(self, msg):
		if self.connected:
			self.queueLock.acquire()
			self.queue.put(msg)
			self.queueLock.release()
			self.handle_write()
		else:
			self.queueLock.acquire()
			self.queue.put(msg)
			self.queueLock.release()
		
	def AddCallback(self, callback):
		self.msgCallbacks.append(callback)
