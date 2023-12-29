#ifndef GLOBALS_H

    #ifdef INIT_GLOBALS_C
        #define GLOBALS_EXT 
        #define GLOBALS_EQU(v) =v
    #else
        #define GLOBALS_EXT extern
        #define GLOBALS_EQU(v)
    #endif

    char tmp_str[1024];

#endif