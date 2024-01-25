DESCRIPTION = "char driver "
SECTION = "examples"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://ioctl_userapp.c \
           file://chardriver_userapp.c \
           file://chardrivermuldevice_userapp.c \
          "
S = "${WORKDIR}"

do_compile() {
	${CC} ${LDFLAGS} ioctl_userapp.c -o ioctl_userapp
	${CC} ${LDFLAGS} chardriver_userapp.c -o chardriver_userapp
	${CC} ${LDFLAGS} chardrivermuldevice_userapp.c -o chardrivermuldevice_userapp
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ioctl_userapp ${D}${bindir}
	install -m 0755 chardriver_userapp ${D}${bindir}
	install -m 0755 chardrivermuldevice_userapp ${D}${bindir}
}
FILES:${PN} += "${bindir}/ioctl_userapp ${bindir}/chardriver_userapp ${bindir}/chardrivermuldevice_userapp"