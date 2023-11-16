// stdafx.h - pch

// System
#define NOMINMAX
#include <Windows.h>

// Stl
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <future>
//#include <coroutine>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <queue>
#include <optional>

// EOS
#include "eos_auth.h"
#include "eos_sdk.h"
#include "eos_types.h"
#include "Windows/eos_Windows.h"
#include "eos_logging.h"
#include "eos_p2p.h"
#include "eos_friends.h"
#include "eos_presence.h"

#if defined(_WIN32) && (defined(__i386) || defined(_M_IX86))
#pragma comment( lib, "EOSSDK-Win32-Shipping" )
#else
#pragma comment( lib, "EOSSDK-Win64-Shipping" )
#endif

#ifdef A0S_GRPC
#endif // A0S_GRPC
