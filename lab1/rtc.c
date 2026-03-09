#include "rtc.h"
#include <minix/syslib.h>
#include <lcom/lcf.h> 


#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A //registro com UIP 
#define RTC_REG_B 0x0B //registro com DM: BCD(0) binaray(1)
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7) //REG_A
#define RTC_DM_MSK (1 << 2) 

static uint8_t bcd_to_bin(uint8_t bcd) { 
  uint8_t upper = (bcd >> 4) & 0x0F;//dezenas
  uint8_t lower = bcd & 0x0F;//unidades
  return upper * 10 + lower;//converte para decimal
}
int busca(uint8_t reg, uint8_t* res){
  int ret;
  uint32_t aux;

  ret = sys_outb(RTC_ADDR_REG, reg);
  if (ret != OK) return -1; 
  ret = sys_inb(RTC_DATA_REG, &aux);
  if (ret != OK) return -1;
  
  *res = (uint8_t)aux;  // passa o valor lido para res
  return 0; 
}

int rtc_read_date(rtc_date *date) { 
  uint8_t bcd_bin;//0 se bcd, 1 se binary
  uint8_t regA,regB, day, month, year;
  if(busca(RTC_REG_B,&regB)==-1) return -1;
  bcd_bin = regB & RTC_DM_MSK ? 1 : 0;
  if(busca(RTC_REG_A,&regA)==-1) return -1;
  if (regA & RTC_UIP_MSK){
    tickdelay(micros_to_ticks(244));
  }
  if(busca(RTC_REG_DAY,&day)==-1) return -1;
  if(busca(RTC_REG_MONTH,&month)==-1) return -1;
  if(busca(RTC_REG_YEAR,&year)==-1) return -1;
  if (!bcd_bin){
    day = bcd_to_bin(day);
    month = bcd_to_bin(month);
    year = bcd_to_bin(year);
  }
  date->day=day;
  date->month=month;
  date->year=year;
  return 0;
}
