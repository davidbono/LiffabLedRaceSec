#ifndef RACE_CONFIG
#define RACE_CONFIG

#include <Adafruit_NeoPixel.h>

#include "DynamicArray.h"
#include "DynamicPointerArray.h"
#include "Player.h"
#include "IObstacle.h"

namespace RaceConfig
{
    struct Record
    {
        char _name[MAX_NAME_LENGTH];
        unsigned long _time;
        uint32_t _color;
        unsigned long _date;
    };

    extern bool RaceStarted;
    extern bool EasyMode;
 
    extern word MaxLoops;
    extern word MaxLED;

    extern DynamicArray<Player> Players;
    extern DynamicPointerArray<IObstacle *> Obstacles;

    extern Record AllTimeRecord;
    extern Record CurrentRecord;
    extern Record EZAllTimeRecord;
    extern Record EZCurrentRecord;

    extern Adafruit_NeoPixel track;
#ifdef LED_CIRCLE
    extern Adafruit_NeoPixel circle;
#endif

    void Save();
    bool Load();
    void Delete();

    bool checkAndSaveRecord(const Player* player, unsigned long time);
    void deleteRecord();
};

#endif //RACE_CONFIG