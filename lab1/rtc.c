#include "rtc.h"
#include <minix/syslib.h>
#include "minix/sysutil.h"

#define TODO return -1

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7)
#define RTC_DM_MSK (1 << 2)


static int bcd_to_bin(uint8_t bcd) {
  //BCD means Binary Coded Decimal. 0010 0100 is read in blocks of 4. 
  //First block is 2, second block is 4, so number is 2*10 + 4 = 24.
  // >> n shifts bits n times to the right
  return (((bcd >> 4) * 10)+(bcd & 0x0F));
}

int rtc_read_reg(uint8_t reg, uint8_t *value) {
  //Writes to the sys_outb the register which we want to read, then read with sys_inb the value of it. 
    if (sys_outb(RTC_ADDR_REG, reg) != 0) return 1;
    uint32_t temp;
    if (sys_inb(RTC_DATA_REG, &temp) != 0) return 1;
    *value = (uint8_t) temp; 
    return 0;
}

int rtc_read_date(rtc_date *date) {
  //While RTC updating stay in loop
  uint8_t statusA, statusB, day, month, year;
  if (rtc_read_reg(RTC_REG_A, &statusA)) return 1;
  while (statusA & RTC_UIP_MSK){
    tickdelay(micros_to_ticks(10));
    if (rtc_read_reg(RTC_REG_A, &statusA)) return 1;
  }

  //Read Values of registers
  rtc_read_reg(RTC_REG_DAY, &day);
  rtc_read_reg(RTC_REG_MONTH, &month);
  rtc_read_reg(RTC_REG_YEAR, &year);
  rtc_read_reg(RTC_REG_B, &statusB);

  //Convert if in BCD
  if (!(statusB & RTC_DM_MSK)) {
        date->day = (uint8_t) bcd_to_bin(day);
        date->month = (uint8_t) bcd_to_bin(month);
        date->year = (uint8_t) bcd_to_bin(year);
    } else {
        date->day = day;
        date->month = month;
        date->year = year;
    }

    return 0;

}
