/*
 * lcd_scroll_text.h
 *  Author: kiran
 */

#ifndef LCD_TEXT_SCROLL_H_
#define LCD_TEXT_SCROLL_H_


#define HIGH_VALUE          1
#define LOW_VALUE           0

#define GPIO_DIR_OUT        HIGH_VALUE
#define GPIO_DIR_IN         LOW_VALUE

#define GPIO_LOW_VALUE      LOW_VALUE
#define GPIO_HIGH_VALUE     HIGH_VALUE

#define SEGMENT_ON          LOW_VALUE
#define SEGMENT_OFF         HIGH_VALUE

//Register selection (Character vs. Command)
/*The register selection pin sets the LCD to command or character mode, so it knows how to
interpret the next set of data that is transmitted via the data lines. Based on the state of this pin,
data sent to the LCD is either interpreted as a command (for example, move the cursor) or
characters (for example, the letter a). */

#define COMMAND_MODE        LOW_VALUE
#define CHAR_MODE           HIGH_VALUE


/* This is the path corresponds to GPIOs in the 'sys' directory */
#define SYS_GPIO_PATH       "/sys/class/gpio"

#define SOME_BYTES          100



#define GPIO_66_P8_7_RS_4        66       /*  Register selection (Character vs. Command) */
#define GPIO_67_P8_8_RW_5        67       /*  Read/write     */
#define GPIO_69_P8_9_EN_6        69       /*  Enable */
#define GPIO_68_P8_10_D4_11      68       /*  Data line 4    */
#define GPIO_45_P8_11_D5_12      45       /*  Data line 5    */
#define GPIO_44_P8_12_D6_13      44       /*  Data line 6    */
#define GPIO_26_P8_14_D7_14      26       /*  Data line 7    */


#define GPIO_46_P8_16_BKLTA_15      46       /*  Backlight anode    */
#define GPIO_48_P9_15_BKLTK_16      48        /*  Backlight cathode */

/*******************************************************************
Function prototypes 
********************************************************************/
int gpio_configure_dir(uint32_t gpio_num, uint8_t dir_value);
int gpio_write_value(uint32_t gpio_num, uint8_t out_value);
int gpio_read_value(uint32_t gpio_num);
int gpio_configure_edge(uint32_t gpio_num, char *edge);
int gpio_file_open(uint32_t gpio_num);
int gpio_file_close(int fd);


#endif /* LCD_TEXT_SCROLL_H_ */
