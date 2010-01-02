/**
 * File:         main.c
 * Version:      0.3
 * Date Created: 11/01/2009
 * Last Updated: 12/29/2009
 * Author:       Matthew Toso
 *
 * Copyright 2009 Matthew Toso
 *
 * LCD Clock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCD Clock.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "lcd.h"
#include "print_functions.h"
#include "rtc_common.h"

void ioinit(void);
void timerinit(void);
void adcinit(void);

void adjust_backlight(void);

inline void alarm_fire(void);
inline void alarm_stop(void);

time t;
time tempTime;
time alarmTime;

bool setup = false;
bool setTime = false;
bool alarmFired = false;
uint8_t menu;
uint8_t menuItem;
uint8_t brightness = 5;
uint8_t blinkCounter;
uint8_t debounceCounter;
uint16_t alarmCounter;

xypos dateOrigin = { 0, 0 };    // Start position for date display
xypos timeOrigin = { 4, 1 };    // Start position for time display

const uint8_t monthMods[13] PROGMEM =
{ 0, 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };

const uint8_t daysInMonth[12] PROGMEM =
{ 31, 28, 31, 30, 30, 30, 31, 31, 30, 31, 30, 31 };

int main(void)
{
	// init time
	t.year = 2009;
	t.month = 11;
	t.date = 29;
	t.hour = 22;
	t.minute = 58;
	t.second = 30;

	ioinit();
	timerinit();
	adcinit();

	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	print_date(t);
	print_time(t);
	lcd_home();

	/* Enable global interrupts */
	sei();

	//set_sleep_mode (SLEEP_MODE_PWR_SAVE);
	set_sleep_mode(SLEEP_MODE_IDLE);

	/* loop forever */
	while (1) {
		//_delay_ms (250);
		//lcd_home ();
		//lcd_puts("whileloop");
		//sleep_mode ();
		//while (ASSR &
		//     (_BV (TCN2UB) | _BV (OCR2AUB) | _BV (OCR2BUB) | _BV (TCR2AUB) |
		//      _BV (TCR2BUB)));

		//lcd_home ();
		//lcd_puts( digit_string (i) );
	}
}

void debug_byte(uint8_t byte)
{
	for (int bit = 7; bit >= 0; bit--) {
		if (byte & 1 << bit)
			lcd_putc('1');
		else
			lcd_putc('0');
	}
}

/**
 * Pin change interrupt. Executed on each button press.
 */

ISR(PCINT2_vect)
{
	// Debounce the button inputs
	if (debounceCounter)
		return;
	// if alarm is fired, stop alarm and return.
	if (alarmFired) {
		alarm_stop();
		return;
	}
	// if button 1 pressed and we are in setup
	if ((~PIND & _BV(BUTTON1)) && setup) {
		if (setTime) {
			switch (menuItem)
			{
			case HOUR_ITEM:
				if (--tempTime.hour < 0)
					tempTime.hour = 23;
				print_hour(tempTime);
				break;
			case MINUTE_ITEM:
				if (--tempTime.minute < 0)
					tempTime.minute = 59;
				print_minute(tempTime);
				break;
			case SECONDS_ITEM:
				if (--tempTime.second < 0)
					tempTime.second = 59;
				print_second(tempTime);
				break;
			case MONTH_ITEM:
				if (--tempTime.month < 1)
					tempTime.date = 12;
				sanitize_date(&tempTime);
				print_date_xy(tempTime, 0, 1);
				break;
			case DAY_ITEM:
				if (--tempTime.date < 1)
					if (tempTime.month == 2 && is_leap_year(tempTime.year))
						tempTime.date = 29;
					else
						tempTime.date = &daysInMonth[tempTime.month];
				print_date_xy(tempTime, 0, 1);
				break;
			case YEAR_ITEM:
				if (tempTime.year > 2007)
					--tempTime.year;
				print_date_xy(tempTime, 0, 1);
				break;
			}
		}
		else{
			// previous menu
			if (menu == 1)
				menu = SET_RETURN;
			else
				menu--;
			print_menu(menu);
		}
	}
	// if button 2 pressed
	if (~PIND & _BV(BUTTON2)) {
		if (setup) {
			switch (menu)
			{
			case SET_TIME:
				if (setTime) {
					switch (menuItem)
					{
					case HOUR_ITEM:
					case MINUTE_ITEM:
						menuItem++;
						print_set_time(tempTime);
						break;
					case SECONDS_ITEM:
					case MONTH_ITEM:
					case DAY_ITEM:
						menuItem++;
						print_set_date(tempTime);
						break;
					case YEAR_ITEM:
						t = tempTime;
						setTime = false;
						print_menu(menu);
						break;
					}
				}
				else {
					setTime = true;
					tempTime = t;
					menuItem = HOUR_ITEM;
					print_set_time(tempTime);
				}
				break;
			case SET_ALARM1:
				if (setTime) {
					if (menuItem == SECONDS_ITEM) {
						alarmTime = tempTime;
						setTime = false;
						print_menu(menu);
					}
					else{
						menuItem++;
						print_set_alarm1(tempTime);
					}
				}
				else{
					setTime = true;
					tempTime = alarmTime;
					menuItem = HOUR_ITEM;
					print_set_alarm1(tempTime);
				}
				break;
			case SET_RETURN:
				setup = false;
				print_date(t);
				print_time(t);
				break;
			}
		}
		else{
			// enter setup menu
			menu = 1;
			setup = true;
			print_menu(menu);
		}
	}
	// if button 3 pressed and we are in setup
	if ((~PIND & _BV(BUTTON3)) && setup) {
		if (setTime) {
			switch (menuItem)
			{
			case HOUR_ITEM:
				if (++tempTime.hour > 23)
					tempTime.hour = 0;
				print_hour(tempTime);
				break;
			case MINUTE_ITEM:
				if (++tempTime.minute > 59)
					tempTime.minute = 0;
				print_minute(tempTime);
				break;
			case SECONDS_ITEM:
				if (++tempTime.second > 59)
					tempTime.second = 0;
				print_second(tempTime);
				break;
			case MONTH_ITEM:
				if (++tempTime.month > 12)
					tempTime.month = 1;
				sanitize_date(&tempTime);
				print_date_xy(tempTime, 0, 1);
				break;
			case DAY_ITEM:
				increment_date(&tempTime, false);
				print_date_xy(tempTime, 0, 1);
				break;
			case YEAR_ITEM:
				tempTime.year++;
				print_date_xy(tempTime, 0, 1);
				break;
			}
		}
		else{
			// advance menu
			menu = menu % SET_RETURN;
			menu++;
			print_menu(menu);
		}
	}
	debounceCounter = DEBOUNCE_DELAY;
}

/**
 * Timer 1 compare match B interrupt.
 * Occurs once every 6ms and is used for timed events
 * (except for clock ticks, which is handled by timer 2).
 */
ISR(TIMER0_COMPB_vect)
{
	if (setTime) {
		blinkCounter++;
		if (blinkCounter == BLINK_DELAY) {
			// blink off
			switch (menuItem)
			{
			case HOUR_ITEM:
				print_blank(timeOrigin.x, timeOrigin.y, 2);
				break;
			case MINUTE_ITEM:
				print_blank(timeOrigin.x + MINUTE_X_OFFSET, timeOrigin.y, 2);
				break;
			case SECONDS_ITEM:
				print_blank(timeOrigin.x + SECOND_X_OFFSET, timeOrigin.y, 2);
				break;
			case MONTH_ITEM:
				print_blank(dateOrigin.x + MONTH_X_OFFSET, 1, 2);
				break;
			case DAY_ITEM:
				print_blank(dateOrigin.x + DAY_X_OFFSET, 1, 2);
				break;
			case YEAR_ITEM:
				print_blank(dateOrigin.x + YEAR_X_OFFSET, 1, 2);
				break;
			}
		}
		if (blinkCounter == 2 * BLINK_DELAY) {
			blinkCounter = 0;
			// blink on
			switch (menuItem)
			{
			case HOUR_ITEM:
				print_hour(tempTime);
				break;
			case MINUTE_ITEM:
				print_minute(tempTime);
				break;
			case SECONDS_ITEM:
				print_second(tempTime);
				break;
			case MONTH_ITEM:
				print_month(tempTime);
				break;
			case DAY_ITEM:
				print_day(tempTime);
				break;
			case YEAR_ITEM:
				print_year(tempTime);
				break;
			}
		}
	}

	if (alarmFired) {
		alarmCounter++;
		if (alarmCounter == ALARM_INTERVAL)
			sbi(TCCR1B, CS10); // buzzer on
		if (alarmCounter == 2 * ALARM_INTERVAL) {
			alarmCounter = 0;
			cbi(TCCR1B, CS10); // buzzer off
		}
	}

	if (debounceCounter)
		debounceCounter--;
}

/**
 * Timer 2 overflow interrupt
 * Occurs on each clock tick and is used to keep track of the time, date,
 * month, and year.
 */
ISR(TIMER2_OVF_vect)
{
	if (++t.second == 60) {
		t.second = 0;
		if (++t.minute == 60) {
			t.minute = 0;
			if (++t.hour == 24) {
				t.hour = 0;
				increment_date(&t, true);
				if (!setup)
					print_date(t);
			}
			if (!setup)
				print_hour(t);
		}
		if (!setup)
			print_minute(t);
	}
	if (!setup)
		print_second(t);

	adjust_backlight();

	if (t.hour == alarmTime.hour
	    && t.minute == alarmTime.minute
	    && t.second == alarmTime.second)
		alarm_fire();
}

void adjust_backlight()
{
	if (ADC < 200)
		// Brightest setting
		OCR0B = 80;
	else if (ADC > 500)
		// Dimmest setting
		OCR0B = 8;
	else
		// Variable brightness between 8 and 80
		OCR0B = 128 - (0.24 * ADC);
	// Begin next ADC conversion
	ADCSRA |= _BV(ADIF);
	ADCSRA |= _BV(ADSC);
}

/**
 * Initialize inputs and outputs.
 * 1 = output, 0 = input
*/
void ioinit(void)
{
	DDRB = 0b00111011;      // PORTB (LCD control lines) PB0 = alarm
	DDRC = 0b00001111;      // PORTC (ADC on ADC5, LCD DATA on PC0:3)
	DDRD = 0b00100000;      // PORTD (OC0B = backlight PWM)

	PORTD = 0b00000111;     // Enable pullups on PD0,1,2

	PCICR |= _BV(PCIE2);    // Enable pin change interrupts for PCINT23..16
	PCMSK2 |= 0b00000111;   // Enable change interrupts only on PD0, PD1, PD2
}

/**
 * Initialize timers for timekeeping and PWM.
*/
void timerinit()
{
	/* Disable interrupts for timer 2 */
	TIMSK2 &= ~(_BV(TOIE2) | _BV(OCIE2A) | _BV(OCIE2B));

	/* Set timer/counter 2 to be asynchronous using the external crystal */
	ASSR |= _BV(AS2);

	/* Write new values to the timer and output compare registers */
	TCNT2 = 0;
	OCR2A = 0;
	OCR2B = 0;

	/* Set prescaler to 128 */
	TCCR2B = _BV(CS22) | _BV(CS20);

	/* Wait for TCN2UB, OCR2UB, and TCR2UB to be cleared */
	while (ASSR &
	       (_BV(TCN2UB) | _BV(OCR2AUB) | _BV(OCR2BUB) | _BV(TCR2AUB) |
	        _BV(TCR2BUB))) ;

	/* Enable overflow interrupt for timer 2 */
	TIMSK2 |= _BV(TOIE2);

	/*** Configure timer 1 for creating the buzzer 2048Hz square wave ***/
	// Fast PWM with ICR1 as TOP (mode 14). Use OC1A pin for output.
	TCCR1A = _BV(COM1A1) | _BV(WGM11);
	TCCR1B = _BV(WGM13) | _BV(WGM12);
	// Enable overflow interrupt
	//TIMSK1 = _BV (OCIE1A);
	// Set to overflow at 2048Hz
	OCR1A = 244;
	ICR1 = 488;
	// Start timer 1 with no prescaler
	//TCCR1B |= _BV (CS10);

	/***Configure timer 0 for backlight PWM and general event timing***/
	/// Fast PWM (mode 7) with OCR0A as TOP
	TCCR0A = _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);
	TCCR0B = _BV(WGM02);
	// Overflow at 80 (195 Hz, 5.1 ms)
	OCR0A = 80;
	// Start LCD at full brightness
	OCR0B = 80;
	// Enable overflow interrupt
	TIMSK0 = _BV(OCIE0B);
	// Start timer 0 with clk/64 prescaler
	TCCR0B |= _BV(CS00) | _BV(CS01);
}

/**
 * Initialize the analog to digital converter for the photoresistor.
*/
void adcinit()
{
	// ADC multiplexer selection (ADC5)
	ADMUX = 5;
	// Voltage reference is AVCC
	ADMUX |= _BV(REFS0);
	// Set auto trigger on Timer1 overflow
	//ADCSRB |= _BV (ADTS2) | _BV (ADTS1);
	// ADC Enable, set prescaler to 8
	ADCSRA |= _BV(ADEN) | _BV(ADPS1) | _BV(ADPS0);
	// Start the first conversion/init the ADC
	ADCSRA |= _BV(ADSC);
	// Enable auto trigger
	//ADCSRA |= _BV (ADATE);
}

inline void alarm_fire(void)
{
	alarmFired = true;
	sbi(TCCR1B, CS10);
}

inline void alarm_stop(void)
{
	alarmFired = false;
	cbi(TCCR1B, CS10);
	cbi(PORTB, PB1);
}
