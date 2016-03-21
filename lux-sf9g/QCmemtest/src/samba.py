#!/usr/bin/env python
"""Low-level sam-ba driver interface"""
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
import time
import logging

def readResult(dev, stopat='>'):
    logging.debug("Function SAMBA readResult")
    """Collect all data sent by SAM-BA until a '>' character is encountered."""
    s = ""
    while 1:
        c = dev.read(1)
        if len(c)==0:
            break
        if (stopat is not None) and (c==stopat):
            return s
        s += c

def checkSamBa(dev):
    logging.debug("Function SAMBA checkSamBa")
    # we have to try it few times
    for i in range(4):
        dev.write("V#")
        dev.flush()
        #if dev.readline() != None:
        if readResult(dev) != None:
            return True
    return False
    
def doVersion(dev):
    logging.debug("Function SAMBA doVersion")
    dev.write("V#")
#    return dev.readline()
    return readResult(dev)

def doReadByte(dev, address):
    logging.debug("Function SAMBA doReadByte")
    dev.write("o%x,#" % address)
    return readResult(dev)

def doReadHalfword(dev, address):
    logging.debug("Function SAMBA doReadHalfword")
    dev.write("h%x,#" % address)
    return readResult(dev)

def doReadWord(dev, address):
    logging.debug("Function SAMBA doReadWord")
    logging.debug("Writing to serial: w%x,#" % address)
    dev.write("w%x,#" % address)
    return dev.readline()
#    return readResult(dev)

def doWriteByte(dev, param):
    logging.debug("Function SAMBA doReadByte")
    dev.write("O%x,%x#" % tuple(param))
    return readResult(dev)

def doWriteHalfword(dev, param):
    logging.debug("Function SAMBA doWriteHalfword")
    dev.write("H%x,%x#" % tuple(param))
    return readResult(dev)

def doWriteWord(dev, param):
    logging.debug("Function SAMBA doWriteWord")
    dev.write("W%x,%x#" % tuple(param))
    return readResult(dev)

def doGo(dev, address):
    logging.debug("Function SAMBA doGo")
    dev.write("G%x#" % address)
    return

def doSend(dev, param):
    logging.debug("Function SAMBA doSend")
    addr = param[0]
    bytes = param[1]
    dev.write("S%x,%x#" % (addr,len(bytes)))
    dev.write(bytes)
    dev.flush()
    readResult(dev)

def doReceiveRaw(dev, addr, bytes):
    logging.debug("Function SAMBA doReceiveRaw")
    dev.write("R%x,%x#" % (addr,bytes))
    bytesRemain = bytes+2   # SAM-BA begins by sending \n\r...we discard these later
    Data = []

    while bytesRemain:
        toread = min(bytesRemain, 128)
        c = dev.read(toread)
        Data.append(c)
        bytesRemain -= len(c)
    
    s = readResult(dev)
    if len(s):
        print '%d extra characters received (not saved)' % len(s)

    # Eliminate the \n\r at the beginning
    eliminate = 2
    while eliminate > 0:
        if len(Data[0]) < eliminate:
            eliminate -= len(Data[0])
            del Data[0]
        else:
            Data[0] = Data[0][eliminate:]
            eliminate = 0
    return Data
