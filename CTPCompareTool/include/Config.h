#pragma once

#include <string>
#include <vector>


struct MdLineConfig
{
    std::string front;

    std::string brokerID;
    std::string userID;
    std::string password;

    std::string flowPath;
};


class Config
{
public:

    bool Load(const std::string& file);

    bool Validate();

    const MdLineConfig& Left() const;

    const MdLineConfig& Right() const;


    const std::vector<std::string>& Instruments() const;


private:

    MdLineConfig m_left;

    MdLineConfig m_right;


    std::vector<std::string> m_instruments;
};