#pragma once
#include "Common.h"
#include <direct.h>
#include "ConnectionState.h"
#include "TickStruct.h"


#define STRCPY_SAFE(dest, src) CopyString(dest, src)

std::string EnsureDirectory(const char* name);

uint64_t GetCurrentTimeUs();

void DebugPrint(const std::string& msg);

void DebugStateTransition(
    LineType line,
    ConnectionState current,
    ConnectionState newState);

const char* ToString(LineType line);
const char* ToString(ConnectionState state);

std::string GetTradingDay();

std::string CurrentDateTime();

std::string WStringToString(const std::wstring& wstr);


