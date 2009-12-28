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

const char menuStrings[4][15] PROGMEM = {
	{ "   Set Time"  },
	{ "  Set Alarm1" },
	{ "  Set Alarm2" },
	{ "    Return"   },
};

xypos dateOrigin = { 0, 0 };    // Start position for date display
xypos timeOrigin = { 4, 1 };    // Start position for time display

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
	lcd_gotoxy(dateOrigin.x, dateOrigin.y);
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
	lcd_gotoxy(timeOrigin.x, timeOrigin.y);
	lcd_puts(digit_string(t.hour));
	lcd_putc(':');
	lcd_puts(digit_string(t.minute));
	lcd_putc(':');
	lcd_puts(digit_string(t.second));
}

void print_hour(time t)
{
	lcd_gotoxy(timeOrigin.x, timeOrigin.y);
	lcd_puts(digit_string(t.hour));
}

void print_blank_hour()
{
	lcd_gotoxy(timeOrigin.x, timeOrigin.y);
	lcd_puts("  ");
}

void print_minute(time t)
{
	lcd_gotoxy(timeOrigin.x + 3, timeOrigin.y);
	lcd_puts(digit_string(t.minute));
}

void print_blank_minute()
{
	lcd_gotoxy(timeOrigin.x + 3, timeOrigin.y);
	lcd_puts("  ");
}

void print_second(time t)
{
	lcd_gotoxy(timeOrigin.x + 6, timeOrigin.y);
	lcd_puts(digit_string(t.second));
}

void print_blank_second()
{
	lcd_gotoxy(timeOrigin.x + 6, timeOrigin.y);
	lcd_puts("  ");
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
	case SET_ALARM2:
		print_time(alarmTime);
		break;
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
