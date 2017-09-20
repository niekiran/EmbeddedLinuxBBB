/*
 ============================================================================
 Name        : lcd_news.c
 Author      : Kiran N < niekiran@gmail.com >
 Version     :
 Copyright   : Your copyright notice
 Description : This Application prints real time news headlines on the 16x2 LCD
 TODOs for the students :
 1. Fetch and display news headlines from different online news channels . 
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "curl/curl.h"

#include "jsmn.h"

#include "gpio_driver.h"
#include "lcd_driver.h"

char t_buf[10][100];

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

/* This is the URL from where we are fetching the news headlines */
char URL[] = "https://newsapi.org/v1/articles?source=cnn&sortBy=top&apiKey=d121b6202e754bcf90d278c5b8f5f4ef";

#define BUFFER_SIZE 10000

char buf[BUFFER_SIZE];


/*The maximum amount of body data that will be passed to the write callback is defined in the curl.h header file: CURL_MAX_WRITE_SIZE (the usual default is 16K).*/

/*The data passed to this function will not be zero terminated!*/

static size_t fetch_data(void *data_ptr_rcvd, size_t size, size_t nmemb, void *userp)
{
    //  buf_t *buf = (buf_t *) userp;
    size_t total_size = size * nmemb;

    if(total_size > BUFFER_SIZE )
        total_size = BUFFER_SIZE;

    memcpy(userp,data_ptr_rcvd,BUFFER_SIZE);

    char *buffer = (char*)userp;
    buffer[BUFFER_SIZE-1] = '\0';
    
    return total_size;
}



char * json_fetch(char *url)
{
    CURL *curl = curl_easy_init();
    if(! curl)
    {
    printf("curl init faild . exiting \n");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);


    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetch_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)buf);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "jsmn-example (https://github.com/alisdair/jsmn-example, alisdair@mcdiarmid.org)");

    struct curl_slist *hs = curl_slist_append(NULL, "Accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        printf("curl_easy_perform failed: %s\n", curl_easy_strerror(res));

    curl_easy_cleanup(curl);
    curl_slist_free_all(hs);

    buf[BUFFER_SIZE-1] = '\0';


    return buf;
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

int fetch_news()
{

    int i,k=0;
    int r;

    lcd_set_cursor( 1,0);
    lcd_printf("Fetching News..!");

    const char *json_string = json_fetch(URL);

    if(! json_string)
    {
        printf("JSON fetch from URL failed .. exiting \n");
        lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
        lcd_printf("News Fetch Failed !");
        return -1;
    }
    jsmn_parser p;
    jsmntok_t t[128 * 2]; /* We expect no more than 128 tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, json_string, strlen(json_string), t, sizeof(t)/sizeof(t[0]));
    if (r < 0) {
        printf("Failed to parse JSON: %d %s\n", r, json_string);
        lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
        lcd_printf("News Fetch Failed !");
        return 1;
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        printf("Object expected\n");
        lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
        lcd_printf("News Fetch Failed !");
        return 1;
    }

    /* Loop over all keys of the root object */
    for (i = 1; i < r; i++) {
        if (jsoneq(json_string, &t[i], "source") == 0) {
            /* We may use strndup() to fetch string value */
            printf("-Source-: %.*s\n", t[i+1].end-t[i+1].start,
                    json_string + t[i+1].start);
            i++;
        } else if (jsoneq(json_string, &t[i], "author") == 0) {
            /* We may additionally check if the value is either "true" or "false" */
            printf("-Author-: %.*s\n", t[i+1].end-t[i+1].start,
                    json_string + t[i+1].start);
            i++;
        } else if (jsoneq(json_string, &t[i], "title") == 0) {
            /* We may want to do strtol() here to get numeric value */
           // printf("- Title-: %.*s\n", t[i+1].end-t[i+1].start,
                 //   json_string + t[i+1].start);

           snprintf(t_buf[k],100, " %.*s", t[i+1].end-t[i+1].start,json_string + t[i+1].start);
           printf("- Title- %s\n",t_buf[k]);
            k++;
            i++;
        } else {
                           ;
        }
    }

    lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
    lcd_printf("SUCCESS !");
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
void setup_lcd(void)
{

	  printf("Application fetches news online and displays on LCD\n");

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
	    uint8_t cmd=0;

	    lcd_init();

		//setting function
	    // only single line mode is chosen here , that means all the 80 characters of the DDRAM will be mapped to lcd line 1.
	    cmd= LCD_CMD_FUNCTION_SET | DATA_LEN_4 | DISPLAY_1_LINE | MATRIX_5_X_8;
	    lcd_send_command(cmd);

	    //3.display on/off control , no cursor
	    cmd = LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL |DISPLAY_ON |CURSOR_OFF  ;
	    lcd_send_command(cmd);



	    lcd_send_command(LCD_CMD_CLEAR_DISPLAY);

#if 0
	    //lets start printing from extreme right end of first row of the lcd. .
	  	    lcd_set_cursor( 1,17);
#endif

}


int k=0;
int main() 
{


	setup_lcd();

	fetch_news();

	sleep(1);

	lcd_send_command(LCD_CMD_CLEAR_DISPLAY);

	lcd_set_cursor(1,17);

	char *ptr = NULL;
	uint8_t cnt,cmd=0;

	while(1)
	{


		//we have total five strings.
		for ( cnt = 0 ;cnt < 2 ; cnt++)
		{
			//print till each string meets NULL char
			ptr= t_buf[cnt];
			while( *ptr != '\0' )
			{
				//printing one character at a time and then left shifting by sending this command.
				cmd = LCD_CMD_CURSOR_DISPLAY_SHIFT_CONTROL | DISPLAY_SHIFT | SHIFT_TO_LEFT ;
				lcd_send_command(cmd);

				lcd_print_char((uint8_t)*ptr);

				ptr++;
				usleep(300 * 1000);

			}

			lcd_print_char('-');
			lcd_print_char('-');
			lcd_print_char('-');
		}

	//	tansition_graphics();
       // lcd_print_string("fetching....");
		cmd = LCD_CMD_ENTRY_MODESET | INC_CURSOR | DO_NOT_ACCOMPANY_DISPLAY_SHIFT;
		lcd_send_command(cmd);
		fetch_news();
		lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
		lcd_set_cursor(1,17);
	}



    return EXIT_SUCCESS;
}
