#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define I2C_DEVICE "/dev/i2c-1"
#define NUNCHUK_ADDRESS 0x52

void nunchuk_write(int fd, char *buffer, size_t bytes) {
  if (write(fd, buffer, bytes) != bytes) {
    perror("write error");
    exit(EXIT_FAILURE);
  }
}
void nunchuk_read(int fd, char *buffer, size_t bytes) {
  if (read(fd, buffer, bytes) != bytes) {
    perror("read error");
    exit(EXIT_FAILURE);
  }
}

int nunchuk_init() {
  int fd;
  char init_cmd[] = {0xF0, 0x55};

  fd = open(I2C_DEVICE, O_RDWR);
  if (fd < 0) {
    perror("i2c");
    exit(EXIT_FAILURE);
  }
  if (ioctl(fd, I2C_SLAVE, NUNCHUK_ADDRESS) < 0) {
    perror("Nunchuk unreachable");
    exit(EXIT_FAILURE);
  }
  nunchuk_write(fd, init_cmd, sizeof(init_cmd));
  return fd;
}

void nunchuk_close(int fd) { close(fd); }

int main(void) {
  char buffer[6], read_cmd[] = {0x00};

  int fd;
  int z, c;

  fd = nunchuk_init();

  while (1) {
    nunchuk_write(fd, read_cmd, sizeof(read_cmd));
    nunchuk_read(fd, buffer, sizeof(buffer));

    z = buffer[5] & 0x01;
    c = (buffer[5] >> 1) & 0x01;

    buffer[2] <<= 2;
    buffer[2] |= ((buffer[5] >> 2) & 0x03);
    buffer[3] <<= 2;
    buffer[3] |= ((buffer[5] >> 6) & 0x03);

    printf("X: %d\tY: %d\tAx: %d\tAy: %d\tAz: %d\t", buffer[0], buffer[1],
           buffer[2], buffer[3], buffer[4]);

    printf("Z: [");
    (z == 1) ? printf(" ]\t") : printf("X]\t");

    printf("C: [");
    (c == 1) ? printf(" ]\n") : printf("X]\n");

    usleep(100000);
  }

  return 0;
}
