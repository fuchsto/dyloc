#ifndef DYLOC__COMMON__CONFIG_H__INCLUDED
#define DYLOC__COMMON__CONFIG_H__INCLUDED

// Platform defines

// OSX
#if defined(__MACH__) && defined(__APPLE__)
#  define DYLOC__PLATFORM__OSX
#endif
// UX
#if (defined(__hpux) || defined(hpux)) || \
     ((defined(__sun__) || defined(__sun) || defined(sun)) && \
      (defined(__SVR4) || defined(__svr4__)))
#  define DYLOC__PLATFORM__UX
#endif
// Linux
#if defined(__linux__)
#  define DYLOC__PLATFORM__LINUX
#  define DYLOC__PLATFORM__POSIX
#endif
// FreeBSD
#if defined(__FreeBSD__)
#  define DYLOC__PLATFORM__FREEBSD
#  define DYLOC__PLATFORM__POSIX
#endif

#endif /* DYLOC__COMMON__CONFIG_H__INCLUDED */
