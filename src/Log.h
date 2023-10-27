// src\Log.h - with formatting warnings, thread safe if printf/wprintf is safe
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#ifndef A0S_cppcheck__ // Cant pass '// cppcheck-suppress cstyleCast' to macro
#	define LOG(fmt, ...) \
		do {																							\
			using namespace std::chrono;																\
			constexpr bool bWchar = std::is_convertible_v< decltype( fmt ), const wchar_t * >;			\
			constexpr bool bChar = std::is_convertible_v< decltype( fmt ), const char * >;				\
			static_assert( bWchar || bChar, "Print only wchar_t or char strings");						\
			auto now_in_sys = floor< milliseconds >( system_clock::now( ) );							\
			auto now_in_local = current_zone( ) ->to_local( now_in_sys );								\
			std::string dateTimeA = std::format( "{0:%T} ", now_in_local );								\
			std::wstring dateTimeW = std::format( L"{0:%T} ", now_in_local );							\
			size_t len;																					\
			bChar																						\
				? len = snprintf( nullptr, 0, (const char *)( fmt ), __VA_ARGS__ )						\
				: len = _snwprintf( nullptr, 0, (const wchar_t *)( fmt ), __VA_ARGS__ )					\
					;																					\
			len++;																						\
			std::vector< char > bufA( len );															\
			std::vector< wchar_t > bufW( len );															\
			bChar																						\
				? (																						\
					snprintf( bufA.data( ), len, (const char *)( fmt ), __VA_ARGS__ )					\
					, dateTimeA += bufA.data( )															\
					, dateTimeA += "\n"																	\
					, printf( dateTimeA.c_str( ) )														\
					, (void)0																			\
					)																					\
				: (																						\
					_snwprintf( bufW.data( ), len, (const wchar_t *)( fmt ), __VA_ARGS__ )				\
					, dateTimeW += bufW.data( )															\
					, dateTimeW += L"\n"																\
					, wprintf( dateTimeW.c_str( ) )														\
					, (void)0																			\
					)																					\
				;																						\
		} while( false )
#endif // A0S_cppcheck__
