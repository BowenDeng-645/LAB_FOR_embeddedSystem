#include <platform.h>
#include <stdint.h>
#include <gpio.h>
#include "lcd.h"
#include "delay.h"

#define COLUMN_NUM 16
#define ROW_NUM 2
#define LCD_BUSY_FLAG_MASK 0x80

Pin P_LCD_DATA[4] = {P_LCD_DATA4, P_LCD_DATA5, P_LCD_DATA6, P_LCD_DATA7};

void 				lcd_write_4bit(uint8_t c);
void 				lcd_write_cmd(uint8_t c);
static void wait_while_busy(void);
static void set_data_dir(PinMode mode);

int char_num = 0;
////////////////////////
//PIN references
/*
P_LCD_RS		PA_9
P_LCD_E 		PC_7

P_LCD_DATA4 PB_5
P_LCD_DATA5 PB_4
P_LCD_DATA6 PB_10
P_LCD_DATA7 PA_8
*/
/////////////////////////////

void lcd_init(void) 
	{
		// STEP 1: Configure GPIO Pins
		gpio_set_mode(P_LCD_RS, Output);
		gpio_set_mode(P_LCD_E,  Output);
		set_data_dir(Output);
		delay_ms(100);

		// STEP 2: Force LCD into 8-bit mode (Reset Sequence)
		lcd_write_4bit(0x3);
		delay_ms(5);
		lcd_write_4bit(0x3);
		delay_us(100);
		lcd_write_4bit(0x3);
		delay_us(100);

		// STEP 3: Switch to 4-bit mode
		lcd_write_4bit(0x2);
		delay_us(100);

		// STEP 4: Configure LCD Function: 4-bit, 2 lines, 5x8 font
		lcd_write_cmd(0x28);
		delay_us(60);

		// STEP 5: Turn Display OFF
		lcd_write_cmd(0x08);
		delay_us(60);

		// STEP 6: Clear Display
		lcd_write_cmd(0x01);
		delay_ms(4);

		// STEP 7: Set Entry Mode: increment cursor, no shift
		lcd_write_cmd(0x06);
		delay_us(60);

		// STEP 8: Turn Display ON
		lcd_write_cmd(0x0C);
		delay_us(60);

		// STEP 9: Final Cursor Configuration
		lcd_set_cursor(0, 0);
		lcd_set_cursor_visible(0);
	}

void lcd_set_cursor_visible(int visible) {
	lcd_write_cmd(0x0C | ((!!visible) << 1));
}

void lcd_write_4bit(uint8_t c) {
	int i = 0;
//	gpio_set(P_LCD_RW, 0);
	gpio_set(P_LCD_E, 1);
	
	for(i=0; i<4; i++) {
		gpio_set(P_LCD_DATA[i], (c & (0x1<<i))>>i);
	}
	
	delay_us(1);
	gpio_set(P_LCD_E, 0);
	delay_us(1);
}

static void lcd_write_data(uint8_t c) 
	{
		// STEP 1: Ensure LCD is Ready
		wait_while_busy();

		// STEP 2: Select Data Register (RS = 1)
		gpio_set(P_LCD_RS, 1);

		// STEP 3: Send Upper 4 Bits (High Nibble)
		lcd_write_4bit((c >> 4) & 0x0F);

		// STEP 4: Send Lower 4 Bits (Low Nibble)
		lcd_write_4bit(c & 0x0F);
	}

void lcd_write_cmd(uint8_t c) 
	{
		// STEP 1: Ensure LCD is Ready
		wait_while_busy();

		// STEP 2: Select Command Register (RS = 0)
		gpio_set(P_LCD_RS, 0);

		// STEP 3: Send Upper 4 Bits (High Nibble)
		lcd_write_4bit((c >> 4) & 0x0F);

		// STEP 4: Send Lower 4 Bits (Low Nibble)
		lcd_write_4bit(c & 0x0F);
	}

void lcd_set_cursor(int column, int row){
		int address;
		address = (row * 0x40) + column;
		address |= 0x80;
		lcd_write_cmd(address);
		char_num = column;
	}

void lcd_clear(void){
		lcd_write_cmd(0x01);
		delay_ms(10);
		lcd_write_cmd(0x02);
		delay_ms(10);
		lcd_set_cursor(0, 0);
	}

void lcd_put_char(char c) {
	lcd_write_data(c);
    //delay_ms(10);
}

void lcd_print(char *string) {
	while(*string) {
		if(char_num == COLUMN_NUM){
			lcd_set_cursor(0, 1); // auto change row
		}
        else if (*string == '\n'){
            lcd_set_cursor(0, 1); // manual change row
            string++; // skip the newline char
        }
        lcd_put_char(*string++);
        char_num++;
	}
}

static void wait_while_busy(void) {
	//while(lcd_read_status() & LCD_BUSY_FLAG_MASK);
    //delay_ms(100);
    delay_ms(10);
    //return;
}

static void set_data_dir(PinMode mode) {
	uint8_t i;
	for (i = 0; i < 4; i++) {
		gpio_set_mode((Pin)(P_LCD_DATA[i]), mode);
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2016*************************************
