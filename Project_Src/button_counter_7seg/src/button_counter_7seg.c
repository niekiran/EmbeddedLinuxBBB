/*
 ============================================================================
 Name        : button_counter_7seg.c
 Author      : Kiran N < niekiran@gmail.com >
 Version     :
 Copyright   : Your copyright notice
 Description : This Application up counts(0 to 9) on 7seg display as you press button .
 TODOs for the students :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>

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

//used for button , you have to connect a button to BBB's expansion header P9 23rd pin */
#define GPIO_49     49
#define GPIO_49_P9_23_BUTTON      GPIO_49


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

#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */

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

/* This function initializes the gpio pin of the button
 * first gpio direction is set to "in
 * Then, value is set to "0"
 * edge detection is configured for "rising"
 */
int initialize_button_gpio(void)
{
    gpio_configure_dir(GPIO_49,GPIO_DIR_IN);
    gpio_write_value(GPIO_68, GPIO_LOW_VALUE );
    gpio_configure_edge(GPIO_49,"rising");
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



int main(int argc, char *argv[]) 
{

    int readvalue=0;
    uint8_t led_counter;
    int gpio_fd, timeout, rc;
    struct pollfd fdset[2];
    uint8_t buf[SOME_BYTES];

    int nfds = 2;

    printf("This program counts button press up to 9 on 7segment display\n");


    if ( (initialize_all_gpios() < 0 ) || (initialize_button_gpio() < 0))
    {
        printf("Error: Could not able to initialize the gpios..Exiting..\n");
    }
    else
    {
        printf("Waiting for the button press \n");

        /* open the button gpio in NON-blocking , READONLY mode */
        gpio_fd = open("/sys/class/gpio/gpio49/value", O_RDONLY | O_NONBLOCK );
        if (gpio_fd < 0)
        {
            perror(" open failed\n");
        }

        /* Time out is set to 3 secs */
        timeout = POLL_TIMEOUT;
        
        while(1)
        {
        	/* Initialize fdset structure members to zero */
            memset((void*)fdset, 0, sizeof(fdset));

            fdset[1].fd = gpio_fd;
            /*we want POLLPRI event to be generated */
            fdset[1].events = POLLPRI;

            /* Keep waiting for the events , it happens when you press the button */
            rc = poll(fdset, nfds, timeout); //program blocks here until you press the button or until timeout

            if (rc < 0) 
            {
                printf("\npoll() failed!\n");
                return -1;
            }

            if (rc == 0) 
            {
            //  printf(".");
            }

            /* check whether the event is POLLPRI */
            if (fdset[1].revents & POLLPRI)
            {
                lseek(fdset[1].fd, 0, SEEK_SET);
                read(fdset[1].fd, buf,100);
                led_counter++;
                Write_number_to_7segdisplay(led_counter);
                printf("\npoll() GPIO %d interrupt occurred\n", readvalue);
            }
        }

     close(gpio_fd);
    }


}
