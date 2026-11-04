#ifndef GUARD_H_INCLUDED
#define GUARD_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

/* Fallback for compilers in case they don't support __FILE_NAME__ (like MSVC or older GCC) */
#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define REQUIRE_NOT_NULLED(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "\n[FATAL] Memory address of: '%s' is NULL at %s:%d when it shouldn't be!\n\n", \
                    #ptr, __FILE_NAME__, __LINE__); \
            abort(); \
        } \
    } while(0)

#define REQUIRE_VALID_MODULE(mod) do { \
    REQUIRE_NOT_NULLED(mod); \
    REQUIRE_NOT_NULLED((mod)->init); \
    REQUIRE_NOT_NULLED((mod)->upd); \
    REQUIRE_NOT_NULLED((mod)->render); \
    REQUIRE_NOT_NULLED((mod)->clean_up); \
} while(0)

#endif // GUARD_H_INCLUDED
