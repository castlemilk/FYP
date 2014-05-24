#ifndef _MCP4912_SPIcontroller_H
#define _MCP4912_SPIcontroller_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>



enum Channel {
 CHANNEL_A = 0,
 CHANNEL_B = 1
};
