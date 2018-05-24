#ifndef STATUS_HH
#define STATUS_HH
#include "../tool/type.h"
void  Status_Init();
void  Status_SetStackPointer(bool isA,u16 pointer);
u16   Status_GetStackPointer(bool isA);
#endif
