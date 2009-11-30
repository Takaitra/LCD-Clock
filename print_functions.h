#include "rtc_common.h"
#ifndef PRINT_FUNCTIONS_H
#define PRINT_FUNCTIONS_H
char *digit_string (uint16_t digits);

void print_date (time t);
void print_weekday (time t);
void print_month (time t);
void print_blank_month (void);
void print_day (time t);
void print_blank_day (void);
void print_year (time t);
void print_blank_year (void);
void print_time (time t);
void print_hour (time t);
void print_blank_hour (void);
void print_minute (time t);
void print_blank_minute (void);
void print_second (time t);
void print_blank_second (void);
void print_menu (uint8_t menu);
void print_arrows (uint8_t row);
void print_set_time (time tempTime);
void print_set_alarm1 (time tempTime);
#endif
