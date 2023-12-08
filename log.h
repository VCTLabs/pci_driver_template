/*
  log.h
    - simple logger
 */


#ifndef PCIE_385_A_SFP_LOG_H
#define PCIE_385_A_SFP_LOG_H

#include <linux/kernel.h>
#include "info.h"

#ifndef LOG_LEVEL_INFO
#define LOG_LEVEL_INFO "INFO"
#endif

#ifndef LOG_LEVEL_ERROR
#define LOG_LEVEL_ERROR "ERROR"
#endif

#ifndef LOG_LEVEL_WARNING
#define LOG_LEVEL_WARNING "WARNING"
#endif

/*
 */
static void __log_func(const char* lvl, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printk("%s [%s]: ", DRIVER_NAME, lvl);
	vprintk(fmt, args);
	va_end(args);
}

#ifndef LOG_INFO
#define LOG_INFO(fmt, args...) \
		__log_func(LOG_LEVEL_INFO, fmt, ## args)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(fmt, args...) \
		__log_func(LOG_LEVEL_ERROR, fmt, ## args)
#endif

#ifndef LOG_WARNING
#define LOG_WARNING(fmt, args...) \
		__log_func(LOG_LEVEL_WARNING, fmt, ## args)
#endif

#endif
