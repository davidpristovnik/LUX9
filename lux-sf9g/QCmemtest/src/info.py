#!/usr/bin/env python
""" AT91 device information """
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
import logging

devices = { 
           0x28890560 : 'at91sam3s1',
           0x28990560 : 'at91sam3s1',
           0x28a90560 : 'at91sam3s1',
           0x288a0760 : 'at91sam3s2',
           0x289a0760 : 'at91sam3s2',
           0x28aa0760 : 'at91sam3s2',
           0x29540960 : 'at91sam3s4',
           0x29440960 : 'at91sam3s4',
           0x29340960 : 'at91sam3s4',
           0x28800960 : 'at91sam3s4',
           0x28900960 : 'at91sam3s4',
           0x28a00960 : 'at91sam3s4',
           0x28ab0a60 : 'at91sam3s8',
           0x289b0a60 : 'at91sam3s8',
           0x288b0a60 : 'at91sam3s8',
           0x28ac0c60 : 'at91sam3s16',
           0x289c0c60 : 'at91sam3s16',
           0x288c0c60 : 'at91sam3s16',
           0x28090560 : 'at91sam3u1',
           0x28190560 : 'at91sam3u1',
           0x280a0760 : 'at91sam3u2',
           0x281a0760 : 'at91sam3u2',
           0x28000960 : 'at91sam3u4',
           0x28100960 : 'at91sam3u4',
           0x5a455040 : 'at91sam3u4',
           0x29540960 : 'at91sam3n4',
           0x29440960 : 'at91sam3n4',
           0x29340960 : 'at91sam3n4',
           0x29590760 : 'at91sam3n2',
           0x29490760 : 'at91sam3n2',
           0x29390760 : 'at91sam3n2',
           0x29580560 : 'at91sam3n1',
           0x29480560 : 'at91sam3n1',
           0x29380560 : 'at91sam3n1',
           0x170a0940 : 'at91sam7a3',
           0x260a0940 : 'at91sam7a3',
           0x27330540 : 'at91sam7l64',
           0x27330740 : 'at91sam7l128',
           0x27280340 : 'at91sam7s321',
           0x27080340 : 'at91sam7s32',
           0x27090540 : 'at91sam7s64',
           0x270a0740 : 'at91sam7s128',
           0x270c0740 : 'at91sam7s128',
           0x270b0940 : 'at91sam7s256',
           0x271c0a40 : 'at91sam7xc512',
           0x019803a0 : 'at91sam9260',
           0x019703a0 : 'at91sam9261',
           0x019607a0 : 'at91sam9263',
           0x819903a0 : 'at91sam9g10',
           0x019903a0 : 'at91sam9g10',
           0x019905a0 : 'at91sam9g20',
           0x819b05a1 : 'at91sam9g45',
           0x819b05a0 : 'at91sam9m10',
           0x019b03a0 : 'at91sam9rl64',
           0x329993a0 : 'at91sam9xe128',
           0x329973a0 : 'at91sam9xe128',
           0x329a93a0 : 'at91sam9xe256',
           0x329a73a0 : 'at91sam9xe512',
           0x329a53a0 : 'at91sam9xe512',
           0x329aa3a0 : 'at91sam9xe512',
           0x329953a0 : 'at91sam9xe512',
           0x83770940 : 'at91cap7',
           0x039a03a0 : 'at91cap9',
           }

processor = {
             0x01 : 'ARM946ES',
             0x02 : 'ARM7TDMI',
             0x03 : 'CM3',
             0x04 : 'ARM920T',
             0x05 : 'ARM926EJS',
             }

arch = {
        0x19 : 'AT91SAM9xx',
        0x29 : 'AT91SAM9XExx',
        0x34 : 'AT91x34',
        0x37 : 'CAP7',
        0x39 : 'CAP9',
        0x3B : 'CAP11',
        0x40 : 'AT91x40',
        0x42 : 'AT91x42',
        0x55 : 'AT91x55',
        0x60 : 'AT91SAM7Axx',
        0x61 : 'AT91SAM7AQxx',
        0x63 : 'AT91x63',
        0x70 : 'AT91SAM7Sxx',
        0x71 : 'AT91SAM7XCxx',
        0x72 : 'AT91SAM7SExx',
        0x73 : 'AT91SAM7Lxx',
        0x75 : 'AT91SAM7Xxx',
        0x76 : 'AT91SAM7SLxx',
        0x80 : 'ATSAM3UxC',
        0x81 : 'ATSAM3UxE',
        0x83 : 'ATSAM3AxC',
        0x84 : 'ATSAM3XxC',
        0x85 : 'ATSAM3XxE',
        0x86 : 'ATSAM3XxG',
        0x88 : 'ATSAM3SxA',
        0x89 : 'ATSAM3SxB',
        0x8a : 'ATSAM3SxC',
        0x92 : 'AT91x92',
        0x93 : 'ATSAM3NA',
        0x94 : 'ATSAM3NB',
        0x95 : 'ATSAM3NC',
        0xf0 : 'AT75Cxx',
        }

def nvprogsize(val):
    val &= 0x0F
    return ["None", "8K", "16K", "32K", "???", "64K", "???", "128K", "???", "256K", "512K", "???", "1024K", "???", "2048K", "???"][val]

def sramsize(val):
    return ["???","1K","2K","???","112K","4K","80K","160K","8K","16K","32K","64K","128K","256K","96K","512K"][val]

def truefalse(val):
    if val:
        return "true"
    else:
        return "false"

def board_info(dev):
    logging.debug("Function board_info: board information")
    print '   SAM-BA version: %s' % (samba.doVersion(dev)).strip()
    val = samba.doReadWord(dev, 0xFFFFF240)
    print val
    val = int(val.strip(),16)
    print '   DBGU_CIDR: 0x%08X' % val
    print '   Device name: %s' % devices[(val & 0xFFFFFE0)]
    print '   Processor: %s Version: %d Non-volatile program memory: %s' \
        % (processor[(val >> 5) & 0x7], (val & 0x1F), nvprogsize(val >> 8))
    print '   Second NV program memory: %s  SRAM size: %s  Architecture: %s' \
        % (nvprogsize(val >> 12), sramsize((val >> 16) & 0xF), arch[(val >> 20) & 0xFF])
    print '   NV program type: %s  DBUG_EXID register: %s' \
        % (["ROM","ROMless/on-chip FLASH","Embedded FLASH","ROM=NVPSIZ1/FLASH=NVPSIZ2","SRAM emulating ROM","???","???","???"][(val>>28)&7], truefalse((val >> 31) & 0x1))
