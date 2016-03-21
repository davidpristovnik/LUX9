#!/usr/bin/python
# QCmemtest.py
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

import serial
import info
import sys
import tests
import glob
import samba
import logging
import optparse
from xmodem import XMODEM

LOGGING_LEVELS = {'critical': logging.CRITICAL,
                  'error': logging.ERROR,
                  'warning': logging.WARNING,
                  'info': logging.INFO,
                  'debug': logging.DEBUG}

data_applet = "../images/AC-memtest-data.bin"
address_applet = "../images/AC-memtest-address.bin"
serial_applet = "../images/AC-memtest-serial.bin"
loadaddress = 0x200000 #address to copy applets and start executing
usertimeout = 0.5 # in seconds

def scan_ports():
    logging.debug("Function scan_ports: list available ttyUSB ports")
    """scan for available ports. return a list of device names."""
    return glob.glob('/dev/ttyUSB*')

def open_serial(port, usertimeout):
    logging.debug("Function open_serial: open serial port for communication")
    """try to open serial port for communication."""
    try:
        dev = serial.Serial(port=port, 
                            baudrate=115200, 
                            timeout=usertimeout,
                            parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE,
                            bytesize=serial.EIGHTBITS)
    except Exception, e:
        logging.debug(e)
        print "Could not open port: %s" %port
        sys.exit(3)
    
    dev.flushInput()
    return dev

def prog_info():
    # print program information
    print "\n   ISP utility for LUX9"
    print "   Copyright Evo-Teh d.o.o."
    print "   Version: 0.1\n"

def find_samba_device():
    logging.debug("Function find_samba_device: find samba device port")
    # scan all the serial USB devices and check if one of them is SAM-BA device
    print "   Searching for SAM-BA device..."
    systemports = scan_ports()
    logging.debug("Available ttyUSB ports: %s" % systemports)
    for port in systemports:
        try:
            dev = serial.Serial(port=port, 
                                baudrate=115200,
                                timeout=1,
                                parity=serial.PARITY_NONE,
                                stopbits=serial.STOPBITS_ONE,
                                bytesize=serial.EIGHTBITS)
        except serial.SerialException:
            continue
        if samba.checkSamBa(dev):
            logging.debug("SAM-BA device found: %s" %port)
            print "   SAM-BA device found on port: %s" % port
            dev.close()
            return port
            break
        else:
            dev.close()
    # If we can't find SAM-BA device we have to quit
    return None

def find_dbgu_port(sambaport):
    logging.debug("Function find_dbgu_device: find dbgu device port")
    # scan all the remaining serial USB devices and check if one of them is DBGU device
    print "   Searching for DBGU serial port..."
    dev = open_serial(sambaport,usertimeout)
    systemports = scan_ports()
    systemports.remove(sambaport)
    logging.debug("Available ttyUSB ports: %s" % systemports)
    
    for port in systemports:
        try:
            #print "checking port: %s" % port
            dbgu = serial.Serial(port=port, 
                                baudrate=115200,
                                timeout=1,
                                parity=serial.PARITY_NONE,
                                stopbits=serial.STOPBITS_ONE,
                                bytesize=serial.EIGHTBITS)
        except serial.SerialException:
            continue
        devs = (dev, dbgu)
        if tests.do_mem_test(devs, ('SERIAL',loadaddress, serial_applet) ):
            logging.debug("DBGU serial port found: %s" % port)
            print "   DBGU serial port found: %s" % port
            dbgu.close()
            return port
            break
        else:
            dbgu.close()
            continue
    # If we can't find DBGU port we have to quit
    return None

#def getc(size, timeout=1):
#    data = dev.read(size)
#    return data
         
#def putc(data, timeout=1):
#    dev.write(data)
#    return len(data)
                 
#x = XMODEM(getc, putc)

def main():
    prog_info()
    
    sambaport = find_samba_device()
    logging.debug("SAM-BA port: %s", sambaport)
    if sambaport == None:
        print "   SAM-BA port not found! Please reset the board and try again."
        return 1
    
#    dbguport = find_dbgu_port(sambaport)
#    if dbguport == None:
#        print "   DBGU port not found! Please reset the board and try again."
#        return 2

    dev = open_serial(sambaport,usertimeout)
#    dbgu = open_serial(dbguport,usertimeout)
    
    def getc(size, timeout=1):
        data = dev.read(size)
        return data

    def putc(data, timeout=1):
        dev.write(data)
        return len(data)

    modem = XMODEM(getc, putc)

    print "   --- BOARD info ---"
    info.board_info(dev)
    print "   --- Run memory tests ---"
    stream = open(data_applet, 'rb')
    logging.debug("Sending applet binary to CPU...")
    dev.write("S%x,#" % loadaddress)
    modem.send(stream)
    stream.close()
    #applet_param = (loadaddress, buffer)
    #samba.doSend(dev, applet_param)
    logging.debug("Executing applet...")
    samba.doGo(dev, loadaddress)

#    devs = (dev, dbgu)
#    print "   DATA memory test ..."
#    if tests.do_mem_test(devs, ('DATA',loadaddress, data_applet) ):
#        print "   DATA test: SUCCESS"
#    else:
#        print "   DATA test: FAILURE"
#        return -1
#    print "   ADDRESS memory test ..."
#    if tests.do_mem_test(devs, ('ADDRESS',loadaddress, address_applet) ):
#        print "   ADDRESS test: SUCCESS"
#    else:
#        print "   ADDRESS test: FAILUE"
#        return -2
#    print "   --- Memory tests finished ---"
    dev.close()
#    dbgu.close()
    return 0
    
if __name__ == "__main__":
    
    parser = optparse.OptionParser()
    parser.add_option('-l', '--logging-level', help='Logging level')
    parser.add_option('-f', '--logging-file', help='Logging file name')
    (options, args) = parser.parse_args()

    logging_level = LOGGING_LEVELS.get(options.logging_level, logging.INFO)
    logging.basicConfig(level=logging_level, filename=options.logging_file, \
                        format='%(asctime)s %(levelname)s: %(message)s', \
                        datefmt='%d.%m.%Y %H:%M:%S')

    try:
        sys.exit(main())
    except:
        e = sys.exc_info()
        logging.debug(e)
