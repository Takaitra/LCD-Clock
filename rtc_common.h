#include <inttypes.h>
#include <stdbool.h>

#ifndef RTC_COMMON_H
#define RTC_COMMON_H

#define FOSC 1000000
#define BAUD 2400
#define MYUBRR FOSC / 16 / BAUD - 1

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t) ~(1 << mask))

#define SET_TIME 1
#define SET_ALARM1 2
#define SET_ALARM2 3
#define SET_RETURN 4

#define HOUR_ITEM 1
#define MINUTE_ITEM 2
#define SECONDS_ITEM 3
#define MONTH_ITEM 4
#define DAY_ITEM 5
#define YEAR_ITEM 6
#define RETURN_ITEM 7

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

extern time t;
extern time alarmTime;

#endif
