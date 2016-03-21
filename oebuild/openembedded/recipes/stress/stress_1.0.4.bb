# stress .bb build file
# Copyright (C) 2006, Advanced Micro Devices, Inc.  All Rights Reserved
# Released under the MIT license (see /COPYING)

HOMEPAGE="http://weather.ou.edu/~apw/projects/stress/"
DESCRIPTION = "a simple tool that imposes certain types of compute stress on UNIX-like operating systems."
LICENSE = "GPL"

inherit autotools

SRC_URI="http://weather.ou.edu/~apw/projects/stress/stress-${PV}.tar.gz"

SRC_URI[md5sum] = "a607afa695a511765b40993a64c6e2f4"
SRC_URI[sha256sum] = "369c997f65e8426ae8b318d4fdc8e6f07a311cfa77cc4b25dace465c582163c0"
