#pragma once

#include <Arduino.h>

namespace RaceConfig
{
    struct Record;
};

namespace TimeUtils
{
    String getTimeString(unsigned long time);
    void resetRecord(RaceConfig::Record* record);
};