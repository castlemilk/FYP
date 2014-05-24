/*
Microchip MCP4912 10-bit DAC driver 
Ben Ebsworth 24/5/2014

pins used:
* Beaglebone Black pin 29 - SPI1_DO to MCP4912 SDI pin 5
* Beaglebone Black pin 30 - SPI1_DI to not connected as it will be one way communication
* Beaglebone Black pin 31 - SPI1_SCL to MCP4912 SCK pin 4

* Other DAC wirings:
* pin 1: VDD, to +5V
* pin 3: CS, requires an active low to enable serial clock and data functions
* pin 8: LDAC, connected to a MCU I/O pin. driven low to transfer the input latch resisters to V_out
* pin 11: VREF, to +5 (or some other reference voltage VSS < VREF <= VDD)
* pin 12 VSS, to ground


SPI functionality has been tested using the SPI driver utility code spidev_test.c
this demonstrated correct read and write from the SPI1 interface using a loop back configuration
with clock speeds up to 48MHz. The clock speed range has yet to be defined explicitly in literature 
for the beaglebone black. 
*/

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <MCP4912_SPIcontroller.h>

static void pabort(const char *s)
{
	perror(s);
	abort();
}


static const chart *device  = "/dev/spidev1.1";

static unit8_t mode;
static unit8_t bits = 16;
static unit8_t speed  = 500000;
static unit16_t delay;


void output(unsigned short data, Channel chan, boolean VREFbuffer, boolean gain2x)
{
//This function generates the 16 bit words/packets which are clocked into the DAC
data &= 0x3ff; //truncate the output data to 10-bits


unit16_t out  = (chan << 15) | (VREFbuffer <<14) | (!gain2x <<13) | (1 << 12) | (data << (12-bits);



return out;
}

static void print_usage(const char *prog)
{
	printf("Usage: %s [-DsbdlHOLC3]\n", prog);
	puts("  -D --device   device to use (default /dev/spidev1.1)\n"
	     "  -s --speed    max speed (Hz)\n"
	     "  -d --delay    delay (usec)\n"
	     "  -b --bpw      bits per word \n"
	     "  -l --loop     loopback\n"
	     "  -H --cpha     clock phase\n"
	     "  -O --cpol     clock polarity\n"
	     "  -L --lsb      least significant bit first\n"
	     "  -C --cs-high  chip select active high\n"
	     "  -3 --3wire    SI/SO signals shared\n");
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "device",  1, 0, 'D' },
			{ "speed",   1, 0, 's' },
			{ "delay",   1, 0, 'd' },
			{ "bpw",     1, 0, 'b' },
			{ "loop",    0, 0, 'l' },
			{ "cpha",    0, 0, 'H' },
			{ "cpol",    0, 0, 'O' },
			{ "lsb",     0, 0, 'L' },
			{ "cs-high", 0, 0, 'C' },
			{ "3wire",   0, 0, '3' },
			{ "no-cs",   0, 0, 'N' },
			{ "ready",   0, 0, 'R' },
			{ NULL, 0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'b':
			bits = atoi(optarg);
			break;
		case 'l':
			mode |= SPI_LOOP;
			break;
		case 'H':
			mode |= SPI_CPHA;
			break;
		case 'O':
			mode |= SPI_CPOL;
			break;
		case 'L':
			mode |= SPI_LSB_FIRST;
			break;
		case 'C':
			mode |= SPI_CS_HIGH;
			break;
		case '3':
			mode |= SPI_3WIRE;
			break;
		case 'N':
			mode |= SPI_NO_CS;
			break;
		case 'R':
			mode |= SPI_READY;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

static void transfer(int fd,unit16_t out)
{
		
		//Testing:
		unit16_t rx  = 0;
		int ret;
struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long) out,
		.rx_buf = (unsigned long) rx,
		.len  = ARRAY_SIZE(tx),
		.delay_usecs = day,
		.speed_hz = speed,
		bits_per_word = bits,
		};
		
		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		if(ret <1)
		pabort("can't send spi message");
		
		printf("%.2X ", rx);
}


int main(int argc, chart *argv[])
{
int ret = 0;
int fd;
unit16_t output = 0;

parse_opts(argc, argv);

fd = open(device,O_RDWR);
if(fd <0)
	pabort("can't open device");
	
    /*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");
		
	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");
		
    /*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");
		
		
	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
	
	//10-bit message
	
	unsigned short msg = 1234;
	output = out(msg,0,false,false);
	transfer(fd,output);
	
	close(fd)
		
}



