#pragma once

//======================================================
// 实验配置（控制整个模拟系统行为）
//======================================================

struct ExperimentConfig
{
    // 🔥 随机种子（关键：保证可重复）
    unsigned int seed = 12345;

    // 左右线路基础延迟（微秒）
    int leftDelayUs = 2000;
    int rightDelayUs = 1000;

    // 抖动范围（随机延迟）
    int jitterUs = 5;

    // 价格噪声
    double priceNoise = 0.01;

    // 是否启用确定性模式
    bool deterministic = true;
};