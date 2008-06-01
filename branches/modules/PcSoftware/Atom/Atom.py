#!/usr/bin/env python

import Settings
import BackendCan
import ServiceManager
import ControllerManager
import Daemon
import os
import sys
import signal, os

def handler(signum, frame):
	print 'Signal handler called with signal', signum

signal.signal(signal.SIGTERM, handler)
signal.signal(signal.SIGHUP, handler)


def main():
	try:
		print "Atom service framework started"
		print "=============================="
		print "Written by Mattias Runge 2008"
		print ""

		Settings.LoadSettings()

		if Settings.Settings.has_key("DAEMON") and Settings.Settings["DAEMON"].upper() == "YES":
			print "Entering daemon mode"

			retCode = Daemon.createDaemon()

			print "Entered daemon mode"
			print ""
			print "Atom service framework started"
			print "=============================="
			print "Written by Mattias Runge 2008"
			print ""
		else:
			print "Not entering daemon mode"

 	
		ServiceManager.GetServiceManager()
		backendCan = BackendCan.GetBackendCan()
	
		controlMan = ControllerManager.GetControllerManager()
		controlMan.StartConfiguredControllers()

		backendCan.join()

	except (KeyboardInterrupt, SystemExit):
		print "User abort detected, stopping program"
		sys.exit()


if __name__ == "__main__":
	main()