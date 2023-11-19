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
#pragma warning( push )
#pragma warning( disable: 4244 )
// gRPC main
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

// helloworld.grpc.pb.cc
#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>

// helloworld.pb.cc
#include <algorithm>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"

#include "gRpc/helloworld.grpc.pb.h"

// Reflection
#include "test/cpp/util/proto_reflection_descriptor_database.h"
#include "google/protobuf/dynamic_message.h"

#pragma warning( pop )
#endif // A0S_GRPC

#include "ThirdParty/Hexdump.hpp"
