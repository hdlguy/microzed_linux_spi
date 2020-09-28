#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c-dev.h>
//#include <i2c/smbus.h>
#include <sys/ioctl.h>

int main(int argc,char** argv)
{
    printf("running i2c test.\n");

    int adapter_nr = 0;     // normally dynamically determined
    int addr = 0x40;        // The I2C address
    int Nbytes = 1;
    printf("bus = %d, addr = 0x%02x\n", adapter_nr, addr);

    int file;
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
    printf("opening %s\n", filename);
    file = open(filename, O_RDWR);
    if (file < 0) {
        printf("device open failed\n");
        exit(1);
    }


    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        printf("ioctl failed\n");
        exit(1);
    }

    printf("reading byte\n");
    char buf[10];
    int num_read = read(file, buf, Nbytes);
    printf("num_read = %d\n", num_read);

    for (int i=0; i<Nbytes; i++){
        printf("buf[0] = 0x%02x\n", buf[0]);
    }

    return 0;
}
