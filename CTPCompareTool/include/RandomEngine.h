#pragma once
#include <random>

//======================================================
// 可重复随机数引擎
//======================================================

class RandomEngine
{
public:
    RandomEngine(unsigned int seed)
        : eng(seed) {
    }

    int RandInt(int minV, int maxV)
    {
        std::uniform_int_distribution<int> dist(minV, maxV);
        return dist(eng);
    }

    double RandDouble(double minV, double maxV)
    {
        std::uniform_real_distribution<double> dist(minV, maxV);
        return dist(eng);
    }

private:
    std::mt19937 eng;
};