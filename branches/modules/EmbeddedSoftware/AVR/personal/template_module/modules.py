#!/bin/env python

# Written by Mattias Runge 2008-05-13

import os
import sys
import getopt

moddir = '../../module'
localmoddir = 'modules'

def getModules():
	modules = []
	for fileName in os.listdir(moddir):
		if fileName[0] != '.':
			modules.append(fileName)

	return modules


def getLocalModules():
	modules = []
	for fileName in os.listdir(localmoddir):
		if fileName[0] != '.':
			modules.append(fileName)

	return modules


def parseModuleIdFromFile(moduleName, fileName):
	config_inc = open(fileName, 'r') 
	config_inc_lines = config_inc.readlines()
	config_inc.close()
	
	for line in config_inc_lines:
		line = line.strip("\n").strip(" ")
		if line.find(moduleName + "_ID") != -1:
			parts = line.split("=")
			return parts[1].strip(" ")
	
	return -1;


def getFreeModuleId(moduleName):
	takenIds = []
	
	for applicationName in os.listdir("../"):
		if applicationName[0] != '.' and os.path.exists("../" + applicationName + "/config.inc"):
			takenId = parseModuleIdFromFile(moduleName, "../" + applicationName + "/config.inc")
			
			if takenId != -1 and takenId != "<ID>":
				takenIds.append(int(takenId, 16))

	if len(takenIds) == 0:
		return "0x01"

	takenIds.sort()

	lastId = 0

	for takenId in takenIds:
		if takenId != lastId+1:
			return hex(lastId+1)

	return hex(lastId+1)
	

def compileMainFile():
	print "Compiling new main.c..."
	main_c_template = open('src/main.c.template', 'r') 
	main_c_template_lines = main_c_template.readlines()
	main_c_template.close()
	
	modules = getLocalModules()
	
	main_c = open('src/main.c', 'w') 
	
	for main_c_template_line in main_c_template_lines:
		pos_include = main_c_template_line.find('%INCLUDE')
		pos_init = main_c_template_line.find('%INIT')
		pos_process = main_c_template_line.find('%PROCESS')
		pos_list = main_c_template_line.find('%LIST')
		pos_handlemsg = main_c_template_line.find('%HANDLEMSG')
		
		if pos_include != -1:
			for moduleName in modules:
				main_c.write(main_c_template_line[:pos_include] + "#include \"../modules/" + moduleName + "/" + moduleName + ".h\"\n")
		elif pos_init != -1:
			for moduleName in modules:
				main_c.write(main_c_template_line[:pos_init] + moduleName + "_Init();\n")
		elif pos_process != -1:
			for moduleName in modules:
				main_c.write(main_c_template_line[:pos_process] + moduleName + "_Process();\n")
		elif pos_list != -1:
			count = 1
			for moduleName in modules:
				main_c.write(main_c_template_line[:pos_list] + moduleName + "_List(" + str(count) + ");\n")
				count += 1
		elif pos_handlemsg != -1:
			for moduleName in modules:
				main_c.write(main_c_template_line[:pos_handlemsg] + moduleName + "_HandleMessage(&rxMsg);\n")
		else:
			main_c.write(main_c_template_line)

	main_c.close()
	
	print "Creation of main.c complete"

def readConfigSection(fileName, sectionName):
	fileInstance = open(fileName, 'r')
	lines = fileInstance.readlines()
	fileInstance.close()
	
	sectionLines = []
	
	inSection = False
	
	for line in lines:
		if not inSection:
			if line.find("## Section " + sectionName) != -1:
				inSection = True
				sectionLines.append(line.strip("\n"))
		else:
			if line.find("## End section " + sectionName) != -1:
				inSection = False
				
			sectionLines.append(line.strip("\n"))
			
	return sectionLines
	

def updateConfigFile():
	print "Updating config.inc..."
	
	modules = getLocalModules()
	moduleSections = {}
	applicationSection = []
	
	if not os.path.exists("config.inc"):
		applicationSection = readConfigSection("src/config.inc.template", "application")
		
		for moduleName in modules:
			moduleSections[moduleName] = readConfigSection(localmoddir + "/" + moduleName + "/config.inc.template", moduleName)
	else:
		applicationSection = readConfigSection("config.inc", "application")
		
		for moduleName in modules:
			moduleSections[moduleName] = readConfigSection("config.inc", moduleName)
			
			if len(moduleSections[moduleName]) <= 1:
				moduleSections[moduleName] = readConfigSection(localmoddir + "/" + moduleName + "/config.inc.template", moduleName)
	
	timers = 0
		
	for moduleName, moduleSection in moduleSections.iteritems():
		c = 0
		for line in moduleSection:
			if line.find("<ID>") != -1:
				line = line.replace("<ID>", getFreeModuleId(moduleName))
			elif line.find("<TIMER>") != -1:
				line = line[:line.find("=")+1] + hex(timers) + " /* <TIMER> -- DO NOT REMOVE THIS COMMENT -- */"
				timers += 1
				
			moduleSection[c] = line;
			c += 1
	
	c = 0
	for line in applicationSection:
		if line.find("<TIMER>") != -1:
			line = line[:line.find("=")+1] + hex(timers) + " /* <TIMER> -- DO NOT REMOVE THIS COMMENT -- */"
			timers += 1
		elif line.find("<NUMBER_OF_TIMERS>") != -1:
			line = line.replace("<NUMBER_OF_TIMERS>", hex(timers))
		elif line.find("<NUMBER_OF_MODULES>") != -1:
			line = line.replace("<NUMBER_OF_MODULES>", hex(len(modules)))
		
		applicationSection[c] = line;
		c += 1
		
	config_inc = open('config.inc', 'w')
	
	for line in applicationSection:
		config_inc.write(line + "\n")
		
	for moduleName, moduleSection in moduleSections.iteritems():
		for line in moduleSection:
			config_inc.write(line + "\n")
		
	config_inc.close()
	
	print "Update of config.inc complete"


def compileSourcesFile():
	print "Compiling sources.inc..."
	
	uniqueSet = []
	
	sources_list_template = open('src/sources.list.template', 'r')
	sources_list_template_lines = sources_list_template.readlines()
	sources_list_template.close()
	
	for sources_list_template_line in sources_list_template_lines:
		if len(sources_list_template_line) > 0:
			uniqueSet.append(sources_list_template_line.strip("\n"))
		
	modules = getLocalModules()
	
	for moduleName in modules:
		sources_list = open(localmoddir + "/" + moduleName + "/sources.list", 'r')
		sources_list_lines = sources_list.readlines()
		sources_list.close()
	
		for sources_list_line in sources_list_lines:
			if len(sources_list_line) > 0:
				uniqueSet.append(sources_list_line.strip("\n"))
	
	sourcesString = "SOURCES = "
	
	#FIXME: Make uniqueSet actually unique
	
	for line in uniqueSet:
		sourcesString += line + " "
		
	sources_inc = open('sources.inc', 'w')
	sources_inc.write(sourcesString.strip(" ") + "\n")
	sources_inc.close()
	
	print "Creation of sources.inc complete"


def regenerateModules():
	print "Regenerating modules..."
	compileSourcesFile()
	compileMainFile()
	updateConfigFile()
	print "Regenerating modules complete"


def addModule(moduleName):
	print "Trying to add module " + moduleName
	
	try:
		os.symlink("../" + moddir + "/" + moduleName, localmoddir + "/" + moduleName)
		print "Added module successfully"
	except OSError, (errno, strerror):
		if errno == 17:
			print "A link, file or directory named " + moduleName + " is already present in the modules directory, not linking"
		else:
			raise
	
	print ""
	
	regenerateModules()


def delModule(moduleName):
	print "Trying to remove module " + moduleName
	
	try:
		os.unlink(localmoddir + "/" + moduleName)
		print "Removed module successfully"
	except OSError, (errno, strerror):
		if errno == 2:
			print "No such " + moduleName + " present in the modules directory, not unlinking"
		else:
			raise
	
	print ""
	
	regenerateModules()


def usage():
	print "Options:"
	print "\t--regenare\t\tRegenarate modules and files"
	print "\t--list\t\t\tList avalible modules"
	print "\t--help\t\t\tPrint this help"
	print "\t--add=<module name>\tAdds a module to the application"
	print ""


def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hlrd", ["help", "list", "regenerate", "add=", "del="])
	except getopt.GetoptError, err:
		# print help information and exit:
		print str(err) # will print something like "option -a not recognized"
		usage()
		sys.exit(2)
	
	if len(sys.argv) == 1:
		usage()
		sys.exit()
	
	for o, a in opts:
		if o in ("-l", "--list"):
			print getModules()
			
		elif o in ("-h", "--help"):
			usage()
			sys.exit()
			
		elif o in ("-r", "--regenerate"):
			regenerateModules()
			
		elif o in ("--add"):
			if len(a) == 0:
				print "No modulename specified"
				usage()
				sys.exit()
			
			if a not in getModules():
				print a + " is not a recognized module name"
				sys.exit()
				
			addModule(a)
			
		elif o in ("--del"):
			if len(a) == 0:
				print "No modulename specified"
				usage()
				sys.exit()
			
			if a not in getModules():
				print a + " is not a recognized module name"
				sys.exit()
				
			delModule(a)
			
		else:
			assert False, "unhandled option"


if __name__ == "__main__":
	main()

