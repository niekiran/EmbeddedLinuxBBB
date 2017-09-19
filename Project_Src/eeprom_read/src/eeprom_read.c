/*
 ============================================================================
 Name        : eeprom_read.c
 Author      : Kiran
 Version     :
 Copyright   : Your copyright notice
 Description : TODO:Read/Modify and display  BBB's EEPROM Contents
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

/*32KB EEPROM
A single 32KB EEPROM is provided on I2C0 that holds the board information. This
information includes board name, serial number, and revision information. This will be
the same as found on the original BeagleBone. It has a test point to allow the device to
be programmed and otherwise to provide write protection when not grounded.
*/
int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
