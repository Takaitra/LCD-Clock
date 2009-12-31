#include "rtc_common.h"
#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H
char *digit_string(uint16_t digits);

void print_date(time t);
void print_date_xy(time t, uint8_t x, uint8_t y);
void print_weekday(time t);
void print_month(time t);
void print_day(time t);
void print_year(time t);
void print_time(time t);
void print_time_xy(time t, uint8_t x, uint8_t y);
void print_hour(time t);
void print_minute(time t);
void print_second(time t);
void print_blank(uint8_t x, uint8_t y, uint8_t len);
void print_menu(uint8_t menu);
void print_arrows(uint8_t row);
void print_set_time(time tempTime);
void print_set_alarm1(time tempTime);

extern xypos dateOrigin = { 0, 0 };    // Start position for date display
extern xypos timeOrigin = { 4, 1 };    // Start position for time display

#endif
