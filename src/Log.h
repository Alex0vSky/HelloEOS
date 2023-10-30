// src\Log.h - with formatting warnings, thread safe if printf/wprintf is safe, 
// make_tuple to avoid multi evaluate __VA_ARGS__ values
// @insp SO/how-do-i-expand-a-tuple-into-variadic-template-functions-arguments
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#ifndef A0S_cppcheck__ // Cant pass '// cppcheck-suppress cstyleCast' to macro
#	define LOG(fmt, ...) \
		do {																							\
			using namespace std::chrono;																\
			using namespace std;																		\
			auto tupleParam = make_tuple( __VA_ARGS__ );												\
			auto tupleA = tuple_cat( make_tuple( (const char *)( fmt ), tupleParam ) );					\
			auto tupleW = tuple_cat( make_tuple( (const wchar_t *)( fmt ), tupleParam ) );				\
			constexpr bool bWchar = std::is_convertible_v< decltype( fmt ), const wchar_t * >;			\
			constexpr bool bChar = std::is_convertible_v< decltype( fmt ), const char * >;				\
			static_assert( bWchar || bChar, "Print only wchar_t or char strings");						\
			auto now = system_clock::now();																\
			auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;						\
			auto timer = system_clock::to_time_t(now);													\
			std::tm bt = *std::localtime(&timer);														\
			std::ostringstream ossA;																	\
			ossA << std::put_time(&bt, "%T");															\
			ossA << '.' << std::setfill('0') << std::setw(3) << ms.count() << " ";						\
			std::string dateTimeA = ossA.str( );														\
			std::wstring dateTimeW( dateTimeA.begin(), dateTimeA.end() );								\
			size_t len;																					\
			bChar																						\
				? len = std::apply( snprintf, tuple_cat( make_tuple( nullptr, 0 ), tupleA ) )			\
				: len = std::apply( _snwprintf, tuple_cat( make_tuple( nullptr, 0 ), tupleW ) )			\
					;																					\
			len++;																						\
			std::vector< char > bufA( len );															\
			std::vector< wchar_t > bufW( len );															\
			bChar																						\
				? (																						\
					std::apply( snprintf, tuple_cat( make_tuple( bufA.data( ), len ), tupleA ) )		\
					, dateTimeA += bufA.data( )															\
					, dateTimeA += "\n"																	\
					, printf( dateTimeA.c_str( ) )														\
					, (void)0																			\
					)																					\
				: (																						\
					std::apply( _snwprintf, tuple_cat( make_tuple( bufW.data( ), len ), tupleW ) )		\
					, dateTimeW += bufW.data( )															\
					, dateTimeW += L"\n"																\
					, wprintf( dateTimeW.c_str( ) )														\
					, (void)0																			\
					)																					\
				;																						\
		} while( false )
#endif // A0S_cppcheck__
