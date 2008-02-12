###########################################################
#
# Daemon configuration
#
###########################################################
import logging as log
import pickle
import hashlib
import os
import imp
import re
from ConfigParser import ConfigParser
import filters
import statespaces


class DynamicModuleCfg:
    """Dynamic module manager"""

    modSubDir = None
    CHECKSUM_FILE = 'checksums'
    CHECKSUM_SECTION = 'SHA-1_CHECKSUMS'
    
    def __init__(self, modSubDir):
        self.modSubDir = modSubDir
        self.checkSumFile = modSubDir + '/' + self.CHECKSUM_FILE
    
    
    def loadModule(self, className):
        pFileName = os.getcwd() + '/' + self.modSubDir + '/' + className + '.p'
        log.debug('Reading from ' + pFileName)
        fp = open(pFileName, 'r')
        p = pickle.Unpickler(fp)
        loadedMod = p.load()
        fp.close()
        if not self.__verifyCheckSum(className, pFileName):
            print 'Invalid checksum'
            return None
        else:
            return loadedMod
        
        
    def loadModules(self, config = None):
        subDirFiles = os.listdir(os.getcwd() + '/' + self.modSubDir)
        p = re.compile('.p$', re.IGNORECASE)
        modFiles = [elem for elem in subDirFiles if p.search(elem) is not None]
        modNames = []
        for mfName in modFiles:
            modNames.append(mfName.split('.')[0])
        log.debug('Found modules: ' + str(modNames))
        myMods = {}
        for mName in modNames:
            classObj = self.loadModule(mName)
            if classObj is not None:
                myMods[mName] = classObj
        return myMods
  
    
    def saveModule(self, className, classObj):
        pFileName = os.getcwd() + '/' + self.modSubDir + '/' + className + '.p'
        log.debug('Writing to ' + pFileName)
        fp = open(pFileName, 'w')
        p = pickle.Pickler(fp)
        p.dump(classObj)
        fp.close()
        self.__writeCheckSum(className, pFileName)
    
    
    def __doCheckSum(self, filename):
        f = open(filename, 'r')
        data = f.read()
        f.close()
        hasher = hashlib.sha1()
        hasher.update(data)
        digest = hasher.hexdigest()
        return digest
    
    
    def __writeCheckSum(self, entryname, filename):
        csumsfile = open(self.checkSumFile, 'r+')
        csumcfg = ConfigParser()
        csumcfg.readfp(csumsfile)
        digest = self.__doCheckSum(filename)
        csumcfg.set(self.CHECKSUM_SECTION, entryname, digest)
        csumsfile.seek(0)
        csumcfg.write(csumsfile)
        
        
    def __verifyCheckSum(self, entryname, filename):
        csumsfile = open(self.checkSumFile, 'r')
        csumcfg = ConfigParser()
        csumcfg.readfp(csumsfile)
        digest = csumcfg.get(self.CHECKSUM_SECTION, entryname)
        cur_digest = self.__doCheckSum(filename)
        log.debug('Old checksum ' + digest + '\nNew checksum: ' + cur_digest)
        if cur_digest == digest:
            return True
        return False
    
    
    def importModule(self, className, requiredAttributes):
        """Imports a given class from the module with the same name """

        codeFile = os.getcwd() + '/' + self.modSubDir + '/' + className + '.py'
        mod = imp.load_source(self.modSubDir + '.' + className, codeFile)
        newClass = eval('mod.' + className)()
        
        for ra in requiredAttributes:
            if not hasattr(newClass, ra):
                print className, ': missing required symbol \"' + ra + '\"'
                return None 
        
        self.saveModule(className, newClass)
        return newClass


class FilterCfg:
    """Filter resource manager"""
    
    REQUIRED_ATTRIBUTES = ['ASSOCIATED_SPACES', 'DESCRIPTIVE_NAME',
                           'attach', 'detach', 'filter']
    
    dynamicModuleCfg = None
    FILTER_SUBDIR = 'filters'
    filterModules = {}
    
    def __init__(self):
        self.dynamicModuleCfg = DynamicModuleCfg(self.FILTER_SUBDIR)
    
    def loadFilter(self, name):
        newFilt = self.dynamicModuleCfg.loadModule(name)
        if newFilt is not None:
            filterModules[name] = newFilt
            return True
        else:
            return False

    def loadFilters(self, config = None):
        self.filterModules = self.dynamicModuleCfg.loadModules()
        for fM in self.filterModules:
            log.debug(fM)

    def __saveFilter(self, className, classObj):
        self.dynamicModuleCfg.saveModule(className, classObj)

    def saveFilters(self):
        for className in self.filterModules:
            self.__saveFilter(className, self.filterModules[className])
        
    def importFilter(self, className):
        """Imports a given filter definition """
        classObj = self.dynamicModuleCfg.importModule(className, self.REQUIRED_ATTRIBUTES)
        if classObj is not None:
            print 'Imported: ' + classObj.DESCRIPTIVE_NAME
            return True
        else:
            return False
        

class StateSpaceCfg:
    """State space resource manager"""
    
    REQUIRED_ATTRIBUTES = ['DESCRIPTIVE_NAME',
                           'load', 'reset', 'run', 'unload']
    
    dynamicModuleCfg = None
    FILTER_SUBDIR = 'statespaces'
    spaceModules = {}
    
    def __init__(self):
        self.dynamicModuleCfg = DynamicModuleCfg(self.FILTER_SUBDIR)
    
    def loadSpace(self, name):
        newSpace = self.dynamicModuleCfg.loadModule(name)
        if newSpace is not None:
            spaceModules[name] = newFilt
            return True
        else:
            return False

    def loadSpaces(self, config = None):
        self.spaceModules = self.dynamicModuleCfg.loadModules()
        for sM in self.spaceModules:
            log.debug(sM)

    def __saveSpace(self, className, classObj):
        self.dynamicModuleCfg.saveModule(className, classObj)

    def saveSpaces(self):
        for className in self.spaceModules:
            self.__saveSpace(className, self.spaceModules[className])
    
    def importSpace(self, name):
        """Imports a given state space definition file """
        classObj = self.dynamicModuleCfg.importModule(name, self.REQUIRED_ATTRIBUTES)
        if classObj is not None:
            print 'Imported: ' + classObj.DESCRIPTIVE_NAME
            return True
        else:
            return False

class DaemonConfig:
    
    stateSpaceCfg = None
    filterCfg = None
    filterChain = []
    
    def __init__ (self):
        self.filterCfg = FilterCfg()
        self.stateSpaceCfg = StateSpaceCfg()
        
    def setupFilterChain(self):
        self.filterChain = ['DefaultFilter']
    
    def setupFilterBindings(self):
        for f in self.filterCfg.filterModules.values():
            assocSpaces = []
            for s in f.ASSOCIATED_SPACES:
                if self.stateSpaceCfg.spaceModules.has_key(s):
                    assocSpaces.append(self.stateSpaceCfg.spaceModules[s])
                else:
                    print 'WARNING: Reference to undefined state space: ' + s
            setattr(f, '__ASSOCIATED_SPACES__', assocSpaces)
    
