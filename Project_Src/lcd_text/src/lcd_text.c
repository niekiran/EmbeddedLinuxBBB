/*
 ============================================================================
 Name        : 7segcnt.c
 Author      : Kiran
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 DIY: IMPLEMENT UPDOWN COUNTING
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include<stdint.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "lcd_text.h"
#include "lcd_driver.h"


/* Define 6 custom characters to display bar graph*/
char STCustom[48] =
  {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Blank
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // 1column  |
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, // 2columns ||
    0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, // 3columns |||
    0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, // 4columns ||||
    0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, // 5columns |||||
  };


/* This is the array of file descriptors for each segment of the display */
int fdgpio[8];


int gpio_configure_dir(uint32_t gpio_num, uint8_t dir_value)
{
	int fd;
	char buf[MAX_LEN];

	snprintf(buf, sizeof(buf), SYS_GPIO_DIR_PATH "/gpio%d/direction", gpio_num);
	//printf("%s\n",buf);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio direction configure\n");
		return fd;
	}

	if (dir_value)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);

	close(fd);
	return 0;
}


int gpio_write_value(uint32_t gpio_num, uint8_t out_value)
{
	int fd;
	char buf[MAX_LEN];

	snprintf(buf, sizeof(buf), SYS_GPIO_DIR_PATH "/gpio%d/value", gpio_num);
	//printf("%s\n",buf);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio write value");
		return fd;
	}

	if (out_value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);

	close(fd);
	return 0;
}


int gpio_read_value(uint32_t gpio_num)
{
	int fd;
	uint8_t read_value=0;
	char buf[MAX_LEN];

	snprintf(buf, sizeof(buf), SYS_GPIO_DIR_PATH "/gpio%d/value", gpio_num);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio read value");
		return fd;
	}

	read(fd, &read_value, 1);

	close(fd);
	return read_value;
}


int gpio_configure_edge(uint32_t gpio_num, char *edge)
{
	int fd;
	char buf[MAX_LEN];

	snprintf(buf, sizeof(buf), SYS_GPIO_DIR_PATH "/gpio%d/edge", gpio_num);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio configure edge");
		return fd;
	}

	write(fd, edge, strlen(edge) + 1);
	close(fd);
	return 0;
}


int gpio_file_open(uint32_t gpio_num)
{
	int fd;
	char buf[MAX_LEN];

	snprintf(buf, sizeof(buf), SYS_GPIO_DIR_PATH "/gpio%d/value", gpio_num);

	fd = open(buf, O_RDONLY | O_NONBLOCK );
	if (fd < 0) {
		perror("gpio file open");
	}
	return fd;
}


int gpio_file_close(int fd)
{
	return close(fd);
}



int print_ip_address()
{
	int fd;
	struct ifreq ifr;

	char iface[] = "usb0";

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;

	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	//display result
	lcd_printf("%s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

	return 0;
}


void tansition_grapphics(void)
{
	lcd_locate(1,1);
	lcd_send_command(LCD_CLEAR_DISPALY);

	for (uint8_t n =0 ; n < 2 ; n++)
	{
		for(uint8_t i=0;i<16;i++)
		{
			lcd_print_char('*');
			usleep(75*1000);

		}
		lcd_locate(2,16);
		lcd_send_command(0x04);

	}

	lcd_locate(1,1);
	lcd_send_command(0x06);
	usleep(450 * 1000);

}

int main(int argc, char *argv[]) {




	printf("This program controls dispaly text on lcd\n");

	if ( argc != 2 ) /* argc should be 3 for correct execution */
	{

		printf( "usage: %s <message>\n", argv[0] );

	}
	else
	{


	/*first configure the direction for segments */
	gpio_configure_dir(GPIO_66_P8_7_RS_4,OUT_DIR);
	gpio_configure_dir(GPIO_67_P8_8_RW_5,OUT_DIR);
	gpio_configure_dir(GPIO_69_P8_9_EN_6,OUT_DIR);
	gpio_configure_dir(GPIO_68_P8_10_D4_7,OUT_DIR);
	gpio_configure_dir(GPIO_45_P8_11_D5_8,OUT_DIR);
	gpio_configure_dir(GPIO_44_P8_12_D6_9,OUT_DIR);
	gpio_configure_dir(GPIO_26_P8_14_D7_10,OUT_DIR);


	gpio_write_value(GPIO_66_P8_7_RS_4,LOW_VALUE);
	/*The RW pin is always tied to ground in this implementation, meaning that you are only writing to
	the display and never reading from it.*/
	gpio_write_value(GPIO_67_P8_8_RW_5,LOW_VALUE);
	/*• The EN pin is used to tell the LCD when data is ready*/
	gpio_write_value(GPIO_69_P8_9_EN_6,LOW_VALUE);
	/*Data pins 4~7 are used for actually transmitting data, and data pins 0~3 are left unconnected*/
	gpio_write_value(GPIO_68_P8_10_D4_7,LOW_VALUE);
	gpio_write_value(GPIO_45_P8_11_D5_8,LOW_VALUE);
	gpio_write_value(GPIO_44_P8_12_D6_9,LOW_VALUE);
	gpio_write_value(GPIO_26_P8_14_D7_10,LOW_VALUE);



	/*
	You can illuminate the backlight by connecting the anode pin to 5V and the cathode pin to ground
	if you are using an LCD with a built-in resistor for the backlight. If you are not, you must put a
	current-limiting resistor in-line with the anode or cathode pin. The datasheet for your device will
	generally tell you if you need to do this. */

	lcd_init();
	lcd_send_command(LCD_CLEAR_DISPALY);
	lcd_send_command(CGRAM_address_start);
    lcd_send_command(LCD_CLEAR_DISPALY);
    lcd_locate(1,1);
    while(1)
    {
         lcd_printf("**Welcome to BBB**\n");
         sleep(1);
         tansition_grapphics();
         lcd_send_command(LCD_CLEAR_DISPALY);
    	 print_ip_address();
    	 sleep(1);
    	 tansition_grapphics();
    	 lcd_send_command(LCD_CLEAR_DISPALY);

    }



	}

    return 0;


}

//https://dawes.wordpress.com/2009/12/23/twitter-to-lcd/
//http://www.dinceraydin.com/lcd/commands.htm#entry
//https://mil.ufl.edu/3744/docs/lcdmanual/commands.html
