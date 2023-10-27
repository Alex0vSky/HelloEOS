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

// EOS
#include "eos_auth.h"
#include "eos_sdk.h"
#include "eos_types.h"
#include "Windows/eos_Windows.h"
#include "eos_logging.h"
#include "eos_p2p.h"
#include "eos_friends.h"

#pragma comment( lib, "EOSSDK-Win64-Shipping" )
