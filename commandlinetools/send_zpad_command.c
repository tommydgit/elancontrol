//  Bit-bang Elan S6 Command Seuquence on GPI10
//  Uses: How to access GPIO registers from C-code on the Raspberry-Pi
//  Dom and Gert

#define BCM2708_PERI_BASE        0x3F000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include "send_zpad_command.h"

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

int send_command(int code,int channel)
{
    int g,rep;
    // Set GPIO channel to output
    INP_GPIO(channel); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(channel);
    for (rep=0; rep<5; rep++)
    {
        GPIO_SET = 1<<channel;
        g=0;
        while (g<906)
        {
            g++;
        }
        GPIO_CLR = 1<<channel;
        nanosleep((const struct timespec[]){{0,5000000L}}, NULL);
    }
    for (rep=0; rep<6; rep++)
    {
        GPIO_SET = 1<<channel;
        g=0;
        while (g<906)
        {
            g++;
        }
        GPIO_CLR = 1<<channel;
        if (code & 1<<5-rep)
        {
            nanosleep((const struct timespec[]){{0,7000000L}}, NULL);
        }
        else
        {
            nanosleep((const struct timespec[]){{0,5000000L}}, NULL);
        }
    }
    GPIO_SET = 1<<channel;
    g=0;
    while (g<906)
    {
        g++;
    }
    GPIO_CLR = 1<<channel;
    return 0;
} // main
//
// Set up a memory regions to access GPIO
//
void setup_io()
{
    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        printf("can't open /dev/mem \n");
        exit(-1);
    }
    /* mmap GPIO */
    gpio_map = mmap(
            NULL,             //Any adddress in our space will do
            BLOCK_SIZE,       //Map length
            PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
            MAP_SHARED,       //Shared with other processes
            mem_fd,           //File to map
            GPIO_BASE         //Offset to GPIO peripheral
            );
    close(mem_fd); //No need to keep mem_fd open after mmap
    if (gpio_map == MAP_FAILED) {
        printf("mmap error %d\n", (int)gpio_map);//errno also set!
        exit(-1);
    }
    // Always use volatile pointer!
    gpio = (volatile unsigned *)gpio_map;
} // setup_io
