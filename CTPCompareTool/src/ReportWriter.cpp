#include "ReportWriter.h"

#include <fstream>

#include "json.hpp"


using json = nlohmann::json;


bool ReportWriter::SaveJson(
    const StatisticsReport& report,
    const std::string& filePath)
{
    json j;

    j["version"] =
        report.version;

    j["startTime"] =
        report.startTime;

    j["endTime"] =
        report.endTime;


    //------------------------------------------------
    // 基础信息
    //------------------------------------------------

    j["instrument"] =
        report.instrument;


    j["tradingDay"] =
        report.tradingDay;


    j["runtimeSeconds"] =
        report.runtimeSeconds;



    //------------------------------------------------
    // Snapshot
    //------------------------------------------------

    const auto& s =
        report.snapshot;


    json snapshot;


    snapshot["matchedCount"] =
        s.matchedCount;


    snapshot["leftWinCount"] =
        s.leftWinCount;


    snapshot["rightWinCount"] =
        s.rightWinCount;


    snapshot["drawCount"] =
        s.drawCount;



    snapshot["leftWinRate"] =
        s.leftWinRate;


    snapshot["rightWinRate"] =
        s.rightWinRate;


    snapshot["drawRate"] =
        s.drawRate;



    snapshot["minLatencyUs"] =
        s.minLatencyUs;


    snapshot["maxLatencyUs"] =
        s.maxLatencyUs;


    snapshot["avgLatencyUs"] =
        s.avgLatencyUs;


    snapshot["p50LatencyUs"] =
        s.p50LatencyUs;


    snapshot["p95LatencyUs"] =
        s.p95LatencyUs;


    snapshot["p99LatencyUs"] =
        s.p99LatencyUs;



    snapshot["avgAdvantageUs"] =
        s.avgAdvantageUs;


    snapshot["maxAdvantageUs"] =
        s.maxAdvantageUs;


    snapshot["leftStreak"] =
        s.leftStreak;


    snapshot["rightStreak"] =
        s.rightStreak;


    j["snapshot"] =
        snapshot;



    //------------------------------------------------
    // 写文件
    //------------------------------------------------

    std::ofstream ofs(filePath);


    if (!ofs.is_open())
    {
        return false;
    }


    ofs
        << j.dump(4);


    return true;
}