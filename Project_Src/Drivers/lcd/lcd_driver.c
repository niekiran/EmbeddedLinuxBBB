#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>

#include "gpio_driver.h"
#include "lcd_driver.h"


/* This function does basic initialization for your lcd  */
void lcd_init(void)
{
	uint8_t cmd=0;

	//setting function
    cmd= LCD_CMD_FUNCTION_SET | DATA_LEN_4 | DISPLAY_2_LINES | MATRIX_5_X_8;
    lcd_send_command(cmd);

    /* either you read the Busy flag and wait until BF=0, or just wait for ~5ms as per data sheet */
    usleep(INS_WAIT_TIME);

    //setting entry mode of the LCD
    cmd=LCD_CMD_ENTRY_MODESET | INC_CURSOR;
    lcd_send_command(cmd);

    usleep(INS_WAIT_TIME);

    //setting display and cursor control options
    cmd=LCD_CMD_DISPLAY_CURSOR_ONOFF_CONTROL | DISPLAY_ON | CURSOR_ON; //0x0e
    lcd_send_command(cmd);

    usleep(INS_WAIT_TIME);

}

/**
  * @brief  Set Lcd to a specified location given by row and column information
  * @param  Row Number (1 to 2)
  * @param  Column Number (1 to 16) Assuming a 2 X 16 characters display
  */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
  column--;
  switch (row)
  {
    case 1:
      /* Set cursor to 1st row address and add index*/
      lcd_send_command(column |= DDRAM_FIRST_LINE_BASE_ADDR);
      break;
    case 2:
      /* Set cursor to 2nd row address and add index*/
        lcd_send_command(column |= DDRAM_SECOND_LINE_BASE_ADDR);
      break;
    default:
      break;
  }
}

//call this function in order to make LCD latch the data lines in to its internal registers.
void lcd_enable(void)
{
    gpio_write_value(GPIO_69_P8_9_EN_6,LCD_ENABLE);
    usleep(2 * 1000); //2ms delay
    gpio_write_value(GPIO_69_P8_9_EN_6,LCD_DISABLE);

}


void lcd_print_char(uint8_t ascii_Value)
{

    gpio_write_value(GPIO_66_P8_7_RS_4,USER_DATA_MODE);

    //first send the msb

    uint8_t data_msb = ((ascii_Value >> 4) & 0X0f ); // d7 d7 d5 d4

    gpio_write_value(LCD_DATABIT4,(data_msb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(data_msb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(data_msb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(data_msb & ( 1 << 3) ));

    lcd_enable();


    uint8_t data_lsb = (( ascii_Value & 0x0f )); // d3 d2 d1 d0

    gpio_write_value(LCD_DATABIT4,(data_lsb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(data_lsb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(data_lsb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(data_lsb & ( 1 << 3) ));

    lcd_enable();

    usleep(5 * 1000);
}

void lcd_print_string(char *message)
{

      do
      {
          lcd_print_char((uint8_t)*message++);
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

    uint8_t command_msb = ((command >> 4) & 0X0f ); // d7 d6 d5 d4

    gpio_write_value(LCD_DATABIT4,(command_msb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(command_msb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(command_msb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(command_msb & ( 1 << 3) ));

    lcd_enable();

    //now send the lsb

    uint8_t command_lsb = (( command & 0x0f )); // d3 d2 d1 d0

    gpio_write_value(LCD_DATABIT4,(command_lsb & ( 1 << 0) ));
    gpio_write_value(LCD_DATABIT5,(command_lsb & ( 1 << 1) ));
    gpio_write_value(LCD_DATABIT6,(command_lsb & ( 1 << 2) ));
    gpio_write_value(LCD_DATABIT7,(command_lsb & ( 1 << 3) ));

    lcd_enable();

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
