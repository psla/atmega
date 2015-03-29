// This code was originally taken form
// https://github.com/aostanin/avr-hd44780
// and further optimized to
// 1. ignore RW pin (connect RW to ground) and remove
//    reference about RW 
// 2. support "putc"
// 3. make it explicit that pins for the data bus must be next to each other

#pragma once

#include <avr/io.h>

// Edit these
#define LCD_DDR  DDRD
#define LCD_PORT PORTD

#define LCD_RS 7
#define LCD_EN 6
#define LCD_D0 2
// note, D1, D2, D3 are ignored, they must follow
// D0 (if D0 is Px0, then they have to be Px1, Px2, Px3
// if D0 is Px2, then they have to be Px3, Px4, Px5
// also, this library currently does not allow you to use other pins
// on the port where you connected screen
// and given that it was optimized to use 6 pins, you are losing 2 pins. TODO
#define LCD_D1 3
#define LCD_D2 4
#define LCD_D3 5

#define LCD_COL_COUNT 16
#define LCD_ROW_COUNT 2

// The rest should be left alone
#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME     0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT    0x10
#define LCD_FUNCTIONSET    0x20
#define LCD_SETCGRAMADDR   0x40
#define LCD_SETDDRAMADDR   0x80

#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON  0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON   0x02
#define LCD_CURSOROFF  0x00
#define LCD_BLINKON    0x01
#define LCD_BLINKOFF   0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE    0x08
#define LCD_1LINE    0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS  0x00

void lcd_init(void);

void lcd_command(uint8_t command);
void lcd_write(uint8_t value);

void lcd_on(void);
void lcd_off(void);

void lcd_clear(void);
void lcd_return_home(void);

void lcd_enable_blinking(void);
void lcd_disable_blinking(void);

void lcd_enable_cursor(void);
void lcd_disable_cursor(void);

void lcd_scroll_left(void);
void lcd_scroll_right(void);

void lcd_set_left_to_right(void);
void lcd_set_right_to_left(void);

void lcd_enable_autoscroll(void);
void lcd_disable_autoscroll(void);

void lcd_create_char(uint8_t location, uint8_t *charmap);

void lcd_set_cursor(uint8_t row, uint8_t col);

void lcd_puts(char *string);
void lcd_printf(char *format, ...);

#define lcd_putc lcd_write
#define lcd_clrscr lcd_clear