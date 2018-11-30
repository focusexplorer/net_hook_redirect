#ifndef BASE_H
#define BASE_H
#define IS64BIT (sizeof(void*)==8)
#define IS32BIT (sizeof(void*)==4)

#ifdef ARCH64 
    #ifndef ULLPRE
    #define ULLPRE ll
    #endif
#endif

#endif