#include "fw/drivers/rtc.h"

#include <minix/sysutil.h>
#include <stdio.h>

int main(void) {
  sef_startup();

  freopen("/dev/console", "w", stdout);
  setvbuf(stdout, NULL, _IONBF, 0);

  rtc_date date;
  if (rtc_read_date(&date) != 0) {
    printf("RTC read failed\n");
    return 1;
  }

  printf("RTC date: %02u/%02u/%02u\n", date.day, date.month, date.year);
  return 0;
}
