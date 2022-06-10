#ifndef MODE_LOOPS_H
#define MODE_LOOPS_H

typedef void (*LoopFun)();
extern LoopFun currentLoopFun;

void lowVoltageLoop();
void goodVoltageLoop();
void smileyLoop();

#endif
