/*
 * lcd_driver.h
 *
 *  Created on: 08-May-2017
 *      Author: kiran
 */

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

/* BBB GPIO numbers used to connect with LCD pins */
#define GPIO_66_P8_7_RS_4       		66       /*  Register selection (Character vs. Command) */
#define GPIO_67_P8_8_RW_5      		 	67       /*  Read/write     */
#define GPIO_69_P8_9_EN_6       		69       /*  Enable */
#define GPIO_68_P8_10_D4_11      		68       /*  Data line 4    */
#define GPIO_45_P8_11_D5_12      		45       /*  Data line 5    */
#define GPIO_44_P8_12_D6_13      		44       /*  Data line 6    */
#define GPIO_26_P8_14_D7_14     		26       /*  Data line 7    */

/* BBB GPIO pins used for LCD backlight */
#define GPIO_46_P8_16_BKLTA_15      	46       /*  Backlight anode    */
#define GPIO_48_P9_15_BKLTK_16      	48        /*  Backlight cathode */


/* LCD data lines */
#define LCD_DATABIT0
#define LCD_DATABIT1
#define LCD_DATABIT2
#define LCD_DATABIT3
#define LCD_DATABIT4    GPIO_68_P8_10_D4_11
#define LCD_DATABIT5    GPIO_45_P8_11_D5_12
#define LCD_DATABIT6    GPIO_44_P8_12_D6_13
#define LCD_DATABIT7    GPIO_26_P8_14_D7_14


//Register selection (Character vs. Command)
/*The register selection pin sets the LCD to command or character mode, so it knows how to
interpret the next set of data that is transmitted via the data lines. Based on the state of this pin,
data sent to the LCD is either interpreted as a command (for example, move the cursor) or
characters (for example, the letter a). */

#define COMMAND_MODE       		 LOW_VALUE
#define USER_DATA_MODE           HIGH_VALUE


/*************************************LCD COMMAND SETS **********************************************/

/*Clears entire display and sets DDRAM address 0 in
address counter .. clears display fills up DDRAM with 20H ..
thats the ASCII value for black character or "space"*/
#define LCD_CMD_CLEAR_DISPLAY  0x01

/*Sets DDRAM address 0 in address counter. Also returns display from being shifted to original position.
DDRAM contents remain unchanged. */
#define LCD_CMD_RETURN_HOME 0x02


/* Sets cursor move direction and specifies display shift. These operations are performed during data write
and read. */
#define LCD_CMD_ENTRY_MODESET   			 0X04

#define INC_CURSOR     						( 1 << 1)
#define DEC_CURSOR    						 (LCD_CMD_ENTRY_MODESET & ~(INC_CURSOR))
#define ACCOMPANY_DISPLAY_SHIFT            	( 1 << 0)
#define DO_NOT_ACCOMPANY_DISPLAY_SHIFT      (LCD_CMD_ENTRY_MODESET & ~(ACCOMPANY_DISPLAY_SHIFT))


/*Sets entire display (D) on/off, cursor on/off (C), and blinking of cursor position character (B).*/
#define LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL  0x08

#define DISPLAY_ON    						(1 << 2)
#define DISPLAY_OFF   						(LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL & ~(DISPLAY_ON))

#define CURSOR_ON    						(1 << 1)
#define CURSOR_OFF  						(LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL & ~(CURSOR_ON))

#define BLINK_CURSOR_ON 	 				(1 << 0)
#define BLINK_CURSOR_OFF  					(LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL & ~(BLINK_CURSOR_ON))

/* Moves cursor and shifts display without changing DDRAM contents*/
#define LCD_CMD_CURSOR_DISPLAY_SHIFT_CONTROL  0x10

#define DISPLAY_SHIFT   					  ( 1 << 3)
#define CURSOR_MOVE     					  (LCD_CMD_CURSOR_DISPLAY_SHIFT_CONTROL & ~(DISPLAY_SHIFT))

#define SHIFT_TO_RIGHT  					( 1 << 2)
#define SHIFT_TO_LEFT   					 (LCD_CMD_CURSOR_DISPLAY_SHIFT_CONTROL & ~(SHIFT_TO_RIGHT))


/*Sets interface data length (DL), number of display lines (N), and character font (F). */
#define LCD_CMD_FUNCTION_SET  				0x20

#define DATA_LEN_8  						( 1 << 4)
#define DATA_LEN_4  						(LCD_CMD_FUNCTION_SET & ~(DATA_LEN_8))
#define DISPLAY_2_LINES 				 	( 1 << 3)
#define DISPLAY_1_LINE  					(LCD_CMD_FUNCTION_SET & ~(DISPLAY_2_LINES))
#define MATRIX_5_X_10 						( 1 << 2)
#define MATRIX_5_X_8 						(LCD_CMD_FUNCTION_SET & ~(MATRIX_5_X_10))


/*Sets CGRAM address. CGRAM data is sent and received after this setting. */
#define LCD_CMD_SET_CGRAM_ADDRESS  			0x40

/* Sets DDRAM address. DDRAM data is sent and received after this setting. */
#define LCD_CMD_SET_DDRAM_ADDRESS  			0x80

#define DDRAM_SECOND_LINE_BASE_ADDR         (LCD_CMD_SET_DDRAM_ADDRESS | 0x40 )
#define DDRAM_FIRST_LINE_BASE_ADDR          LCD_CMD_SET_DDRAM_ADDRESS


#define LCD_ENABLE 1
#define LCD_DISABLE 0

/* HD44780 CGRAM address start */
#define CGRAM_address_start 0x40

#define INS_WAIT_TIME ( 8 * 1000)


//public function prototypes
void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t column);
void lcd_enable(void);
void lcd_print_char(uint8_t ascii_Value);
void lcd_print_string(char *message);
void lcd_send_command(uint8_t command);
void lcd_printf(const char *fmt, ...);
void lcd_load_cgram(char tab[], uint8_t charnum);



#endif /* LCD_DRIVER_H_ */
