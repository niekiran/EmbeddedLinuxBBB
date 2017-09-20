/*
 ============================================================================
 Name        : lcd_text.c
 Author      : Kiran N < niekiran@gmail.com >
 Version     :
 Copyright   : Your copyright notice
 Description : This Application prints your message on the 16x2 LCD along with ip address of BBB, date and time
 TODOs for the students : The LCD is not displaying correct time & date .. Fix with real time details
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
#include "gpio_driver.h"

/*=========================================================================================================
BBB_expansion_header_pins       GPIO number     16x2 LCD pin      Purpose 
===========================================================================================================
P8.7                             GPIO_66          4(RS)           Register selection (Character vs. Command)
P8.8                             GPIO_67          5(RW)           Read/write 
P8.9                             GPIO_69          6(EN)           Enable
P8.10                            GPIO_68          11(D4)          Data line 4
P8.11                            GPIO_45          12(D5)          Data line 5
P8.12                            GPIO_44          13(D6)          Data line 6
P8.14                            GPIO_26          14(D7)          Data line 7 
P8.16                            GPIO_46          15(BKLTA)       Backlight anode(+)
P9.15                            GPIO_48          16(BKLTK)       Backlight cathode(-)

P9.0                              ----            1(VSS/GND)      Ground
P9.7                              ----            2(sys_VDD +5V)  +5V supply 
============================================================================================================= */



/* This function initializes all the gpios for this application
 *  TODO : Error handling implementation
 */
int initialize_all_gpios(void)
{
	/* first lets export all the GPIOs */
	gpio_export(GPIO_66_P8_7_RS_4);
	gpio_export(GPIO_67_P8_8_RW_5);
	gpio_export(GPIO_69_P8_9_EN_6);
	gpio_export(GPIO_68_P8_10_D4_11);
	gpio_export(GPIO_45_P8_11_D5_12);
	gpio_export(GPIO_44_P8_12_D6_13);
	gpio_export(GPIO_26_P8_14_D7_14);

    
   /*first configure the direction for LCD pins */
    gpio_configure_dir(GPIO_66_P8_7_RS_4,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_67_P8_8_RW_5,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_69_P8_9_EN_6,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_68_P8_10_D4_11,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_45_P8_11_D5_12,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_44_P8_12_D6_13,GPIO_DIR_OUT);
    gpio_configure_dir(GPIO_26_P8_14_D7_14,GPIO_DIR_OUT);

    return 0;

}


void print_time_and_date()
{
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

    lcd_send_command(DDRAM_FIRST_LINE_BASE_ADDR);

	lcd_printf("%d-%d-%d",1900+timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday);

   lcd_send_command(DDRAM_SECOND_LINE_BASE_ADDR);

   lcd_printf("%d:%d:%d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

}

/* This function gathers the ip address of the system and prints it on LCD */
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
    // 1. print the interface name on the first line
    lcd_send_command(DDRAM_FIRST_LINE_BASE_ADDR);
    lcd_print_string((char *)iface);
    lcd_send_command(DDRAM_SECOND_LINE_BASE_ADDR);
    lcd_print_string((char *)inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));

 //   lcd_printf("%s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

    return 0;
}

/* Some silly graphics :) */
void tansition_graphics(void)
{

	 sleep(1);

    lcd_set_cursor(1,1);
    lcd_send_command(LCD_CMD_CLEAR_DISPLAY);

    for (uint8_t n =0 ; n < 2 ; n++)
    {
        for(uint8_t i=0;i<16;i++)
        {
            lcd_print_char('*');
            usleep(75*1000);

        }
        lcd_set_cursor(2,16);
        lcd_send_command(0x04);
    }
    
    lcd_set_cursor(1,1);
    lcd_send_command(0x06);
    usleep(450 * 1000);

    lcd_send_command(LCD_CMD_CLEAR_DISPLAY);

}

int main(int argc, char *argv[])
 {
    char msg_buf[16 * 2];
    
    printf("This Application prints your message on the 16x2 LCD\n");

    if ( argc != 2 ) /* argc should be 2 for correct execution */
    {

        printf( "usage: %s <message>\n", argv[0] );

    }
    else
    {

    	initialize_all_gpios();

        gpio_write_value(GPIO_66_P8_7_RS_4,GPIO_LOW_VALUE);
       
       /*The RW pin is always tied to ground in this implementation, meaning that you are only writing to
        the display and never reading from it.*/
        gpio_write_value(GPIO_67_P8_8_RW_5,GPIO_LOW_VALUE);
       
       /*• The EN pin is used to tell the LCD when data is ready*/
        gpio_write_value(GPIO_69_P8_9_EN_6,GPIO_LOW_VALUE);
        
        /*Data pins 4~7 are used for actually transmitting data, and data pins 0~3 are left unconnected*/
        gpio_write_value(GPIO_68_P8_10_D4_11,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_45_P8_11_D5_12,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_44_P8_12_D6_13,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_26_P8_14_D7_14,GPIO_LOW_VALUE);

        /*
        You can illuminate the backlight by connecting the anode pin to 5V and the cathode pin to ground
        if you are using an LCD with a built-in resistor for the backlight. If you are not, you must put a
        current-limiting resistor in-line with the anode or cathode pin. The datasheet for your device will
        generally tell you if you need to do this. */


        lcd_init();


        lcd_send_command(LCD_CMD_CLEAR_DISPLAY);

        lcd_set_cursor(1,1);
        
        //This is the message given by user 
        strncpy(msg_buf,argv[1],32);
        
        while(1)
        {
             lcd_printf("BBB LCD Demo\n");
             tansition_graphics();
             print_ip_address();
             tansition_graphics();
             lcd_printf(msg_buf);
             tansition_graphics();
             print_time_and_date();
             tansition_graphics();
        }
    }

    return 0;
}


