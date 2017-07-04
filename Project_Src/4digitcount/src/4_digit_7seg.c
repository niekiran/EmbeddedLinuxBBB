/*
 ============================================================================
 Name        : 4_digit_7seg.c
 Author      : Kiran N < niekiran@gmail.com >
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : This application implements a 4 digit up/down/random counter on 4 dgit 7seg LED display
 TODOs for the studnets 
 1)Convert this application to implement Digital clock. 
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

/*==========================================================================================
BBB_expansion_header_P8_pins           GPIO number            4-digit 7seg Dispaly pin number
=============================================================================================
P8.7                                   GPIO_66                     11
P8.8                                   GPIO_67                     7
P8.9                                   GPIO_69                     4
P8.10                                  GPIO_68                     3
P8.11                                  GPIO_45                     2
P8.12                                  GPIO_44                     1
P8.14                                  GPIO_26                     10
P8.16                                  GPIO_46                     5
============================================================================================ */


/*==========================================================================================
BBB_expansion_header_P9_pins           GPIO number            4-digit 7seg Dispaly pin number
=============================================================================================
P9.15                                   GPIO_48                     12
P9.23                                   GPIO_49                      9
P9.30                                   GPIO_112                     8
P9.27                                   GPIO_115                     6
============================================================================================= */


#define GPIO_66     66
#define GPIO_67     67
#define GPIO_69     69
#define GPIO_68     68
#define GPIO_45     45
#define GPIO_44     44
#define GPIO_26     26
#define GPIO_46     46

#define GPIO_48     48
#define GPIO_49     49
#define GPIO_112    112
#define GPIO_115    115

 
#define GPIO_66_P8_7_SEGA       GPIO_66       /*  display pin 11    */
#define GPIO_67_P8_8_SEGB       GPIO_67       /*  display pin 7     */
#define GPIO_69_P8_9_SEGC       GPIO_69       /*  display pin 4     */
#define GPIO_68_P8_10_DP        GPIO_68       /*  display pin 3     */
#define GPIO_45_P8_11_SEGD      GPIO_45       /*  display pin 2     */
#define GPIO_44_P8_12_SEGE      GPIO_44       /*  display pin 1     */
#define GPIO_26_P8_14_SEGF      GPIO_26       /*  display pin 10    */
#define GPIO_46_P8_16_SEGG      GPIO_46       /*  display pin 5     */

#define GPIO_48_P9_15_DIGIT1     GPIO_48       /*  display pin 12    */
#define GPIO_49_P9_23_DIGIT2     GPIO_49       /*  display pin 9     */
#define GPIO_117_P9_30_DIGIT3    GPIO_112      /*  display pin 8     */
#define GPIO_115_P9_27_DIGIT4    GPIO_115      /*  display pin 6     */ 



#define HIGH_VALUE          1
#define LOW_VALUE           0

#define GPIO_DIR_OUT        HIGH_VALUE
#define GPIO_DIR_IN         LOW_VALUE

#define GPIO_LOW_VALUE      LOW_VALUE
#define GPIO_HIGH_VALUE     HIGH_VALUE

#define SEGMENT_ON          LOW_VALUE
#define SEGMENT_OFF         HIGH_VALUE


/* This is the path corresponds to GPIOs in the 'sys' directory */
#define SYS_GPIO_PATH       "/sys/class/gpio"

#define SOME_BYTES          100

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

/* This function displays number on the 7segment display */
void Write_number_to_7segdisplay(uint8_t numberToDisplay)
{

    switch (numberToDisplay){

    case 0:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_OFF);
    break;

    case 1:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_OFF);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_OFF);
    break;

    case 2:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_OFF);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 3:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 4:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_OFF);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 5:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_OFF);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 6:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_OFF);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 7:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_OFF);
    break;

    case 8:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 9:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 10:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_OFF);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_OFF);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_OFF);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_OFF);
    break;
    }



}

void dispaly_numbers(uint32_t number)
{

    for(int digit = 4 ; digit > 0 ; digit--) 
    {

        //Turn on a digit for a short amount of time
        switch(digit) 
        {
            case 1:
                gpio_write_value(GPIO_48_P9_15_DIGIT1,HIGH_VALUE);
              break;

            case 2:
                gpio_write_value(GPIO_49_P9_23_DIGIT2,HIGH_VALUE);
              break;
            case 3:
                gpio_write_value(GPIO_117_P9_30_DIGIT3,HIGH_VALUE);
              break;
            case 4:
                gpio_write_value(GPIO_115_P9_27_DIGIT4,HIGH_VALUE);
              break;
        }


        Write_number_to_7segdisplay(5);
        number /= 10;

        usleep(500);

#if 0
     //Turn off all segments
        //passing 10 as an argument to this function, turns off all the segments
        Write_number_to_7segdisplay(5);

        //Turn off all digits
        gpio_write_value(GPIO_48_P_15_DIGIT1, LOW_VALUE);
        gpio_write_value(GPIO_49_P_23_DIGIT2, LOW_VALUE);
        gpio_write_value(GPIO_117_P_30_DIGIT3, LOW_VALUE);
        gpio_write_value(GPIO_115_P_27_DIGIT4, LOW_VALUE);
#endif

    }
}


uint32_t get_time_in_ms(void)
{

#if 1
    uint32_t ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    return ms;

#endif
}


void start_upcounting(int delay_value)
{
    int counter=0;
    uint32_t old_ms;
    printf("UP COUNTING.......\n");
    
    old_ms = get_time_in_ms();

        while(1)
       {
#if 1
           if(get_time_in_ms() < (0 + 500 ) )
           {
                dispaly_numbers(counter);
                printf("ms %d\n", get_time_in_ms());
           }
           else
           {
               counter++;
               printf("number display %d\n", counter);
               old_ms = get_time_in_ms();

           }
#endif
           dispaly_numbers(counter % 100);
           counter++;
       }

}

void start_downcounting(int delay_value)
{
     printf("DOWN COUNTING.......\n");
        while(1)
       {


       }

}

void start_updowncounting(int delay_value)
{ 
    printf("/* DIY : Student has to implement this */\n");

}

void start_randomcounting(int delay_value)
{
     printf("/* DIY : Student has to implement this */\n");
}

int main(int argc, char *argv[])
 {
    printf("4 digit 7seg LED up/down/random counter application\n");

    if ( argc != 3 ) /* argc should be 3 for correct execution */
    {

        printf( "usage: %s <direction> <delay>\n", argv[0] );
        printf( "valid direction : up, down, updown,random\n");
        printf ("recommended delay range : 0 ms to 1000 ms\n");
    }
    else
    {
        int value = atoi(argv[2]);

        /*first configure the direction for segments */
        gpio_configure_dir(GPIO_66_P8_7_SEGA,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_67_P8_8_SEGB,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_69_P8_9_SEGC,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_68_P8_10_DP,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_45_P8_11_SEGD,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_44_P8_12_SEGE,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_26_P8_14_SEGF,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_46_P8_16_SEGG,GPIO_DIR_OUT);

        /*configure the direction for digit control */
        gpio_configure_dir(GPIO_48_P9_15_DIGIT1,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_49_P9_23_DIGIT2,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_117_P9_30_DIGIT3,GPIO_DIR_OUT);
        gpio_configure_dir(GPIO_115_P9_27_DIGIT4,GPIO_DIR_OUT);

        /* make all segments low */
        gpio_write_value(GPIO_66_P8_7_SEGA,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_67_P8_8_SEGB,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_69_P8_9_SEGC,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_68_P8_10_DP,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_45_P8_11_SEGD,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_44_P8_12_SEGE,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_26_P8_14_SEGF,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_46_P8_16_SEGG,GPIO_LOW_VALUE);

        /*MAKE ALL DIGITS OFF */
        gpio_write_value(GPIO_48_P9_15_DIGIT1,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_49_P9_23_DIGIT2,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_117_P9_30_DIGIT3,GPIO_LOW_VALUE);
        gpio_write_value(GPIO_115_P9_27_DIGIT4,GPIO_LOW_VALUE);

        /* argc is correct , lets check argv */
        if (strcmp(argv[1], "up") == 0)
        {
            start_upcounting(value);
        }
        else if (strcmp(argv[1], "down") == 0)
        {
            start_downcounting(value);
        }
        else if (strcmp(argv[1], "updown") == 0)
        {
            start_updowncounting(value);
        }
        else if (strcmp(argv[1], "random") == 0)
        {
            start_randomcounting(value);
        }
        else /* default: */
        {
            printf("Invalid direction values\n");
            printf( "valid direction values : up, down,updown,random\n");
        }
    }
}
