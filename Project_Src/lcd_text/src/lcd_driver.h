/*
 * lcd_driver.h
 *
 *  Created on: 08-May-2017
 *      Author: kiran
 */

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_



#define LCD_CLEAR_DISPALY           0x01
#define LCD_MOV_2ND_ROW             0xC0
#define LCD_MOV_HOME                0x02
#define LCD_LEFT_SHIFT              0x18
#define LCD_RIGHT_SHIFT             0x1C
#define LCD_DISP_ON                 0x0C
#define LCD_DISP_OFF                0x08
#define LCD_SET_CURS_POSITION


#define LCD_DATABIT0
#define LCD_DATABIT1
#define LCD_DATABIT2
#define LCD_DATABIT3
#define LCD_DATABIT4    GPIO_68_P8_10_D4_7
#define LCD_DATABIT5    GPIO_45_P8_11_D5_8
#define LCD_DATABIT6    GPIO_44_P8_12_D6_9
#define LCD_DATABIT7    GPIO_26_P8_14_D7_10



#define LCD_ENABLE 1
#define LCD_DISABLE 0

void lcd_init(void);
void lcd_locate(uint8_t row, uint8_t column);
void lcd_enable(void);
void lcd_print_char(uint8_t ascii_Value);
void lcd_print_string(uint8_t *message);
void lcd_send_command(uint8_t command);
void lcd_printf(const char *fmt, ...);
void lcd_load_cgram(char tab[], uint8_t charnum);

/* HD44780 CGRAM address start */
#define CGRAM_address_start 0x40




#endif /* LCD_DRIVER_H_ */
