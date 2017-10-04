/*
 ============================================================================
 Name        : BBB_led_control.c
 Author      : Kiran N < niekiran@gmail.com >
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : Controls the BBB user led 3 trigger and brightness values
 TODOs for Students : Make it generic for all the on board leds of the BBB
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/* This is the path corresponds to USER LEDS in the 'sys' directory */
#define SYS_FS_LEDS_PATH "/sys/class/leds"

/* This is the path corresponds to USER LED 3 in the 'sys' directory */
//#define SYS_FS_ "/sys/class/leds/beaglebone:green:usr3"

#define USR_LED_NUMBER 3
#define SOME_BYTES 100 

/*This function writes the tigger values for the given "led_no"
 * returns 0 if success, else -1 
 */
int write_trigger_values(uint8_t led_no, char *value)
{
	int fd,ret=0;
	char buf[SOME_BYTES];
	
	/* we are concatenating  2 strings and storing that in to 'buf'  */
	snprintf(buf,sizeof(buf),SYS_FS_LEDS_PATH "/beaglebone:green:usr%d/trigger",led_no);
	
	/* open the file in write mode */
	/*Returns the file descriptor for the new file. The file descriptor returned is always the smallest integer greater than zero that is still available. If a negative value is returned, then there was an error opening the file.*/
	fd = open(buf, O_WRONLY );
	if (fd <= 0) {
		perror(" write trigger error\n");
		return -1;
	}
	
	/* Write the 'value' in to the file designated by 'fd' */
	/*Returns the number of bytes that were written. 
	  If value is negative, then the system call returned an error.
	*/
	ret = write(fd, (void*)value, strlen(value) );
	if ( ret <= 0)
	{
		printf("trigger value write error\n");
		return -1;
	}
	
	return 0;

}

/*This function writes the brightness values for the given "led_no"
 * returns 0 if success , else -1 
 */
int write_brightness_values(uint8_t led_no, int value)
{
	int fd,ret=0;
	char buf[SOME_BYTES];
	
	/* we are concatenating  2 strings and storing that in to 'buf'  */
	snprintf(buf,sizeof(buf),SYS_FS_LEDS_PATH "/beaglebone:green:usr%d/brightness",led_no);
	
	/* open the file in write mode */
	/*Returns the file descriptor for the new file. The file descriptor returned is always the smallest integer greater than zero that is still available. If a negative value is returned, then there was an error opening the file.*/
	fd = open(buf, O_WRONLY );
	if (fd < 0) {
		perror(" write trigger error\n");
		return -1;
	}
	
	/* Write the 'value' in to the file designated by 'fd' */
	/*Returns the number of bytes that were written. 
	  If value is negative, then the system call returns an error.
	*/
	ret = write(fd, (void *)&value, sizeof(value) );
	if ( ret <= 0)
	{
		printf("trigger value write error\n");
		return -1;
	}
	
	return 0;

}

/* Compare and process the trigger values given by the user */
void process_trigger_values( char *value)
{	
	if( ! (strcmp(value, "timer") && strcmp(value, "heartbeat") && \
			strcmp(value, "none") && strcmp(value, "oneshot") && \
			strcmp(value, "default-on") ) )
	{
			write_trigger_values(USR_LED_NUMBER,value);
	}
	else /* default: */
	{
		printf("Invalid value\n");
		printf("valid trigger values : heartbeat,timer,none,oneshot,default-on\n");
	}


}

/* process the brightness values given by the user */
void process_brightness_values( int value)
{
	 switch(value)
	   {
	   case 1:
		   write_brightness_values(USR_LED_NUMBER,value);
		   break;

	   case 0:
		   write_brightness_values(USR_LED_NUMBER,value);
	   	   break;

	   default:
	      	printf("Invalid value\n");
	      	printf ("valid brightness values : 0,1\n");
	   }

}


int main( int argc, char *argv[] )
{
	printf("This Application controls the USER LED3\n");

    if ( argc != 3 ) /* argc should be 3 for correct execution */
    {
        printf( "usage: %s <control_option> <value>\n", argv[0] );
        printf( "valid control_options : brightness, trigger\n");
        printf ("valid 'brightness' values : 0,1\n");
        printf("valid 'trigger' values : heartbeat,timer,none,oneshot,default-on\n");
    }
    else
    {
    	/* argc is valid , lets check argv */
    	if (strcmp(argv[1], "trigger") == 0)
    	{
    		process_trigger_values(argv[2]);
    	}
    	else if (strcmp(argv[1], "brightness") == 0)
    	{
			/* Converting argv[2] which is a brightness value in to integer */
    		int value = atoi(argv[2]);
    		process_brightness_values(value);
    	}
    	else /* default: */
    	{
    		printf("Invalid control option\n");
    		printf( "valid control_options : brightness, trigger\n");
    	}

    }
    return 0;

}
