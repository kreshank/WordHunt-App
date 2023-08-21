#pragma once

// Helper Macros
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

// Math Macros
#define IM_PI                           3.14159265358979323846f
#define IM_FLOOR(_VAL)                  ((float)(int)(_VAL))  
#define IM_ROUND(_VAL)                  ((float)(int)((_VAL) + 0.5f)) 
