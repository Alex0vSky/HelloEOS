// src\Log.h - with formatting warnings
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#ifndef A0S_cppcheck__ // Cant pass '// cppcheck-suppress cstyleCast' to macro
#	define LOG(fmt, ...) \
		do {																							\
			constexpr bool bWchar = std::is_convertible_v< decltype( fmt ), const wchar_t * >;			\
			constexpr bool bChar = std::is_convertible_v< decltype( fmt ), const char * >;				\
			static_assert( bWchar || bChar, "Print only wchar_t or char strings");						\
			bChar																						\
				? printf( (const char *)( fmt ), __VA_ARGS__ )											\
				: wprintf( (const wchar_t *)( fmt ), __VA_ARGS__ )										\
					;																					\
			printf( "\n" );																				\
		} while( false )
#endif // A0S_cppcheck__
