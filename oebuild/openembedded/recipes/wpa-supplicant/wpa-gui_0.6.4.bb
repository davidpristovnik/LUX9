DESCRIPTION = "Qt interface for choosing which configured network to connect \
to. It also provides a method for browsing 802.11 SSID scan results, an event \
history log of messages generated by wpa_supplicant, and a method to add or \
edit wpa_supplicant networks."
SECTION = "network"
LICENSE = "GPL BSD"
HOMEPAGE = "http://hostap.epitest.fi/wpa_supplicant/"
RDEPENDS_${PN} = "wpa-supplicant"

SRC_URI = "http://hostap.epitest.fi/releases/wpa_supplicant-${PV}.tar.gz "

S = "${WORKDIR}/wpa_supplicant-${PV}/wpa_supplicant/wpa_gui"

inherit qt4x11
ARM_INSTRUCTION_SET = "arm"

EXTRA_QMAKEVARS_POST += "CONFIG+=thread"

do_install () {
	install -d ${D}${sbindir}
	install -m 755 wpa_gui ${D}${sbindir}
}

SRC_URI[md5sum] = "eb06a9a05d3916addf9451297a558aa2"
SRC_URI[sha256sum] = "0c10e59dd079c4e5d9ec6eebe9a8ac0e1b9e472cccef49c705f87a78391e79fa"