DESCRIPTION = "Opkg Package Manager"
DESCRIPTION_libopkg = "Opkg Package Manager Library"
SECTION = "base"
LICENSE = "GPLv2"
DEPENDS = "curl gpgme openssl"
SRCREV = ${OPKG_SRCREV}
PV = "0.1.6+svnr${SRCPV}"
INC_PR = "r21"

FILESPATHPKG =. "opkg:"

SRC_URI = "svn://opkg.googlecode.com/svn;module=trunk;proto=http \
	file://opkg_unarchive.patch;maxrev=201 \
	file://opkg-intercept-cleanup.patch;maxrev=241 \
"

SRC_URI += "file://configure"

S = "${WORKDIR}/trunk"

inherit autotools pkgconfig

do_install_prepend() {
  install -d ${D}${sysconfdir}/rcS.d
  install -m 0755 ${WORKDIR}/configure ${D}${sysconfdir}/rcS.d/S98configure
}
