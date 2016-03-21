#include <linux/serial.h>
#include <linux/fcntl.h>
#include <errno.h>
#include <assert.h>

/* Driver-specific ioctls: */
#define TIOCGRS485      0x542E
#define TIOCSRS485      0x542F

int main(void) {

	int fd = open ("/dev/ttyS3", O_RDWR);
	assert(fd >= 0);

	struct serial_rs485 rs485conf;

	/* Set RS485 mode: */
	rs485conf.flags |= SER_RS485_ENABLED;

        /* Set rts delay before send, if needed: */
        //rs485conf.flags |= SER_RS485_RTS_BEFORE_SEND;
        //rs485conf.delay_rts_before_send = ...;

        /* Set rts delay after send, if needed: */
        //rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;
        //rs485conf.delay_rts_after_send = ...;

        if (ioctl (fd, TIOCSRS485, &rs485conf) < 0) {
		printf("Error occured: %s\n", strerror(errno));
		return -1;
	}        

        /* Close the device when finished: */
        if (close (fd) < 0)
        assert(fd >= 0);
}
