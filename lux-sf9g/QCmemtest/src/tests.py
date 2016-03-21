#!/usr/bin/env python
""" LUX mem test routines"""
# SAM-BA ISP utility for LUX9 Boards  
#
# Author: David Pristovnik - david@evo-teh.com
#
# Copyright 2011 Evo-Teh d.o.o.
# http://www.evo-teh.com
# All Rights Reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import samba
import sys
import logging
import signal

class TimeoutException(Exception): 
    pass 

def timeout(timeout_time, default):
    def timeout_function(f):
        def f2(*args):
            def timeout_handler(signum, frame):
                raise TimeoutException()

            old_handler = signal.signal(signal.SIGALRM, timeout_handler) 
            signal.alarm(timeout_time) # triger alarm in timeout_time seconds
            try: 
                retval = f(*args)
            except TimeoutException:
                return default
            finally:
                signal.signal(signal.SIGALRM, old_handler) 
            signal.alarm(0)
            return retval
        return f2
    return timeout_function

def do_mem_test(devs, param):
    logging.debug("Function do_mem_test: uploads the applet into memory and executes")
    # param tuple with three parameters test type(DATA, ADDRESS), loadaddress, applet file name 
    # read memory test applet into buffer
    type = param[0]
    address = param[1]
    applet = param[2]
    logging.debug("Parameters type: %s address: %s applet: %s" % (type, address, applet))
    try:
        fd = open(applet,'rb')
    except Exception, e:
        print "Could not open applet file: %s" % applet
        logging.debug(e)
        sys.exit(3)
    buffer = fd.read()
    fd.close()
    # send SAM-BA address and code to load
    logging.debug("Sending applet binary to CPU...")
    applet_param = (address, buffer)
    samba.doSend(devs[0], applet_param)
    # run the applet
    logging.debug("Executing applet...")
    samba.doGo(devs[0], address)
    if type == 'SERIAL':
        logging.debug("Performing serial check...")
        return(parse_serial(devs[1]))
    else:
        logging.debug("Performing memory test...")
        return(parse_memory(devs[1]))

@timeout(60, False)
def parse_memory(dev):
    logging.debug("Function parse_memory: parse output of memory test")
    #parse applet output
    while True:
        line = dev.readline()
        logging.debug("---> %s" % line)
        if 'ADDRESS' in line:
            #print "We are running ADDRESS test!"
            continue
        elif 'DATA' in line:
            #print "We are running DATA test!"
            continue
        elif 'SUCCESS' in line:
            #print "Test was a SUCCESS!"
            outcome = True
            continue
        elif 'FAIL' in line:
            #print "Test was a FAILURE!"
            outcome = False
            continue
        elif 'DONE' in line:
            #print "We finished the testing"
            return(outcome)
            break
        else:
            continue

@timeout(2, False)
def parse_serial(dev):
    logging.debug("Function parse_serial: parse output of serial test")
    while True:
        line = dev.readline()
        logging.debug("---> %s" % line)
        if 'SERIAL' in line:
            return(True)
        else:
            continue