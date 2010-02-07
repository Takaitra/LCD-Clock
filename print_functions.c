#include "rtc_common.h"
#include "print_functions.h"
#include <avr/pgmspace.h>
#include "lcd.h"
#include <stdlib.h>

const char monthStrings[12][4] PROGMEM = {
	{ "Jan" },
	{ "Feb" },
	{ "Mar" },
	{ "Apr" },
	{ "May" },
	{ "Jun" },
	{ "Jul" },
	{ "Aug" },
	{ "Sep" },
	{ "Oct" },
	{ "Nov" },
	{ "Dec" }
};

const char dayStrings[7][4] PROGMEM = {
	{ "Sun" },
	{ "Mon" },
	{ "Tue" },
	{ "Wed" },
	{ "Thr" },
	{ "Fri" },
	{ "Sat" },
};

const char menuStrings[3][15] PROGMEM = {
	{ "   Set Time"  },
	{ "  Set Alarm" },
	//{ "  Set Alarm2" },
	{ "    Return"   },
};

char buffer[7];

char *digit_string(uint16_t digits)
{
	if (digits == 0)
		return "00";
	else if (digits < 10) {
		utoa(digits, buffer, 10);
		// Prepend "0"
		// TODO why is strcat() b0rked?
		buffer[1] = buffer[0];
		buffer[0] = '0';
		buffer[2] = '\0';
		return buffer;
	}
	else
		return utoa(digits, buffer, 10);
}

void print_date(time t)
{
	print_date_xy(t, dateOrigin.x, dateOrigin.y);
}

void print_date_xy(time t, uint8_t x, uint8_t y)
{
	lcd_gotoxy(x, y);
	lcd_puts_p(dayStrings[get_day_of_week(t)]);
	lcd_putc(' ');
	lcd_puts_p(monthStrings[t.month - 1]);
	lcd_putc(' ');
	lcd_puts(digit_string(t.date));
	lcd_puts(", ");
	lcd_puts(digit_string(t.year));
}

void print_time(time t)
{
	print_time_xy(t, timeOrigin.x, timeOrigin.y);
}

void print_time_xy(time t, uint8_t x, uint8_t y)
{
	lcd_gotoxy(x, y);
	lcd_puts(digit_string(t.hour));
	lcd_putc(':');
	lcd_puts(digit_string(t.minute));
	lcd_putc(':');
	lcd_puts(digit_string(t.second));
}

void print_blank(uint8_t x, uint8_t y, uint8_t len)
{
	lcd_gotoxy(x, y);
	for (; len>0; len--)
		lcd_putc(' ');
}

void print_hour(time t)
{
	lcd_gotoxy(timeOrigin.x, timeOrigin.y);
	lcd_puts(digit_string(t.hour));
}

void print_minute(time t)
{
	lcd_gotoxy(timeOrigin.x + MINUTE_X_OFFSET, timeOrigin.y);
	lcd_puts(digit_string(t.minute));
}

void print_second(time t)
{
	lcd_gotoxy(timeOrigin.x + SECOND_X_OFFSET, timeOrigin.y);
	lcd_puts(digit_string(t.second));
}

void print_month(time t)
{
	lcd_gotoxy(dateOrigin.x + MONTH_X_OFFSET, dateOrigin.y + 1);
	lcd_puts_p(monthStrings[t.month - 1]);
}

void print_day(time t)
{
	lcd_gotoxy(dateOrigin.x + DAY_X_OFFSET, dateOrigin.y + 1);
	lcd_puts(digit_string(t.date));
}

void print_year(time t)
{
	lcd_gotoxy(dateOrigin.x + YEAR_X_OFFSET, dateOrigin.y + 1);
	lcd_puts(digit_string(t.year));
}

void print_menu(uint8_t menu)
{
	lcd_clrscr();
	print_arrows(0);
	lcd_gotoxy(1, 0);
	lcd_puts_p(menuStrings[menu - 1]);

	switch (menu)
	{
	case SET_TIME:
		print_time(t);
		break;
	case SET_ALARM1:
		print_time(alarmTime);
		break;
	//case SET_ALARM2:
	//	print_time(alarmTime);
	//	break;
	case SET_RETURN:
		break;
	}
}

void print_set_time(time tempTime)
{
	lcd_clrscr();
	lcd_gotoxy(1, 0);
	lcd_puts_p(menuStrings[SET_TIME - 1]);
	print_time(tempTime);
}

void print_set_date(time tempTime)
{
	lcd_clrscr();
	lcd_gotoxy(1, 0);
	lcd_puts_p(menuStrings[SET_TIME - 1]);
	print_date_xy(tempTime, 0, 1);
}

void print_set_alarm1(time tempTime)
{
	lcd_clrscr();
	lcd_gotoxy(1, 0);
	lcd_puts_p(menuStrings[SET_ALARM1 - 1]);
	print_time(tempTime);
}

void print_arrows(uint8_t row)
{
	lcd_gotoxy(0, row);
	lcd_data(0x7F);         // <--
	lcd_gotoxy(15, row);
	lcd_data(0x7E);         // -->
}
