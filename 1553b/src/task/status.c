#include "./status.h"

static u16  g_stackptra_prev;//unhandled messages are between BU_RegGetVal(BU_CMDSTACKPOINT) ~g_stackptr_prev
static u16  g_stackptrb_prev;//unhandled messages are between BU_RegGetVal(BU_CMDSTACKPOINT) ~g_stackptr_prev
void  Status_Init(){
    g_stackptra_prev = 0;
	g_stackptrb_prev = 0;
}

void  Status_SetStackPointer(bool isA,u16 pointer){
    isA ? (g_stackptra_prev = pointer) : (g_stackptrb_prev = pointer);
}
u16   Status_GetStackPointer(bool isA){
    return isA ? g_stackptra_prev : g_stackptrb_prev;
}
