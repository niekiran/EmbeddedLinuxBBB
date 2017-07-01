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
#include<stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 120
uint8_t numbers[10]={0x77,0x06,0xB3,0x97,0xc6,0xD5,0xf5,0x07,0xf7,0XD7};

/*
P8.7 ( GPIO_66); a
P8.8 ( GPIO_67); b
P8.9 ( GPIO_69); c
P8.10 ( GPIO_68);dp (h)
P8.11 ( GPIO_45);d
P8.12 ( GPIO_44);e
P8.14 ( GPIO_26);f
P8.16 ( GPIO_46);g */

#define SYS_GPIO_PATH "/sys/class/gpio"

/* This is the array of file descriptors for each segment of the display */
FILE *sysgpio[8];


int initialize_all_gpios(void)
{
	char *str="out";
	/* first init the direction as out */
	/* open the file in both read and write mode */
	sysgpio[0] = fopen("/sys/class/gpio/gpio66/direction", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(sysgpio[0] != 0)
	{

		fprintf(sysgpio[0],"%s",str);
		fclose(sysgpio[0]);
		sysgpio[0] = fopen("/sys/class/gpio/gpio66/value", "w+" );
		fprintf(sysgpio[0],"%d",0);
		fflush(sysgpio[0]);
	}
	else
	{
		return -1;
	}


/* first init the direction as out */
	/* open the file in both read and write mode */
	sysgpio[1] = fopen("/sys/class/gpio/gpio67/direction", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(sysgpio[1] != 0)
	{
		fprintf(sysgpio[1],"%s","out");
		fclose(sysgpio[1]);
		sysgpio[1] = fopen("/sys/class/gpio/gpio67/value", "w+" );
		fprintf(sysgpio[1],"%d",0);
		fflush(sysgpio[1]);
	}
	else
	{
		return -1;
	}

/* first init the direction as out */
	/* open the file in both read and write mode */
	sysgpio[2] = fopen("/sys/class/gpio/gpio69/direction", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(sysgpio[2] != 0)
	{
		fprintf(sysgpio[2],"%s","out");
		fclose(sysgpio[2]);
		sysgpio[2] = fopen("/sys/class/gpio/gpio69/value", "w+" );
		fprintf(sysgpio[2],"%d",0);
		fflush(sysgpio[2]);

	}
	else
	{
		return -1;
	}



/* first init the direction as out */
	/* open the file in both read and write mode */
	sysgpio[3] = fopen("/sys/class/gpio/gpio68/direction", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(sysgpio[3] != 0)
	{
		fprintf(sysgpio[3],"%s","out");
		fclose(sysgpio[3]);
		sysgpio[3] = fopen("/sys/class/gpio/gpio68/value", "w+" );
		fprintf(sysgpio[3],"%d",0);
		fflush(sysgpio[3]);
	}
	else
	{
		return -1;
	}

/* first init the direction as out */
	/* open the file in both read and write mode */
	sysgpio[4] = fopen("/sys/class/gpio/gpio45/direction", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(sysgpio[4] != 0)
	{
		fprintf(sysgpio[4],"%s","out");
		fclose(sysgpio[4]);
		sysgpio[4] = fopen("/sys/class/gpio/gpio45/value", "w+" );
		fprintf(sysgpio[4],"%d",0);
		fflush(sysgpio[4]);
	}
	else
	{
		return -1;
	}


/* first init the direction as out */
	/* open the file in both read and write mode */
	sysgpio[5] = fopen("/sys/class/gpio/gpio44/direction", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(sysgpio[5] != 0)
	{
		fprintf(sysgpio[5],"%s","out");
		fclose(sysgpio[5]);
		sysgpio[5] = fopen("/sys/class/gpio/gpio44/value", "w+" );
		fprintf(sysgpio[5],"%d",0);
		fflush(sysgpio[5]);
	}
	else
	{
		return -1;
	}


/* first init the direction as out */
	/* open the file in both read and write mode */
	sysgpio[6] = fopen("/sys/class/gpio/gpio26/direction", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(sysgpio[6] != 0)
	{
		fprintf(sysgpio[6],"%s","out");
		fclose(sysgpio[6]);
		sysgpio[6] = fopen("/sys/class/gpio/gpio26/value", "w+" );
		fprintf(sysgpio[6],"%d",0);
		fflush(sysgpio[6]);
	}
	else
	{
		return -1;
	}


/* first init the direction as out */
	/* open the file in both read and write mode */
	sysgpio[7] = fopen("/sys/class/gpio/gpio46/direction", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(sysgpio[7] != 0)
	{
		fprintf(sysgpio[7],"%s","out");
		fclose(sysgpio[7]);
		sysgpio[7] = fopen("/sys/class/gpio/gpio46/value", "w+" );
		fprintf(sysgpio[7],"%d",0);
		fflush(sysgpio[7]);
	}
	else
	{
		return -1;
	}




return 0;

}

void Write_number(uint8_t value)
{

	int i=0,value1;


	for (i=0;i< 8;i++)
	{
		value1 =  ( value & ( 1 << i) );
		value1 = value1 >> i;
		fprintf(sysgpio[i],"%d", value1 );
		fflush(sysgpio[i]);
	}

}

FILE *filebutton;
int initialize_button_gpio(void)
{

	filebutton = fopen("/sys/class/gpio/gpio49/direction", "w+" );
	/* fopen returns 0, the NULL pointer, on failure */
	if(filebutton != 0)
	{
		fprintf(filebutton,"%s","in");
		fclose(filebutton);
		filebutton = fopen("/sys/class/gpio/gpio49/value", "w+" );
		fprintf(filebutton,"%d",0);
		filebutton = fopen("/sys/class/gpio/gpio49/edge", "w+" );
		fprintf(filebutton,"%s","rising");
		fclose(filebutton);
	}
	else
	{
		return -1;
	}

	return 0;

}

uint8_t led_counter;
int main(int argc, char *argv[]) {

	 int readvalue=0;
	 int gpio_fd, timeout, rc;
	struct pollfd fdset[2];
	char *buf[MAX_BUF];
		unsigned int gpio;
		int nfds = 2;
		int len;

	printf("This program counts button press up to 9 on 7segment LED\n");



	if ( (initialize_all_gpios() < 0 ) || (initialize_button_gpio() < 0))
	{
		printf("Error: Could not able to initialize the gpios..Exiting..\n");
	}else
	{

		printf("Waiting for the button press \n");

		gpio_fd = open("/sys/class/gpio/gpio49/value", O_RDONLY | O_NONBLOCK );
			if (gpio_fd < 0) {
				perror("gpio/fd_open");
			}

			timeout = POLL_TIMEOUT;
		while(1)
		{
			memset((void*)fdset, 0, sizeof(fdset));

			fdset[1].fd = gpio_fd;
			fdset[1].events = POLLPRI;


			rc = poll(fdset, nfds, timeout);

			if (rc < 0) {
			printf("\npoll() failed!\n");
			return -1;
		}

			if (rc == 0) {
			//	printf(".");
			}

			if (fdset[1].revents & POLLPRI) {
				lseek(fdset[1].fd, 0, SEEK_SET);
			    len = read(fdset[1].fd, buf,100);
				led_counter++;
				led_counter %= 10;
				Write_number(numbers[led_counter]);

				printf("\npoll() GPIO %d interrupt occurred\n", readvalue);
			}


			fflush(stdout);


		}

#if 0
		while(1)
		{
#if 0
			filebutton = fopen("/sys/class/gpio/gpio49/value", "r+" );
			fscanf(filebutton, "%d",&readvalue);
			fclose(filebutton);
#endif

			printf("Button read : %d\n",readvalue);

			//while(!readvalue);
			if(readvalue)
			{

				cnt++;
				if(cnt > 150)
				{
					led_counter++;
					led_counter %= 10;
					Write_number(numbers[led_counter]);
					cnt=0;
				}
			}


		}
#endif

     close(gpio_fd);
	}


}
