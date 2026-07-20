#pragma once
#include "Common.h"
#include <direct.h>

std::string CreateFlowDir(const char* name);

uint64_t GetCurrentTimeUs();

void DebugPrint(const std::string& msg);

// 将 string 安全拷贝到 char 数组（自动截断）
template<size_t N>
bool CopyString(char(&dest)[N], const std::string& src)
{
    if (src.empty()) {
        dest[0] = '\0';
        return true;
    }

    size_t copyLen = std::min(src.length(), N - 1);
    src.copy(dest, copyLen, 0);
    dest[copyLen] = '\0';
    return copyLen == src.length();  // true 表示完整拷贝，false 表示被截断
}

// 重载：支持 const char* 源
template<size_t N>
bool CopyString(char(&dest)[N], const char* src)
{
    if (src == nullptr) {
        dest[0] = '\0';
        return false;
    }
    return CopyString(dest, std::string(src));
}