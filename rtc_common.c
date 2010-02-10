#include <rtc_common.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

void increment_date(time *t, bool carryover)
{
	if (++t->date == 32) {
		t->date = 1;
		if (carryover)
			t->month++;
	}
	else if (t->date == 31) {
		if ((t->month == 4) || (t->month == 6) || (t->month == 9)
			|| (t->month == 11)) {
			t->date = 1;
			if (carryover)
				t->month++;
		}
	}
	else if (t->date == 30) {
		if (t->month == 2) {
			t->date = 1;
			if (carryover)
				t->month++;
		}
	}
	else if (t->date == 29) {
		if ((t->month == 2) && (!is_leap_year(t->year))) {
			t->date = 1;
			if (carryover)
				t->month++;
		}
	}
	if (t->month == 13) {
		t->month = 1;
		t->year++;
	}
}

void sanitize_date(time *t)
{
	while (t->date > &daysInMonth[t->month]) {
		t->date--;
	}
}

/****************************************************************
 * GetDayofWeek
 * This routine calculate the Day(Sunday, Monday,...Saturday) of
 * week when a Date(year, Month, Day) is given.
 * Input : Year, Month and Day which in this routine is used
 * as global variable.
 * Output Variable : 0 to 6(which correspond to Sunday to Saturdaday
 * respectively) if the input is acceptable, else a value 8 is return
 *****************************************************************/
uint8_t get_day_of_week(time t)
{
	uint16_t tempYear;
	uint8_t accValue;

	// TODO validate input or extend code to work with any date
	//if(CheckValidInput(CurrentYear)!=OK)/* Return Error if input not Valid*/
	//return Error;
	tempYear = 2007;        // Comparation start with year 2007
	accValue = 0;
	/* If TempYear is a leap year AccValue +2, else AccValue+1 */
	while (tempYear != t.year) {
		accValue++;
		if (is_leap_year(tempYear))
			accValue++;
		tempYear++;
	}
	if (t.month > 2)
		if (is_leap_year(tempYear) == 1)
			accValue++;

	accValue += pgm_read_byte(&monthMods[t.month]);
	accValue += t.date;
	accValue = accValue % 7;

	return accValue;
}

bool is_leap_year(uint16_t year)    // Check for leap year
{
	if (((year % 4) == 0 && (year % 100) != 0) || (year % 400) == 0)
		return true;
	else
		return false;
}
