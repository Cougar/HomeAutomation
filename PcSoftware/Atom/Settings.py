
import os

Settings = {}

def FindConfig():
	filename = "/etc/atom.conf"

	if os.path.exists(filename):
		return filename

	filename = "./atom.conf"

	if os.path.exists(filename):
		return filename

	print "No config file found"
	return False

def LoadSettings():
	filename = FindConfig()

	if filename != False:
		settingsFile = open(filename, 'r')
		lines = settingsFile.readlines()
		settingsFile.close()
	
		for line in lines:
			line = line.strip("\n").strip(" ")
			if len(line) > 0:
				if line[0] != '#':
					settingName = line[:line.find("=")]
					settingValue = line[line.find("=")+1:]

					Settings[settingName] = settingValue 

		print "Loaded settings from " + filename

	print ""
