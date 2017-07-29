/*
 ============================================================================
 Name        : counter_7seg.c
 Author      : Kiran N < niekiran@gmail.com >
 Version     :
 Copyright   : Your copyright notice
 Description : This Application implements up/down/random counting on ( single digit ) 7 seg display
 TODOs for the students :
 1) Implement the random counting
 2) Implement the updown counting
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/*==================================================================================
BBB_expansion_header_P8_pins           GPIO number            7Seg Display segment
===================================================================================
P8.7                                   GPIO_66                     a
P8.8                                   GPIO_67                     b
P8.9                                   GPIO_69                     c
P8.10                                  GPIO_68                     h //decimal point
P8.11                                  GPIO_45                     d
P8.12                                  GPIO_44                     e
P8.14                                  GPIO_26                     f
P8.16                                  GPIO_46                     g
=================================================================================== */


#define GPIO_66     66
#define GPIO_67     67
#define GPIO_69     69
#define GPIO_68     68
#define GPIO_45     45
#define GPIO_44     44
#define GPIO_26     26
#define GPIO_46     46


#define GPIO_66_P8_7_SEGA       GPIO_66       
#define GPIO_67_P8_8_SEGB       GPIO_67       
#define GPIO_69_P8_9_SEGC       GPIO_69       
#define GPIO_68_P8_10_DP        GPIO_68      
#define GPIO_45_P8_11_SEGD      GPIO_45       
#define GPIO_44_P8_12_SEGE      GPIO_44      
#define GPIO_26_P8_14_SEGF      GPIO_26       
#define GPIO_46_P8_16_SEGG      GPIO_46      



#define HIGH_VALUE  		1
#define LOW_VALUE   		0

#define GPIO_DIR_OUT        HIGH_VALUE
#define GPIO_DIR_IN         LOW_VALUE

#define GPIO_LOW_VALUE      LOW_VALUE
#define GPIO_HIGH_VALUE     HIGH_VALUE

#define SEGMENT_ON          HIGH_VALUE
#define SEGMENT_OFF         LOW_VALUE


/* This is the path corresponds to GPIOs in the 'sys' directory */
#define SYS_GPIO_PATH 		"/sys/class/gpio"

#define SOME_BYTES 			100

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
 *  edge : rising, falling , none
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
	/*  we are driving the leds , so,
	 * 1. first make the gpio which drives the led to output mode , that means set the direction as "out"
	 * 2. then write the value "0" to turn off all the leds or segments.
	 */
    
    gpio_configure_dir(GPIO_66,GPIO_DIR_OUT);
    gpio_write_value(GPIO_66, GPIO_LOW_VALUE );
    
    gpio_configure_dir(GPIO_67,GPIO_DIR_OUT);
    gpio_write_value(GPIO_67, GPIO_LOW_VALUE );
    
    gpio_configure_dir(GPIO_69,GPIO_DIR_OUT);
    gpio_write_value(GPIO_69, GPIO_LOW_VALUE );
    
    gpio_configure_dir(GPIO_68,GPIO_DIR_OUT);
    gpio_write_value(GPIO_68, GPIO_LOW_VALUE );
    
    gpio_configure_dir(GPIO_45,GPIO_DIR_OUT);
    gpio_write_value(GPIO_45, GPIO_LOW_VALUE );
    
    gpio_configure_dir(GPIO_44,GPIO_DIR_OUT);
    gpio_write_value(GPIO_44, GPIO_LOW_VALUE );
    
    gpio_configure_dir(GPIO_26,GPIO_DIR_OUT);
    gpio_write_value(GPIO_26, GPIO_LOW_VALUE );
    
    gpio_configure_dir(GPIO_46,GPIO_DIR_OUT);
    gpio_write_value(GPIO_46, GPIO_LOW_VALUE );

    return 0;

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

/* implements the up counting from 0 to 9 */
void start_upcounting(int delay_value_ms)
{
    uint8_t i =0;
    
    if ( initialize_all_gpios() < 0)
    {
        printf("Error: GPIO init failed !\n");
    }
    else
    {
        printf("UP COUNTING.......\n");
        while(1)
       {
  
            for(i=0;i<10;i++)
            {
                Write_number_to_7segdisplay(i);
                /*suspend execution for microsecond intervals*/
                usleep(delay_value_ms * 1000); //converting ms to micro
            }

       }

    }

}

/* implements the down counting from 9 to 0 */
void start_downcounting(int delay_value_ms)
{
    int i=9;
    if ( initialize_all_gpios() < 0)
    {
        printf("Error: GPIO init failed !\n");
    }
    else
    {
        printf("DOWN COUNTING.......\n");
        while(1)
       {
            for(i=9;i >= 0 ;i--)
            {
                Write_number_to_7segdisplay(i);
                /*suspend execution for microsecond intervals*/
                usleep(delay_value_ms * 1000); //converting ms to micro
            }
       }
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

int main(int argc, char *argv[]) {
    
    printf("Application for up/down/random counter on 7 seg display\n");

    if ( argc != 3 ) /* argc should be 3 for correct execution */
    {

        printf( "usage: %s <direction> <delay>\n", argv[0] );
        printf( "valid direction : up, down, updown, random\n");
        printf ("recommended delay range in ms : 0 to 1000\n");
    }
    else
    {
    	/* convert the 'delay' value , which argv[2]  in to integer */
        int delay_value = atoi(argv[2]);
        
        /* argc is correct , lets check argv */
        if (! strcmp(argv[1], "up") )
        {
            start_upcounting(delay_value);
        }
        else if (strcmp(argv[1], "down") == 0)
        {
            start_downcounting(delay_value);
        }
        else if (strcmp(argv[1], "updown") == 0)
        {
            start_updowncounting(delay_value);
        }
        else if (strcmp(argv[1], "random") == 0)
        {
            start_randomcounting(delay_value);
        }
        else /* default: */
        {
            printf("Invalid direction values\n");
            printf( "valid direction values : up, down,updown,random\n");
        }
    }
}
