#ifndef GUARD_H_INCLUDED
#define GUARD_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#define REQUIRE_NOT_NULLED(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "\n[FATAL] Memory address of: '%s' is NULL at %s:%d when it shouldn't be!\n\n", \
                    #ptr, __FILE_NAME__, __LINE__); \
            abort(); \
        } \
    } while(0)

#define REQUIRE_NULLED(ptr) \
    do { \
        if ((ptr) != NULL) { \
            fprintf(stderr, "\n[FATAL] Memory address of: '%s' is NOT NULL at %s:%d when it should be!\n\n", \
                    #ptr, __FILE_NAME__, __LINE__); \
            abort(); \
        } \
    } while(0)

#endif // GUARD_H_INCLUDED
