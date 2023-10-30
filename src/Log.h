// src\Log.h - with formatting warnings, thread safe if printf/wprintf is safe, 
// using tuple to heavy, buffer avoid multi evaluate __VA_ARGS__ values
// @insp SO/how-do-i-expand-a-tuple-into-variadic-template-functions-arguments
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#ifndef A0S_cppcheck__ // Cant pass '// cppcheck-suppress cstyleCast' to macro
#	define LOG(fmt, ...) \
		do {																						\
			using namespace std::chrono;															\
			constexpr bool bWchar = std::is_convertible_v< decltype( fmt ), const wchar_t * >;		\
			constexpr bool bChar = std::is_convertible_v< decltype( fmt ), const char * >;			\
			static_assert( bWchar || bChar, "Print only wchar_t or char strings");					\
			auto now = system_clock::now( );														\
			auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;					\
			auto timer = system_clock::to_time_t(now);												\
			std::tm bt = *std::localtime(&timer);													\
			std::ostringstream ossA;																\
			ossA << std::put_time(&bt, "%T");														\
			ossA << '.' << std::setfill('0') << std::setw(3) << ms.count() << " | ";				\
			std::string dateTimeA = ossA.str( );													\
			std::wstring dateTimeW( dateTimeA.begin(), dateTimeA.end() );							\
			char bufA[ 1024 + 1 ] = { };															\
			wchar_t bufW[ 1024 + 1 ] = { };															\
			bChar																					\
				? (																					\
					snprintf( bufA, 1024, (const char *)( fmt ), __VA_ARGS__ )						\
					, dateTimeA += bufA																\
					, dateTimeA += "\n"																\
					, printf( dateTimeA.c_str( ) )													\
					, (void)0																		\
					)																				\
				: (																					\
					_snwprintf( bufW, 1024, (const wchar_t *)( fmt ), __VA_ARGS__ )					\
					, dateTimeW += bufW																\
					, dateTimeW += L"\n"															\
					, wprintf( dateTimeW.c_str( ) )													\
					, (void)0																		\
					)																				\
				;																					\
		} while( false )
#endif // A0S_cppcheck__
