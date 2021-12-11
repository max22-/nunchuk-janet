#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <janet.h>

#define I2C_DEVICE "/dev/i2c-1"
#define NUNCHUK_ADDRESS 0x52

static int fd = -1;
static char buffer[6];
static int _bz = 0, _bc = 0, _jx = 0, _jy = 0, _ax = 0, _ay = 0, _az = 0;

#define check_init() { if(fd == -1) janet_panic("Nunchuk not initialized"); }

static void nunchuk_write(char *buffer, size_t bytes) {
  check_init();
  if (write(fd, buffer, bytes) != bytes) {
    janet_panic("I2C Write error");
  }
}

static void nunchuk_read(char *buffer, size_t bytes) {
  check_init();
  if (read(fd, buffer, bytes) != bytes)
    ("read error");
}

static Janet init(int32_t argc, Janet *argv) {
  char init_cmd[] = {0xF0, 0x55};

  janet_fixarity(argc, 0);

  fd = open(I2C_DEVICE, O_RDWR);
  if (fd < 0)
    janet_panic("Cannot open i2c device");

  if (ioctl(fd, I2C_SLAVE, NUNCHUK_ADDRESS) < 0)
    janet_panic("Nunchuk not found");
  
  nunchuk_write(init_cmd, sizeof(init_cmd));
  return janet_wrap_nil();
}

static Janet update(int32_t argc, Janet *argv) {
  char read_cmd[] = {0x00};

  janet_fixarity(argc, 0);
  check_init();
  nunchuk_write(read_cmd, sizeof(read_cmd));
  nunchuk_read(buffer, sizeof(buffer));

  _bz = !(buffer[5] & 0x01);
  _bc = !((buffer[5] >> 1) & 0x01);
  buffer[2] <<= 2;
  buffer[2] |= ((buffer[5] >> 2) & 0x03);
  buffer[3] <<= 2;
  buffer[3] |= ((buffer[5] >> 6) & 0x03);
  _jx = buffer[0];
  _jy = buffer[1];
  _ax = buffer[2];
  _ay = buffer[3];
  _az = buffer[4];
  
  return janet_wrap_nil();
}

static Janet bz(int32_t argc, Janet *argv)
{
  check_init();
  return janet_wrap_boolean(_bz);
}

static Janet bc(int32_t argc, Janet *argv)
{
  check_init();
  return janet_wrap_boolean(_bc);
}

static Janet jx(int32_t argc, Janet *argv)
{
  check_init();
  return janet_wrap_integer(_jx);
}

static Janet jy(int32_t argc, Janet *argv)
{
  check_init();
  return janet_wrap_integer(_jy);
}

static Janet ax(int32_t argc, Janet *argv)
{
  check_init();
  return janet_wrap_integer(_ax);
}

static Janet ay(int32_t argc, Janet *argv)
{
  check_init();
  return janet_wrap_integer(_ay);
}

static Janet az(int32_t argc, Janet *argv)
{
  check_init();
  return janet_wrap_integer(_az);
}

static Janet end(int32_t argc, Janet *argv) { if(fd != -1) close(fd); fd = -1; return janet_wrap_nil(); }

static const JanetReg cfuns[] = {
  {"init", init, "(nunchuk/init)\n\nInitializes the nunchuk."},
  {"update", update, "(nunchuk/update)\n\nReads new values from the nunchuk."},
  {"bz", bz, "(nunchuk/bz)\n\nReturns z button state."},
  {"bc", bc, "(nunchuk/bc)\n\nReturns c button state."},
  {"jx", jx, "(nunchuk/jx)\n\nReturns joystick x axis value."},
  {"jy", jy, "(nunchuk/jy)\n\nReturns joystick x axis value."},
  {"ax", ax, "(nunchuk/ax)\n\nReturns accelerometer x axis value."},
  {"ay", ay, "(nunchuk/ax)\n\nReturns accelerometer y axis value."},
  {"az", az, "(nunchuk/ax)\n\nReturns accelerometer z axis value."},
  {"end", end, "(nunchuk/end)\n\nCloses the file descriptor."},
  {NULL, NULL, NULL}
};

JANET_MODULE_ENTRY(JanetTable *env) {
  janet_cfuns(env, "nunchuk", cfuns);
}
