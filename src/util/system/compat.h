#pragma once

#include "defaults.h"

#include <cctype>
#include <cstring>
#include <cstdarg>

#include <signal.h>

#if defined(_unix_)
    #include <unistd.h>
#endif

#if defined (_win_)
    #include <process.h>
#endif

#if !defined(__unix__)
    #define __CONCAT1(x,y) x ## y
    #define __CONCAT(x,y)  __CONCAT1(x,y)
#endif
#if !defined (__FreeBSD__)
    #define __STRING(x)    #x             /* stringify without expanding x */
    #define __XSTRING(x)   __STRING(x)    /* expand x, then stringify */
#endif

extern "C" {
#if defined(_win_)
    typedef int pid_t;

    inline unsigned int alarm(unsigned int /*seconds*/) {
        return 0; // no alarm is currently set :)
    }

    #define SIGQUIT SIGBREAK // instead of 3
    #define SIGKILL SIGTERM  // instead of 9
    #define SIGPIPE 13       //will not receive under win?
    #define SIGALRM 14       //will not receive under win?
#endif

#if !defined (__FreeBSD__) || (__FreeBSD__ < 4)
    size_t strlcpy(char* dst, const char* src, size_t len);

    inline size_t strlcat(char* dst, const char* src, size_t len) {
        size_t dstlen = strlen(dst);
        size_t srclen = strlen(src);

        if (dstlen < len) {
            len -= dstlen;
            dst += dstlen;
            strlcpy(dst, src, len);
        }

        return dstlen + srclen;
    }
#endif

#if (defined(__FreeBSD__) && __FreeBSD__ >= 5) || defined(_darwin_)
    #define HAVE_NATIVE_GETPROGNAME
#endif

#ifndef HAVE_NATIVE_GETPROGNAME
    const char* getprogname();
#endif

#if defined (_MSC_VER) || defined(_sun_)
    void err(int e, const char *m, ...);
    void errx(int e, const char *m, ...);
    void warn(const char *m, ...);
    void warnx(const char *m, ...);
    void vwarnx (const char *format, va_list ap);
    void vwarn (const char *format, va_list ap);
    void verrx (int status, const char *format, va_list ap);
#   ifndef _sun_
        int snprintf(char *str, size_t size, const char *format, ...);
#if _MSC_VER < 1400
        int vsnprintf(char *str, size_t size, const char *format, va_list argptr);
#endif
        int asprintf(char **ret, const char *fmt, ...);
        int vasprintf(char **ret, const char *fmt, va_list params);
#   endif
#else
#   include <err.h>
#endif

#if !defined (_MSC_VER)
    char* strlwr(char*);
    char* strupr(char*);
    char* strrev(char*);
#if !defined(__CYGWIN__)
    inline int stricmp(const char*s1, const char *s2)
    {
        return strcasecmp(s1, s2);
    }
    inline int strnicmp(const char*s1, const char *s2, size_t len)
    {
        return strncasecmp(s1, s2, len);
    }
#endif
#endif

#if !defined(__linux__) && (!defined(__FreeBSD__) || __FreeBSD__ <= 4)
    char *stpcpy(char *dst, const char *src);
#endif

#if defined (_MSC_VER) || defined(_sun_)
    char *strsep(char **stringp, const char *delim);
#endif

}

#if _MSC_VER < 1400
// following two functions are not compatibility issue:
// they are just our extensions
inline size_t strnlen(const char* str, size_t buflen)
{
    const char *nulptr = (const char*)memchr(str, '\x00', buflen);
    if (nulptr == NULL)
        return buflen;
    return nulptr - str;
}
#endif

inline char *strncopy(char *dst, const char *src, size_t len)
{
    strlcpy(dst, src, len);
    return dst;
}

#ifdef _MSC_VER
#   define popen _popen
#   define pclose _pclose
#   if _MSC_VER < 1800
#       define strtoll _strtoi64
#       define strtoull _strtoui64
#       define atoll(x) _strtoi64(x, NULL, 10)
        inline int isblank(int c) {
            return isprint(c) && isalnum(c) == 0 && ispunct(c) == 0;
        }
#   endif
#endif

#ifdef _win_
#   define NAME_MAX FILENAME_MAX
#endif
#ifdef _sun_
#   define NAME_MAX PATH_MAX
#endif

#ifdef _win_
#   ifdef sleep // may be defined by perl
#       undef sleep
#   endif
    inline void sleep(i64 len) {Sleep((unsigned long)len * 1000);}
    inline void usleep(i64 len) {Sleep((unsigned long)len / 1000);}
#endif

#ifdef _win_
struct iovec {  // not defined in win32 :-/
    char   *iov_base;
    size_t iov_len;
};
#endif

#if defined(_win_) || defined(_cygwin_)
template<class T>
T* strcasestr(T* str1, T* str2)
{
    T* cp = str1;
    T* s1;
    T* s2;

    if (!*str2)
        return str1;

    while (*cp)
    {
        s1 = cp;
        s2 = str2;

        while (*s1 && *s2 && (tolower(*s1) == tolower(*s2)))
        {
            ++s1;
            ++s2;
        }

        if (!*s2)
            return cp;

        ++cp;
    }

    return NULL;
}
#endif

#if defined(_win_) || defined(_cygwin_) || defined(_darwin_) || (defined(__FreeBSD__) && __FreeBSD_version < 702000)
/* Copyright (c) 2007 Todd C. Miller <Todd.Miller@courtesan.com> */
inline void *memrchr(const void *s, int c, size_t n) {
    if (n != 0) {
        const unsigned char *cp = (unsigned char *)s + n;
        do {
            if (*(--cp) == (unsigned char)c)
                return ((void*)cp);
        } while (--n != 0);
    }
    return 0;
}

#endif

#ifdef _win_
int ftruncate(int fd, i64 length);
int truncate(const char* name, i64 length);
#endif

#if defined(GNUC)
    #ifndef va_copy
        #define va_copy __va_copy
    #endif
#endif

#if defined(_MSC_VER) && _MSC_VER < 1800
    #define va_copy(a, b) a = b
#endif

#if defined(__GNUC__)
#    define FUNC_SIGNATURE __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#    define FUNC_SIGNATURE __FUNCSIG__
#else
#    define FUNC_SIGNATURE ""
#endif
