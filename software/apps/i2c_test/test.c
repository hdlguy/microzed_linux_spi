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

// program to read the TI HDC1080 temperature and humidity sensor.
int main(int argc,char** argv)
{

    int i2c_bus = 0;     // normally dynamically determined
    int hdc1080_addr = 0x40;   // I2C address
    int isl29501_addr = 0x57;   // I2C address

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
    printf("\naccessing HDC1080 temperature/humidity sensor.\n");
    printf("bus = %d, addr = 0x%02x\n", i2c_bus, hdc1080_addr);
    if (ioctl(file, I2C_SLAVE, hdc1080_addr) < 0) {
        printf("ioctl failed\n");
        exit(1);
    }

    char buf[10];
    for (int i=0; i<10; i++) buf[i] = 0;
    int regval;
    double floatval;

    buf[0] = 0x00; // register address for temperature sensor
    write(file, buf, 1);
    usleep(100000); // needs delay
    read(file, buf, 2);
    regval = ((buf[0] << 8) + buf[1]);
    floatval = 165.0*regval/pow(2.0,16.0) - 40.0;
    printf("HDC1080 regval = 0x%04x, temperature = %lf\n", regval, floatval);

    buf[0] = 0x01; // register address for humidity sensor
    write(file, buf, 1);
    usleep(100000); // needs delay
    read(file, buf, 2);
    regval = ((buf[0] << 8) + buf[1]);
    floatval = 100.0*regval/pow(2.0,16.0);
    printf("HDC1080 regval = 0x%04x, relative humidity = %lf\n", regval, floatval);

    buf[0] = 0xfe; // register address for manufacturer ID
    write(file, buf, 1);
    read(file, buf, 2);
    regval = ((buf[0] << 8) + buf[1]);
    printf("HDC1080 manufacturer id = 0x%04x\n", regval);

    buf[0] = 0xff; // register address for device ID
    write(file, buf, 1);
    read(file, buf, 2);
    regval = ((buf[0] << 8) + buf[1]);
    printf("HDC1080 device id = 0x%04x\n", regval);

    
    // access the ISL29501 time of flight sensor
    // set the I2C address
    printf("\naccessing ISL29501 time of flight sensor.\n");
    printf("bus = %d, addr = 0x%02x\n", i2c_bus, isl29501_addr);
    if (ioctl(file, I2C_SLAVE, isl29501_addr) < 0) {
        printf("ioctl failed\n");
        exit(1);
    }

    buf[0] = 0x00; // register address for device ID
    write(file, buf, 1);
    read(file, buf, 1);
    regval = buf[0];
    printf("ISL29501 device id = 0x%04x\n", regval);

    buf[0] = 0x13; // register address for sample control
    write(file, buf, 1);
    read(file, buf, 1);
    regval = buf[0];
    printf("ISL29501 sample control = 0x%04x\n", regval);


    return 0;
}
