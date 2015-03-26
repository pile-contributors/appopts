/**
 * @file appopts-private.h
 * @brief Declarations for AppOpts class
 * @author Nicu Tofan <nicu.tofan@gmail.com>
 * @copyright Copyright 2014 piles contributors. All rights reserved.
 * This file is released under the
 * [MIT License](http://opensource.org/licenses/mit-license.html)
 */

#ifndef GUARD_APPOPTS_PRIVATE_H_INCLUDE
#define GUARD_APPOPTS_PRIVATE_H_INCLUDE

#include <appopts/appopts-config.h>

#if 0
#    define APPOPTS_DEBUGM printf
#else
#    define APPOPTS_DEBUGM black_hole
#endif

#if 0
#    define APPOPTS_TRACE_ENTRY printf("APPOPTS ENTRY %s in %s[%d]\n", __func__, __FILE__, __LINE__)
#else
#    define APPOPTS_TRACE_ENTRY
#endif

#if 0
#    define APPOPTS_TRACE_EXIT printf("APPOPTS EXIT %s in %s[%d]\n", __func__, __FILE__, __LINE__)
#else
#    define APPOPTS_TRACE_EXIT
#endif


static inline void black_hole (...)
{}

#endif // GUARD_APPOPTS_PRIVATE_H_INCLUDE
