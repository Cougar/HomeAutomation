
import ControllerBase
import time
import datetime

class Controller (ControllerBase.ControllerBase):
	
	def __init__(self, settings):
		ControllerBase.ControllerBase.__init__(self, settings)

		self.serviceLcd = self.serviceManager.AddService(self.Settings["LCD_ID"], "LCD")
		self.serviceSensor = self.serviceManager.AddService(self.Settings["SENSOR_ID"], "Sensor")
		self.serviceSensor.RegisterEventHandler("onValueUpdate", self.onValueUpdateHandler)

	#	self.serviceImap = self.serviceManager.AddService("1", "Imap")
	#	self.serviceImap.RegisterIntervalHandler("onNewMail", 10, ["server", "user", "pass", "Inbox"], self.onNewMailHandler)

	#def onNewMailHandler(self, data):
	#	print "You have " + str(self.serviceImap.CheckNewEmail("server", "user", "pass", "Inbox")) + " new emails"
	
	def onValueUpdateHandler(self, data):
		self.serviceLcd.SetCursorPosition(0, 0)
		self.serviceLcd.Print(str(self.serviceSensor.Values[data]) + chr(223) + "C" + "\n")
		#self.serviceLcd.SetCursorPosition(0, 1)
		self.serviceLcd.Print(str(datetime.date.today()) + " " + time.strftime("%H:%M:%S", time.localtime()))
