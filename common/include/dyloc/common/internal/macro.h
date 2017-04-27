#ifndef DYLOC__COMMON__INTERNAL__MACRO_H__INCLUDED
#define DYLOC__COMMON__INTERNAL__MACRO_H__INCLUDED

/**
 * Macro parameter value string expansion.
 */
#define dyloc__tostr(s) #s
/**
 * Macro parameter string expansion.
 */
#define dyloc__toxstr(s) dyloc__tostr(s)
/**
 * Mark variable as intentionally or potentially unused to avoid compiler
 * warning from unused variable.
 */
#define dyloc__unused(x) (void)(x)

#endif // DYLOC__COMMON__INTERNAL__MACRO_H__INCLUDED
