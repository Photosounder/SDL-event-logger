#include "rl.h"

buffer_t print_log={0};

void fprintf_wrapper(FILE *stream, const char* format, ...)
{
	va_list args;

	va_start(args, format);

	vbufprintf(&print_log, format, args);	// printf to buffer
	buf_tail(&print_log, 300);		// keep buffer to only the last 300 lines
	vfprintf(stream, format, args);   	// printf to original stream
	fflush(stream);

	va_end(args);
}

#include "rouziclib/rouziclib.c"
