#pragma once

#include <cellstatus.h>
#include <sys/prx.h>
#include <exception>
#include <ppu_intrinsics.h>
#include <netinet/in.h>
#include <sys/cdefs.h>
#include <sys/process.h>
#include <sys/socket.h>
#include <sys/timer.h>
#include <sysutil/sysutil_oskdialog.h>
#include <sysutil/sysutil_msgdialog.h>

#define nullptr 0
#define Sleep(time) sys_timer_usleep(time * 1000)



extern "C" {
	int	_sys_printf(const char* format, ...);
	int	_sys_sprintf(char* s, const char* format, ...);
	int	_sys_vsprintf(char * s, const char * format, va_list arg);

	void *_sys_malloc(size_t n);
	void _sys_free(void* ptr);
	void *_sys_memcpy(void * destination, const void * source, size_t num);
	int _sys_memcmp(const void * ptr1, const void * ptr2, size_t num);
	void *_sys_memset(void * ptr, int value, size_t num);

	char *_sys_strcat(char * destination, const char * source);
	int _sys_strcmp(const char * str1, const char * str2);
	char *_sys_strcpy(char * destination, const char * source);
	size_t _sys_strlen(const char * str);
	char *_sys_strncat(char * destination, const char * source, size_t num);
	int _sys_strncmp(const char * str1, const char * str2, size_t num);
	char *_sys_strncpy(char * destination, const char * source, size_t num);
}

#define printf			_sys_printf
#define sprintf			_sys_sprintf
#define vsprintf		_sys_vsprintf

#define memcpy			_sys_memcpy
#define memcmp			_sys_memcmp
#define memset			_sys_memset

#define strcat			_sys_strcat
#define strcmp			_sys_strcmp
#define strcpy			_sys_strcpy
#define strlen			_sys_strlen
#define strncat			_sys_strncat
#define strncmp			_sys_strncmp
#define strncpy			_sys_strncpy

inline void* operator new(size_t size) { return _sys_malloc(size); }
inline void* operator new[](size_t size) { return _sys_malloc(size); }
inline void* operator new(unsigned int size, unsigned int) { return _sys_malloc(size); }
inline void operator delete(void* ptr) { _sys_free(ptr); }
inline void operator delete[](void* ptr) { _sys_free(ptr); }

inline std::exception::~exception() {}
inline void std::exception::_Raise() const {}
inline void std::exception::_Doraise() const {}
inline void std::_Throw(std::exception const&) {}
inline const char* std::exception::what() const { return (const char*)0;  }

struct opd_s
{
	uint32_t sub;
	uint32_t toc;
};

#undef vector
#include <vector>
#include <string>
#include <sys/ppu_thread.h>
#include <np.h>

#include "ROP.h"
#include "Utils.h"
#include "Detour.h"
#include "Hooks.h"
#include "Cheats.h"
#include "HudElems.h"
#include "dwStuffs.h"

#define TOC 0x00D67E98
