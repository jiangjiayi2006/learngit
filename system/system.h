#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "common.h"

typedef enum {
    MODE_SPEED_CONTROL = 0,
    MODE_POSITION_FOLLOW
} SystemMode_t;

void System_Init(void);
void System_Update(void);
void System_ChangeMode(SystemMode_t new_mode);
SystemMode_t System_GetCurrentMode(void);
uint32_t System_GetTick(void);
void System_10ms_Task(void);  // ÃÌº”’‚––

extern volatile uint32_t system_tick;

#endif /* __SYSTEM_H */