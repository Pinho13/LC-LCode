#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <string.h>

#include "fw/drivers/rtc.h"
#include "fw/drivers/timer.h"
#include "fw/common/utils.h"

#ifdef ASSERT
  #undef ASSERT
#endif

#define ASSERT(x, msg)                                                         \
  if (!(x)) {                                                                  \
    printf("%s FAILED!\n", msg);                                               \
    exit(1);                                                                   \
  } else {                                                                     \
    printf("%s passed.\n", msg);                                               \
  }

// Errors
void error_example();

// RTC
int rtc_example();
void test_rtc_date();

// Timer
int timer_example();

int(proj_main_loop)(int argc, char *argv[]) {
  if (argc != 1) {
    printf("Usage:\n");
    printf("./proj <driver>\n");
    printf("<driver>: error | rtc | timer | keyboard | mouse | video\n");
    return 1;
  } else {
    if (strcmp(argv[0], "error") == 0) {
      error_example();

    } else if (strcmp(argv[0], "rtc") == 0) {
      rtc_example();
      test_rtc_date();

    } else if (strcmp(argv[0], "timer") == 0) {
      timer_example();

    } else if (strcmp(argv[0], "keyboard") == 0) {

    } else if (strcmp(argv[0], "mouse") == 0) {

    } else if (strcmp(argv[0], "video") == 0) {

    } else {
      return fail(ERR, "argv[1]: invalid argument");
    }
  }

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

void error_example() {
  fail(WARN, "This is a Warning");
  fail(ERR, "This is an Error");
  fail(ERR_RTC, "This is a RTC Error");
  fail(ERR_TIMER, "This is a Timer Error");
  fail(ERR_KBC, "This is a KBC Error");
  fail(ERR_KEYBOARD, "This is a Keybooard Error");
  fail(ERR_MOUSE, "This is a Mouse Error");
  fail(ERR_VIDEO, "This is a Video Error");
}

int rtc_example() {
  rtc_date date = {0, 0, 0};
  if (rtc_read_date(&date) != OK) return 1;

  printf("\n\n");
  printf("RTC EXAMPLE \n");
  printf("day: %u \n", date.day);
  printf("month: %u \n", date.month);
  printf("year: %u", date.year);

  return 0;
}

void test_rtc_date() {
  rtc_date date = {0, 0, 0};
  rtc_read_date(&date);

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  struct tm *tm_info = localtime(&ts.tv_sec);

  printf("\n\n");
  printf("RTC TEST \n");

  ASSERT(abs((int)date.day - (int)tm_info->tm_mday) < 2, "rtc read day");
  ASSERT(abs((int)date.month - (int)tm_info->tm_mon) < 2, "rtc read month");
  ASSERT(date.year == tm_info->tm_year % 100, "rtc read year");
}

int timer_example() {
  printf("\n\n");
  printf("Timer Example \n");

  uint8_t bit_no;

  int ipc_status;
  message msg;

  uint32_t seconds_to_quit = 5;

  if (timer_subscribe_int(&bit_no) != 0)
    return 1;

  int irq_set = BIT(bit_no);

  int r;
  while (get_int_counter() < seconds_to_quit*60) {
    
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            
            timer_int_handler();

            if(get_int_counter() % 60 == 0)
              printf(" -%us\n", get_int_counter()/60);
            }
          break;
      }
    }

    
  }

  timer_unsubscribe_int();

  return 0;
}
