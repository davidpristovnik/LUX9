SECTION = "x11"
PE = "2"

SRC_URI = "${SOURCEFORGE_MIRROR}/mesa3d/MesaLib-${PV}.tar.bz2 \
	file://mklib-rpath-link.patch"

S = "${WORKDIR}/Mesa-${PV}"

PROVIDES = "virtual/libgl"

LICENSE = "LGPL"

INC_PR = "r3"

DEPENDS = "makedepend-native xf86vidmodeproto glproto virtual/libx11 libxext libxxf86vm libxi libxmu libice"
