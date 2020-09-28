#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <math.h>

// program to read the TI HDC108 temperature and humidity sensor.
int main(int argc,char** argv)
{
    printf("running i2c test.\n");

    int i2c_bus = 0;     // normally dynamically determined
    int addr = 0x40;        // The I2C address
    printf("bus = %d, addr = 0x%02x\n", i2c_bus, addr);

    int file;
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", i2c_bus);
    printf("opening %s\n", filename);

    // open the I2C bus controller
    file = open(filename, O_RDWR);
    if (file < 0) {
        printf("device open failed\n");
        exit(1);
    }

    // set the I2C address
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("ioctl failed\n");
        exit(1);
    }

    char buf[10];
    for (int i=0; i<10; i++) buf[i] = 0;
    int regval;
    double floatval;

    buf[0] = 0x00; // register address for temperature sensor
    write(file, buf, 1);
    usleep(100000);
    read(file, buf, 2);
    regval = ((buf[0] << 8) + buf[1]);
    floatval = 165.0*regval/pow(2.0,16.0) - 40.0;
    printf("regval = 0x%08x, temperature = %lf\n", regval, floatval);

    buf[0] = 0x01; // register address for humidity sensor
    write(file, buf, 1);
    usleep(100000);
    read(file, buf, 2);
    regval = ((buf[0] << 8) + buf[1]);
    floatval = 100.0*regval/pow(2.0,16.0);
    printf("regval = 0x%08x, relative humidity = %lf\n", regval, floatval);

    return 0;
}
