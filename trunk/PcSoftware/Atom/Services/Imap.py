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
import Util
from imaplib import *

class Service (ServiceBase.ServiceBase):
	Values = {}
	
	def __init__(self, id):
		ServiceBase.ServiceBase.__init__(self, id, "Imap")
		
		self.events = {"onMailStatusUpdate":[]}

	#	self.CreateInterval(20, "CheckNewEmail", self.CheckNewEmail)

	def CheckNewEmail(self, server, username, password, mailbox):

		server = IMAP4(server)

		server.login(username, password)

		r = server.select(mailbox)

		r, data = server.search(None, "(NEW)")

		if r == "OK":
			return len(data)

		return -1




