#ifndef __MONITOR_H__
#define __MONITOR_H__

#include "machine.h"

extern void  initialize_readline();
extern char* rl();
extern int   monitor_execute(MACHINE* machine, char* command);

#endif
