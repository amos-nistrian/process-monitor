#include <gtk/gtk.h>

int proc();

extern char *g_processName; // Global stores process name
extern char *g_userName; // Global stores user
extern gint g_processID; // Global store the pid
extern gfloat g_memory; // Global store the memory
extern gint g_procTicks; // Global store prock tick
extern int g_cpuTick; // Global store cpu tick
