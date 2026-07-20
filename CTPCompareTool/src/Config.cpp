#include "Config.h"

#include "json.hpp"

#include <fstream>


using json = nlohmann::json;


bool Config::Load(
    const std::string& file)
{
    std::ifstream fs(file);

    if (!fs.is_open())
    {
        return false;
    }


    json root;

    fs >> root;


    auto left = root["left"];

    m_left.front =
        left["front"].get<std::string>();

    m_left.brokerID =
        left["broker"].get<std::string>();

    m_left.userID =
        left["user"].get<std::string>();

    m_left.password =
        left["password"].get<std::string>();

    m_left.flowPath =
        left["flowPath"].get<std::string>();



    auto right = root["right"];

    m_right.front =
        right["front"].get<std::string>();

    m_right.brokerID =
        right["broker"].get<std::string>();

    m_right.userID =
        right["user"].get<std::string>();

    m_right.password =
        right["password"].get<std::string>();

    m_right.flowPath =
        right["flowPath"].get<std::string>();



    for (auto& item :
        root["subscribe"])
    {
        m_instruments.push_back(
            item.get<std::string>());
    }


    return true;
}

const MdLineConfig& Config::Left() const
{
    return m_left;
}

const MdLineConfig& Config::Right() const
{
    return m_right;
}

const std::vector<std::string>& Config::Instruments() const
{
    return m_instruments;
}
