#pragma once

//=============================================================================
// Windows
//=============================================================================

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "Comctl32.lib")

//=============================================================================
// C Runtime
//=============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>

//=============================================================================
// STL
//=============================================================================

#include <string>
#include <string_view>

#include <vector>
#include <array>

#include <map>
#include <unordered_map>

#include <set>

#include <deque>

#include <memory>

#include <mutex>
#include <shared_mutex>

#include <thread>

#include <chrono>

#include <algorithm>

#include <functional>

#include <atomic>

#include <fstream>

#include <sstream>

#include <iomanip>

//=============================================================================
// Common Types
//=============================================================================

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

using namespace std::chrono_literals;

//=============================================================================
// Project Headers
//=============================================================================

#include "Version.h"


