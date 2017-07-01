/*
 ============================================================================
 Name        : TestProject.c
 Author      : Kiran
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 DIY
 1) take the led number also as argument
 2)
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#define USR_LED3_PATH "/sys/class/leds/beaglebone:green:usr3"



void write_trigger_values(char *value)
{
	/* open the file in both read and write mode */
	FILE *file = fopen(USR_LED3_PATH "/trigger", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(file != 0)
	{
		/* lets write the value to the file and close */
		fprintf(file,"%s",value);
		fclose(file);
	}
	else
	{
		printf("Error: file open failed\n");
	}

}


void write_brightness_values(int value)
{
	/* open the file in both read and write mode */
	FILE *file = fopen(USR_LED3_PATH "/brightness", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(file != 0)
	{
		fprintf(file,"%d",value);
		fclose(file);
	}
	else
	{
		printf("Error: file open failed\n");
	}

}


void process_trigger_values( char *value)
{
	if (strcmp(value, "timer") == 0)
	{
		write_trigger_values(value);
	}
	else if (strcmp(value, "heartbeat") == 0)
	{
		write_trigger_values(value);
	}
	else if (strcmp(value, "none") == 0)
	{
		write_trigger_values(value);
	}
	else if (strcmp(value, "oneshot") == 0)
	{
		write_trigger_values(value);
	}
	else if (strcmp(value, "default-on") == 0)
	{
		write_trigger_values(value);
	}
	else /* default: */
	{
		printf("Invalid value\n");
		printf("valid trigger values : heartbeat,timer,none,oneshot,default-on\n");
	}


}


void process_brightness_values( int value)
{
	 switch(value)
	   {
	   case 1:
		   write_brightness_values(value);
		   break;

	   case 0:
		   write_brightness_values(value);
	   	   break;

	   default:
	      	printf("Invalid value\n");
	      	printf ("valid brightness values : 0,1\n");
	   }


}


int main( int argc, char *argv[] )
{
	printf("This program controls the USER LED3\n");

    if ( argc != 3 ) /* argc should be 3 for correct execution */
    {

        printf( "usage: %s <control_option> <value>\n", argv[0] );
        printf( "valid control_options : brightness, trigger\n");
        printf ("valid brightness values : 0,1\n");
        printf("valid trigger values : heartbeat,timer,none,oneshot,default-on\n");
    }
    else
    {
    	/* argc is correct , lets check argv */
    	if (strcmp(argv[1], "trigger") == 0)
    	{
    		process_trigger_values(argv[2]);
    	}
    	else if (strcmp(argv[1], "brightness") == 0)
    	{
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
