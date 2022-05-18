#define RL_SDL
#define GAUSSLIMIT 0.001
#define GAUSSRAD_HQ 2.8
#define LRGB_NODITHER
#define RL_INCL_UNICODE_DATA_MINI
#define RL_INCL_VECTOR_TYPE_FILEBALL

#define fprintf_rl fprintf_wrapper

#include "rouziclib/rouziclib.h"

extern void fprintf_wrapper(FILE *stream, const char* format, ...);
extern buffer_t print_log;
