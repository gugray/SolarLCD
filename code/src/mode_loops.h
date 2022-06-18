#ifndef MODE_LOOPS_H
#define MODE_LOOPS_H

typedef void (*LoopFun)();
extern LoopFun currentLoopFun;

void startupLoop();
void lowVoltageLoop();
void midVoltageLoop();
void highVoltageLoop();

#endif
