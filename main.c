/*
    File:       main.c
    Version:    0.1
    Date:       November 1, 2009
    Author:     Matthew Toso
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

void ioinit (void);			// initializes IO
void timerinit (void);
void adcinit (void);

void adjust_backlight (void);

inline void alarm_fire (void);
inline void alarm_stop (void);

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

int
main (void)
{
  // init time
  t.year = 2009;
  t.month = 10;
  t.date = 14;
  t.hour = 22;
  t.minute = 58;
  t.second = 30;

  ioinit ();			//Setup IO pins and defaults
  timerinit ();
  adcinit ();

  lcd_init (LCD_DISP_ON);
  lcd_clrscr ();
  print_date (t);
  print_time (t);
  lcd_home ();

  /* Enable global interrupts */
  sei ();

  //set_sleep_mode (SLEEP_MODE_PWR_SAVE);
  set_sleep_mode (SLEEP_MODE_IDLE);

  /* loop forever */
  while (1)
    {
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

void
debug_byte (uint8_t byte)
{
  for (int bit = 7; bit >= 0; bit--)
    {
      if (byte & 1 << bit)
	lcd_putc ('1');
      else
	lcd_putc ('0');
    }
}

ISR (PCINT2_vect)
{
  // Pin change interrupt. Executed on each button press.

  // Debounce the button inputs
  if (debounceCounter)
    return;
  // if alarm is fired, stop alarm and return.
  if (alarmFired)
  {
    alarm_stop();
    return;
  }
  // if button 1 pressed and we are in setup
  if ((~PIND & _BV (BUTTON1)) && setup)
    {
      if (setTime)
      {
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
        }
      }
      else
      {
        // previous menu
        if (menu == 1)
          menu = SET_RETURN;
        else
	  menu--;
        print_menu (menu);
      }
    }
  // if button 2 pressed
  if (~PIND & _BV (BUTTON2))
    {
      if (setup)
	{
	switch (menu)
	  {
	  case SET_TIME:
            if (setTime)
            {
	      if (menuItem == SECONDS_ITEM)
	      {
                t = tempTime;
	        setTime = false;
                print_menu (menu);
              }
	      else
	      {
                menuItem++;
		print_set_time(tempTime);
	      }
            }
            else
            {
              setTime = true;
	      tempTime = t;
	      menuItem = HOUR_ITEM;
	      print_set_time (tempTime);
            }
	  break;
	  case SET_ALARM1:
            if (setTime)
            {
	      if (menuItem == SECONDS_ITEM)
	      {
                alarmTime = tempTime;
	        setTime = false;
                print_menu (menu);
              }
	      else
	      {
                menuItem++;
		print_set_alarm1 (tempTime);
	      }
            }
            else
            {
              setTime = true;
	      tempTime = alarmTime;
	      menuItem = HOUR_ITEM;
	      print_set_alarm1 (tempTime);
            }
	    break;
	  case SET_RETURN:
	      setup = false;
	      print_date (t);
	      print_time (t);
	    break;
	  }
	}
      else
	{
	  // enter setup menu
	  menu = 1;
	  setup = true;
	  print_menu (menu);
	}
    }
  // if button 3 pressed and we are in setup
  if ((~PIND & _BV (BUTTON3)) && setup)
    {
      if (setTime)
      {
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
        }
      }
      else
      {
        // advance menu
        menu = menu % SET_RETURN;
        menu++;
        print_menu (menu);
      }
    }
    debounceCounter = DEBOUNCE_DELAY;
}

//ISR (TIMER1_OVF_vect)

// Timer 1 compare match A interrupt.
// Occurs once every 6ms and is used for timed events
// (except for clock ticks, which is handled by timer 2).
ISR (TIMER0_COMPB_vect)
{
  if (setTime)
  {
    blinkCounter++;
    if (blinkCounter == BLINK_DELAY)
    {
      // blink off
      switch (menuItem)
      {
        case HOUR_ITEM:
          print_blank_hour ();
	  break;
        case MINUTE_ITEM:
          print_blank_minute ();
          break;
        case SECONDS_ITEM:
          print_blank_second ();
          break;
      }
    }
    if (blinkCounter == 2 * BLINK_DELAY)
    {
      blinkCounter = 0;
      // blink on
      switch (menuItem)
      {
        case HOUR_ITEM:
          print_hour (tempTime);
	  break;
        case MINUTE_ITEM:
          print_minute (tempTime);
          break;
        case SECONDS_ITEM:
          print_second (tempTime);
          break;
      }
    }
  }

    if (alarmFired)
  {
    alarmCounter++;
    if (alarmCounter == ALARM_INTERVAL)
      sbi (TCCR1B, CS10); // buzzer on
    if (alarmCounter == 2 * ALARM_INTERVAL)
    {
      alarmCounter = 0;
      cbi (TCCR1B, CS10); // buzzer off
    }
  }

  if (debounceCounter)
    debounceCounter--;
}

// Timer 2 overflow interrupt
// Occurs on each clock tick
ISR (TIMER2_OVF_vect)
{
  if (++t.second == 60)		//keep track of time, date, month, and year
    {
      t.second = 0;
      if (++t.minute == 60)
	{
	  t.minute = 0;
	  if (++t.hour == 24)
	    {
	      t.hour = 0;
	      if (++t.date == 32)
		{
		  t.month++;
		  t.date = 1;
		}
	      else if (t.date == 31)
		{
		  if ((t.month == 4) || (t.month == 6) || (t.month == 9)
		      || (t.month == 11))
		    {
		      t.month++;
		      t.date = 1;
		    }
		}
	      else if (t.date == 30)
		{
		  if (t.month == 2)
		    {
		      t.month++;
		      t.date = 1;
		    }
		}
	      else if (t.date == 29)
		{
		  if ((t.month == 2) && (!is_leap_year (t.year)))
		    {
		      t.month++;
		      t.date = 1;
		    }
		}
	      if (t.month == 13)
		{
		  t.month = 1;
		  t.year++;
		}
	      if (!setup)
		print_date (t);
	    }
	  if (!setup)
	    print_hour (t);
	}
      if (!setup)
	print_minute (t);
    }
  if (!setup)
    print_second (t);

  //PORTB ^= _BV (PC1);
  adjust_backlight ();
  
  if (t.hour == alarmTime.hour
      && t.minute == alarmTime.minute
      && t.second == alarmTime.second)
  {
    alarm_fire ();
  }
}

void
adjust_backlight ()
{
  if (ADC < 200)
      // Brightest setting
      OCR0B = 80;
  else if (ADC > 500)
      // Dimmest setting
      OCR0B = 8;
  else
      // Variable brightness between 8 and 80
      OCR0B = 128 - (0.24*ADC);
  // Begin next ADC conversion
  ADCSRA |= _BV (ADIF);
  ADCSRA |= _BV (ADSC);
}

void
ioinit (void)
{
  //1 = output, 0 = input
  DDRB = 0b00111011;		// PORTB (LCD control lines) PB0 = alarm
  DDRC = 0b00001111;		// PORTC (ADC on ADC5, LCD DATA on PC0:3)
  DDRD = 0b00100000;		// PORTD (OC0B = backlight PWM)

  PORTD = 0b00000111;		// Enable pullups on PD0,1,2

  PCICR |= _BV (PCIE2);		// Enable pin change interrupts for PCINT23..16
  PCMSK2 |= 0b00000111;	// Enable change interrupts only on PD0, PD1, PD2
}

void
timerinit ()
{
  /* Disable interrupts for timer 2 */
  TIMSK2 &= ~(_BV (TOIE2) | _BV (OCIE2A) | _BV (OCIE2B));

  /* Set timer/counter 2 to be asynchronous using the external crystal */
  ASSR |= _BV (AS2);

  /* Write new values to the timer and output compare registers */
  TCNT2 = 0;
  OCR2A = 0;
  OCR2B = 0;

  /* Set prescaler to 128 */
  TCCR2B = _BV (CS22) | _BV (CS20);

  /* Wait for TCN2UB, OCR2UB, and TCR2UB to be cleared */
  while (ASSR &
	 (_BV (TCN2UB) | _BV (OCR2AUB) | _BV (OCR2BUB) | _BV (TCR2AUB) |
	  _BV (TCR2BUB)));

  /* Enable overflow interrupt for timer 2 */
  TIMSK2 |= _BV (TOIE2);

  /*** Configure timer 1 for creating the buzzer 2048Hz square wave ***/
  // Fast PWM with ICR1 as TOP (mode 14). Use OC1A pin for output.
  TCCR1A = _BV (COM1A1) | _BV (WGM11);
  TCCR1B = _BV (WGM13) | _BV (WGM12);
  // Enable overflow interrupt
  //TIMSK1 = _BV (OCIE1A);
  // Set to overflow at 2048Hz
  OCR1A = 244;
  ICR1 = 488;
  // Start timer 1 with no prescaler
  //TCCR1B |= _BV (CS10);

  /***Configure timer 0 for backlight PWM and general event timing***/
  /// Fast PWM (mode 7) with OCR0A as TOP
  TCCR0A = _BV (COM0B1) | _BV (WGM00) | _BV (WGM01);
  TCCR0B = _BV (WGM02);
  // Overflow at 80 (195 Hz, 5.1 ms)
  OCR0A = 80;
  // Start LCD at full brightness
  OCR0B = 80;
  // Enable overflow interrupt
  TIMSK0 = _BV (OCIE0B);
  // Start timer 0 with clk/64 prescaler
  TCCR0B |= _BV (CS00) | _BV (CS01);
}

void
adcinit ()
{
  // ADC multiplexer selection (ADC5)
  ADMUX = 5;
  // Voltage reference is AVCC
  ADMUX |= _BV (REFS0);
  // Set auto trigger on Timer1 overflow
  //ADCSRB |= _BV (ADTS2) | _BV (ADTS1);
  // ADC Enable, set prescaler to 8
  ADCSRA |= _BV (ADEN) | _BV (ADPS1) | _BV (ADPS0);
  // Start the first conversion/init the ADC
  ADCSRA |= _BV (ADSC);
  // Enable auto trigger
  //ADCSRA |= _BV (ADATE);
}

inline void alarm_fire (void)
{
  alarmFired = true;
  sbi (TCCR1B, CS10);
}

inline void alarm_stop (void)
{
  alarmFired = false;
  cbi (TCCR1B, CS10);
  cbi (PORTB, PB1);
}
