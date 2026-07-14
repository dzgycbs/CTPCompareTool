#include "Utils.h"

std::string CreateFlowDir(const char* name)
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
        OutputDebugStringA("Create Flow Folder\n");
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