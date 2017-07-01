/*
 * lcd_text.h
 *
 *  Created on: 08-May-2017
 *      Author: kiran
 */

#ifndef LCD_TEXT_H_
#define LCD_TEXT_H_



#define MAX_LEN 100

#define DIGIT_ON 1
#define DIGIT_OFF 0

#define SYS_GPIO_DIR_PATH "/sys/class/gpio"

/*
P8.7 ( GPIO_66); a
P8.8 ( GPIO_67); b
P8.9 ( GPIO_69); c
P8.10 ( GPIO_68);dp (h)
P8.11 ( GPIO_45);d
P8.12 ( GPIO_44);e
P8.14 ( GPIO_26);f
P8.16 ( GPIO_46);g */

#define GPIO_66_P8_7_RS_4  		66       /*  Register selection (Character vs. Command)	*/
#define GPIO_67_P8_8_RW_5		67       /*  Read/write 	*/
#define GPIO_69_P8_9_EN_6 		69       /*  Enable	*/
#define GPIO_68_P8_10_D4_7		68       /*  Data line 4	*/
#define GPIO_45_P8_11_D5_8 		45       /*  Data line 5	*/
#define GPIO_44_P8_12_D6_9 		44       /*  Data line 6	*/
#define GPIO_26_P8_14_D7_10		26       /*  Data line 7	*/


#define GPIO_46_P8_16_BKLTA_15		46       /*  Backlight anode	*/
#define GPIO_48_P9_15_BKLTK_16      48        /*  Backlight cathode	*/

#define GPIO_49_P_23_DIGIT2    49        /*  display pin 9	*/
#define GPIO_117_P_30_DIGIT3   112       /*  display pin 8 	*/
#define GPIO_115_P_27_DIGIT4   115      /*  display pin 6	*/


#define HIGH_VALUE 1
#define LOW_VALUE  0

#define OUT_DIR HIGH_VALUE
#define IN_DIR  LOW_VALUE

//Register selection (Character vs. Command)
/*The register selection pin sets the LCD to command or character mode, so it knows how to
interpret the next set of data that is transmitted via the data lines. Based on the state of this pin,
data sent to the LCD is either interpreted as a command (for example, move the cursor) or
characters (for example, the letter a). */

#define COMMAND_MODE LOW_VALUE
#define CHAR_MODE  HIGH_VALUE

int gpio_configure_dir(uint32_t gpio_num, uint8_t dir_value);
int gpio_write_value(uint32_t gpio_num, uint8_t out_value);
int gpio_read_value(uint32_t gpio_num);
int gpio_configure_edge(uint32_t gpio_num, char *edge);
int gpio_file_open(uint32_t gpio_num);
int gpio_file_close(int fd);


#endif /* LCD_TEXT_H_ */
