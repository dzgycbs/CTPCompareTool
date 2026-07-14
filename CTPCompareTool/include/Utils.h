#pragma once
#include "Common.h"
#include <direct.h>

std::string CreateFlowDir(const char* name);

uint64_t GetCurrentTimeUs();

void DebugPrint(const std::string& msg);