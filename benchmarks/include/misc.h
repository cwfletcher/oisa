#ifndef __MISC_HEADER__
#define __MISC_HEADER__

// calculate the log_2 value _after_ round val up to the closest power_of_2 value
// Example: log_2(1023) = 10, log_2(1024) = 10, log_2(1025) = 11
inline int log_2(int val);

// calculate the log_2 value _after_ round val down to the closest power_of_2 value
// Example: log_2(1023) = 9, log_2(1024) = 10, log_2(1025) = 10
inline int log_2_tight(int val);

inline int bitwiseReverse(int val, int L);

inline int roundToPowerOf2(int val);

#endif
