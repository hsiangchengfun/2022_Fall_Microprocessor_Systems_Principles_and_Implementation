#ifndef __STDDEF_H___
#define __STDDEF_H___

#if !defined(_SIZE_T_)
#define _SIZE_T
typedef unsigned long size_t;
#endif

#if !defined(_WCHAR_T_)
#define _WCHAR_T
typedef int wchar_t;
typedef unsigned int wint_t;
#endif

#if !defined(NULL)
#define NULL 0
#endif

#endif

