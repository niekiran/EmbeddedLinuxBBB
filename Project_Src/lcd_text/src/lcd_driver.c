#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>

#include "lcd_text.h"
#include "lcd_driver.h"

void lcd_init(void)
{

    gpio_write_value(GPIO_69_P8_9_EN_6,LCD_ENABLE);

    //Initialization of HD44780-based LCD (4-bit HW)
    //lcd_send_command(0x33);
    usleep(4 * 1000);
//  lcd_send_command(0x32);
    usleep(4 * 1000);
    //Function Set 4-bit mode
    lcd_send_command(0x28); //  0010 1000
    //Display On/Off Control
    lcd_send_command(0x0C);
    //Entry mode set
    lcd_send_command(0x06);
    lcd_send_command(LCD_CLEAR_DISPALY);
    //Minimum delay to wait before driving LCD module
    usleep(200 * 1000);


}

/**
  * @brief  Set Cursor to a specified location given by row and column information
  * @param  Row Number (1 to 2)
  * @param  Column Number (1 to 16) Assuming a 2 X 16 characters display
  * @retval None
  */
void lcd_locate(uint8_t row, uint8_t column)
{
  column--;
  switch (row)
  {
    case 1:
      /* Set cursor to 1st row address and add index*/
      lcd_send_command(column |= 0x80);
      break;
    case 2:
      /* Set cursor to 2nd row address and add index*/
        lcd_send_command(column |= 0x40 | 0x80);
      break;
    default:
      break;
  }
}

void lcd_enable(void)
{
    gpio_write_value(GPIO_69_P8_9_EN_6,LCD_ENABLE);
    usleep(2 * 1000); //2ms delay
    gpio_write_value(GPIO_69_P8_9_EN_6,LCD_DISABLE);


}
void lcd_print_char(uint8_t ascii_Value)
{

    gpio_write_value(GPIO_66_P8_7_RS_4,CHAR_MODE);

    //first send the msb

    uint8_t data_msb = ((ascii_Value >> 4) & 0X0f ); // d7 d7 d5 d4

    gpio_write_value(LCD_DATABIT4,(data_msb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(data_msb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(data_msb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(data_msb & ( 1 << 3) ));

    lcd_enable();
    uint8_t data_lsb = (( ascii_Value & 0x0f )); // d7 d7 d5 d4

    gpio_write_value(LCD_DATABIT4,(data_lsb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(data_lsb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(data_lsb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(data_lsb & ( 1 << 3) ));

    lcd_enable();

    usleep(2 * 1000);


}

void lcd_print_string(uint8_t *message)
{

      do
      {
          lcd_print_char(*message++);
      }
      while (*message != '\0');

}


void lcd_send_command(uint8_t command)
{
/*RS: Is a Register Select Control signal.
When this signal is '1' = It accepts data to be displayed.
When this signal is '0' = It accepts instructions for
the LCD like setting font, cursor position etc. */

    //first make RS=0
    gpio_write_value(GPIO_66_P8_7_RS_4,COMMAND_MODE);

    //first send the msb

    uint8_t command_msb = ((command >> 4) & 0X0f ); // d7 d7 d5 d4

    gpio_write_value(LCD_DATABIT4,(command_msb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(command_msb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(command_msb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(command_msb & ( 1 << 3) ));

    lcd_enable();
    uint8_t command_lsb = (( command & 0x0f )); // d7 d7 d5 d4

    gpio_write_value(LCD_DATABIT4,(command_lsb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(command_lsb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(command_lsb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(command_lsb & ( 1 << 3) ));

    lcd_enable();

    usleep(2*1000);


}

/**
  * @brief  Initializes Character Generator CGRAM with custom characters data
  * @param  Table containing characters definition values
  * @param  Number of characters defined in the table
  * @retval None
  */
void lcd_load_cgram(char tab[], uint8_t charnum)
{
  uint8_t index;
  /* Each character contains 8 definition values*/
  charnum = charnum * 8;
  for (index = 0;index < charnum;index++)
  {
    /* Store values in LCD*/
    lcd_print_char(tab[index]);
    usleep(1*1000);
  }
}


void lcd_printf(const char *fmt, ...)
{
     int i;
      uint32_t text_size, letter;
      static char text_buffer[32];
      va_list args;

      va_start(args, fmt);
      text_size = vsprintf(text_buffer, fmt, args);

      // Process the string
      for (i = 0; i < text_size; i++)
      {
        letter = text_buffer[i];

        if (letter == 10)
          break;
        else
        {
          if ((letter > 0x1F) && (letter < 0x80))
              lcd_print_char(letter);
        }
      }
  }
