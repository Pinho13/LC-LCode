#include <lcom/lcf.h>

#include "fw/drivers/rtc.h"

#define ASS(x, msg)                                                         \
  if (!(x)) {                                                                  \
    printf("%s FAILED!\n", msg);                                               \
    exit(1);                                                                   \
  } else {                                                                     \
    printf("%s passed.\n", msg);                                               \
  }


int rtc_example() {
  rtc_date date = {0, 0, 0};
  if (rtc_read_date(&date) != OK) return 1;

  printf("\nday: %u \n", date.day);
  printf("month: %u \n", date.month);
  printf("year: %u \n", date.year);

  return 0;
}

void test_rtc_date() {
  rtc_date date = {0, 0, 0};
  rtc_read_date(&date);

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  struct tm *tm_info = localtime(&ts.tv_sec);

  ASS(abs((int)date.day - (int)tm_info->tm_mday) < 2, "rtc read day");
  ASS(abs((int)date.month - (int)tm_info->tm_mon) < 2, "rtc read month");
  ASS(date.year == tm_info->tm_year % 100, "rtc read year");
}

int(proj_main_loop)(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  rtc_example();
  test_rtc_date();

  return 0;
}

int(main)(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/framework/trace.txt");
  lcf_log_output("/home/lcom/labs/framework/output.txt");
  
  if (lcf_start(argc, argv))
    return 1;

  lcf_cleanup();
  return 0;
}
