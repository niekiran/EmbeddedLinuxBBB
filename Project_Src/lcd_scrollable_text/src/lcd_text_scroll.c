/*
 ============================================================================
 Name        : lcd_scroll_text.c
 Author      : Kiran N <niekiran@gmail.com >
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : This application prints pre-stored strings in a scrollable format on the 16x2 LCD
TODOs for the students 
1) Take multiple strings as a command line argument and print in scrollable fashion instead of pre-stored. 
 ============================================================================*/
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

#include "lcd_driver.h"
#include "lcd_text_scroll.h"


/*=========================================================================================================
BBB_expansion_heade_P8_pins     GPIO number     16x2 LCD pin      Purpose 
===========================================================================================================
P8.7                             GPIO_66          4(RS)           Register selection (Character vs. Command)
P8.8                             GPIO_67          5(RW)           Read/write 
P8.9                             GPIO_69          6(EN)           Enable
P8.10                            GPIO_68          7(D4)           Data line 4
P8.11                            GPIO_45          8(D5)           Data line 5
P8.12                            GPIO_44          9(D6)           Data line 6
P8.14                            GPIO_26          10(D7)          Data line 7 
P8.16                            GPIO_46          15(BKLTA)       Backlight anode
P9.15                            GPIO_48          16(BKLTK)        Backlight cathode
============================================================================================================= */

char *some_strings[5]=
{
    "Mahatma Gandhi said,",
    "First they ignore you",
    "then they laugh at you ",
    "then they fight you",
    "then you win"
};

char buffer[250];


/*
 *  GPIO configure direction
 *  dir_value : 1 means 'out' , 0 means "in"
 */
int gpio_configure_dir(uint32_t gpio_num, uint8_t dir_value)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/direction", gpio_num);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio direction configure\n");
        return fd;
    }

    if (dir_value)
        write(fd, "out", 4); //3+1  +1 for NULL character 
    else
        write(fd, "in", 3);

    close(fd);
    return 0;
}

/*
 *  GPIO write value
 *  out_value : can be either 0 or 1
 */
int gpio_write_value(uint32_t gpio_num, uint8_t out_value)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/value", gpio_num);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio write value\n");
        return fd;
    }

    if (out_value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}

/*
 *  GPIO read value
 */
int gpio_read_value(uint32_t gpio_num)
{
    int fd;
    uint8_t read_value=0;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/value", gpio_num);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio read value\n");
        return fd;
    }

    read(fd, &read_value, 1);

    close(fd);
    return read_value;
}


/*
 *  GPIO configure the edge of trigger
 *  edge : rising, falling
 */
int gpio_configure_edge(uint32_t gpio_num, char *edge)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/edge", gpio_num);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio configure edge\n");
        return fd;
    }

    write(fd, edge, strlen(edge) + 1);
    close(fd);
    return 0;
}

/*
 *  Open the sys fs file corresponding to gpio number
 */
int gpio_file_open(uint32_t gpio_num)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/value", gpio_num);

    fd = open(buf, O_RDONLY | O_NONBLOCK );
    if (fd < 0) {
        perror("gpio file open\n");
    }
    return fd;
}

/*
 *  close a file
 */
int gpio_file_close(int fd)
{
    return close(fd);
}


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
    lcd_printf("%s - %s\n" , iface , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

    return 0;
}

/* Some silly graphics :) */
void tansition_graphics(void)
{

     sleep(1);

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

    lcd_send_command(LCD_CLEAR_DISPALY);

}


int main(int argc, char *argv[]) 
{

    printf("Application to print text in scrollable fashion on LCD\n");
    char *pstar ="**";

    initialize_all_gpios();

    gpio_write_value(GPIO_66_P8_7_RS_4,LOW_VALUE);
   
   /*The RW pin is always tied to ground in this implementation, meaning that you are only writing to
    the display and never reading from it.*/
    gpio_write_value(GPIO_67_P8_8_RW_5,LOW_VALUE);
   
   /*• The EN pin is used to tell the LCD when data is ready*/ 
    gpio_write_value(GPIO_69_P8_9_EN_6,LOW_VALUE);
   
   /*Data pins 4~7 are used for actually transmitting data, and data pins 0~3 are left unconnected*/
    gpio_write_value(GPIO_68_P8_10_D4_11,LOW_VALUE);
    gpio_write_value(GPIO_45_P8_11_D5_12,LOW_VALUE);
    gpio_write_value(GPIO_44_P8_12_D6_13,LOW_VALUE);
    gpio_write_value(GPIO_26_P8_14_D7_14,LOW_VALUE);


    /*
    You can illuminate the backlight by connecting the anode pin to 5V and the cathode pin to ground
    if you are using an LCD with a built-in resistor for the backlight. If you are not, you must put a
    current-limiting resistor in-line with the anode or cathode pin. The datasheet for your device will
    generally tell you if you need to do this. */

    lcd_init();
    lcd_send_command(LCD_CLEAR_DISPALY);
    lcd_send_command(0x80);
    lcd_send_command(LCD_CLEAR_DISPALY);
    lcd_locate(1,1);
    lcd_send_command(0x20);// this is for 4 bit 1 line
    lcd_printf("**Welcome to BBB**\n");
    sleep(1);
   //  tansition_grapphics();
    lcd_send_command(LCD_CLEAR_DISPALY);

    lcd_send_command(0x20);// this is for 4 bit 1 line
    lcd_send_command(0x07); //shift left .
    lcd_locate(1,16);
    uint8_t char_value=1;
    uint8_t n=0;
     while(1)
      {
         for(uint8_t i=0;i<2;i++)
         {

            memcpy(buffer,some_strings[i],strlen(some_strings[i])+1);
            printf("size :%d\n",strlen(some_strings[i])+1);
#if 1
            while(1)
            {

                 char_value = buffer[n++];
                 if(! char_value)
                 {
                     lcd_print_string(pstar);
                     break;
                 }
                 printf("%d\n",char_value);
                 lcd_print_char(char_value);
                 lcd_send_command(0x07);
                 usleep(300 * 1000);
            }


            n=0;
            memset(buffer,0,250);
#endif

         }
       }

return 0;

}


