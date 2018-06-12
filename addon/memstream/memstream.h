#if defined(__linux__)
# include <features.h>
#endif

#include <stdio.h>

// #ifdef CUSTOM_MEMSTREAM

FILE *open_memstream(char **ptr, size_t *sizeloc);

// #endif /* CUSTOM_MEMSTREAM */