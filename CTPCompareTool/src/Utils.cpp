#include "Utils.h"

std::string EnsureDirectory(const char* name)
{
    std::string path = "";
    char cwd[512];
    // 检查返回值是否为 NULL
    if (_getcwd(cwd, sizeof(cwd)) != NULL)
    {
        path = std::string(cwd) + "\\" + name + "\\";
    }
    else 
    {
        // 处理错误情况，例如打印错误信息
        perror("_getcwd failed");
    }

    if (path.length() == 0)
    {
        return path;
    }  

    if (_mkdir(path.c_str()) == 0)
    {
        OutputDebugStringA("Create Folder\n");
    }

    return path;
}

uint64_t GetCurrentTimeUs()
{
    using namespace std::chrono;

    return duration_cast<microseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}

void DebugPrint(const std::string& msg) {
    OutputDebugStringA(msg.c_str());
}

void DebugStateTransition(LineType line, ConnectionState current, ConnectionState newState)
{
    DebugPrint(
        std::string("[") +
        ToString(line) +
        std::string("] ")
        + ToString(current)
        + std::string(" -> ")
        + ToString(newState)
        + "\n");
}

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


const char* ToString(LineType line)
{
    if (line == LineType::Left)
    {
        return "LEFT";
    }
    else
    {
        return "RIGHT";
    }
}

const char* ToString(ConnectionState state)
{
    switch (state)
    {
    case ConnectionState::Idle:
        return "Idle";

    case ConnectionState::Connecting:
        return "Connecting";

    case ConnectionState::FrontConnected:
        return "FrontConnected";

    case ConnectionState::Logining:
        return "Logining";

    case ConnectionState::LoginSuccess:
        return "LoginSuccess";

    case ConnectionState::Subscribing:
        return "Subscribing";

    case ConnectionState::Running:
        return "Running";

    case ConnectionState::Disconnected:
        return "Disconnected";

    case ConnectionState::Stopped:
        return "Stopped";

    default:
        return "Unknown";
    }
}

std::string GetTradingDay()
{
    time_t t = time(nullptr);

    tm tmNow;

    localtime_s(
        &tmNow,
        &t);

    char buf[32];

    sprintf_s(
        buf,
        "%04d%02d%02d",
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday);

    return buf;
}
