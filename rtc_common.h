#include <inttypes.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

#ifndef RTC_COMMON_H
#define RTC_COMMON_H

#define FOSC 1000000
#define BAUD 2400
#define MYUBRR FOSC / 16 / BAUD - 1

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t) ~(1 << mask))

#define SET_TIME 1
#define SET_ALARM1 3
#define SET_ALARM2 4
#define SET_RETURN 5

#define HOUR_ITEM 1
#define MINUTE_ITEM 2
#define SECONDS_ITEM 3
#define MONTH_ITEM 4
#define DAY_ITEM 5
#define YEAR_ITEM 6
#define RETURN_ITEM 7

#define MINUTE_X_OFFSET 3
#define SECOND_X_OFFSET 6

#define MONTH_X_OFFSET 4
#define DAY_X_OFFSET 8
#define YEAR_X_OFFSET 12

#define BUTTON1 PD2
#define BUTTON2 PD1
#define BUTTON3 PD0

#define BLINK_DELAY 100
#define ALARM_INTERVAL 100
#define DEBOUNCE_DELAY 10

typedef struct {
        int8_t second;
        int8_t minute;
        int8_t hour;
        int8_t date;
        int8_t month;
        int16_t year;
        uint8_t dayOfWeek;      // LSB = Sunday, used for alarm
} time;

typedef struct {
        uint8_t x;
        uint8_t y;
} xypos;

bool is_leap_year(uint16_t year);
uint8_t get_day_of_week(time t);
void increment_date(time *t, bool carryover);
void sanitize_date(time *t);

extern time t;
extern time alarmTime;

extern xypos dateOrigin;    // Start position for date display
extern xypos timeOrigin;    // Start position for time display

/**
 * monthMods are used as part of the day of week calculation and are stored
 * in ROM
*/
extern const uint8_t monthMods[13] PROGMEM;

extern const uint8_t daysInMonth[12] PROGMEM;

#endif
