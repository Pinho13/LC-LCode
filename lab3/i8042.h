#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/* I/O port addresses */

#define KBC_STATUS_REG   0x64 
#define KBC_DATA_REG    0x60 

/* KBC control */

/* KBC selection: bits 7 and 6 */

#define KBC_PAR_ERR   BIT(7)            
#define KBC_TOUT_ERR  BIT(6)            
#define KBC_OUT_BUFF  BIT(0) 
#define KBC_ST_IBF    BIT(1)

#define RD_CMD 0x20
#define WR_CMD 0x60

#define BREAKCODE BIT(7)


/**@}*/

#endif /* _LCOM_I8042_H_ */
