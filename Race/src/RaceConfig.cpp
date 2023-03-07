#include "RaceConfig.h"

#ifdef USE_SPIFFS
#include <SPIFFS.h>
File file;
static const char configFileName[] = "/olr.cfg";
#else
#include <EEPROM.h>
#endif

#include "Car.h"
#include "Controller.h"
#include "OilObstacle.h"
#include "RampObstacle.h"
#include "TimeUtils.h"
#include "Coroutines/LedLightingCoroutine.h"

using namespace TimeUtils;

namespace RaceConfig
{
    bool RaceStarted = true;

    word MaxLoops = DEFAULT_LOOPS;
    word MaxLED = DEFAULT_LED;

    DynamicArray<Player> Players(MAX_PLAYERS);
    DynamicPointerArray<IObstacle *> Obstacles(2);

    Record AllTimeRecord;
    Record CurrentRecord;
    Record EZAllTimeRecord;
    Record EZCurrentRecord;

    Adafruit_NeoPixel track(MaxLED, PIN_LED, NEO_GRB + NEO_KHZ800);
    
#ifdef LED_CIRCLE
    Adafruit_NeoPixel circle(MAXLEDCIRCLE, PIN_CIRCLE, NEO_GRB + NEO_KHZ800);
#endif //LED_CIRCLE

    void writeWord(int &offset, word data)
    {
#ifdef USE_SPIFFS
        file.write((uint8_t*)&data, sizeof(word));
#else
        EEPROM.writeUInt(offset, data);
#endif
        offset += sizeof(int);
    }

    void readWord(int &offset, word &data)
    {
#ifdef USE_SPIFFS
        file.read((uint8_t*)&data, sizeof(word));
#else
        EEPROM.get(offset, data);
#endif
        offset += sizeof(word);
    }

    void writeUInt(int &offset, uint32_t data)
    {
        writeWord(offset, (word)data);
    }

    void readUInt(int &offset, uint32_t &data)
    {
        word wordData;
        readWord(offset, wordData);
        data = wordData;
    }

    void writeULong(int& offset, unsigned long data)
    {
#ifdef USE_SPIFFS
        file.write((uint8_t*)&data, sizeof(unsigned long));
#else
        EEPROM.writeULong(offset, data);
#endif
        offset += sizeof(int);
    }

    void readULong(int &offset, unsigned long &data)
    {
#ifdef USE_SPIFFS
        file.read((uint8_t*)&data, sizeof(unsigned long));
#else
        EEPROM.get(offset, data);
#endif
        offset += sizeof(word);
    }

    void writeByte(int &offset, byte data)
    {
#ifdef USE_SPIFFS
        file.write(data);
#else
        EEPROM.writeByte(offset, data);
#endif

        offset += sizeof(byte);
    }

    void readByte(int &offset, byte &data)
    {
#ifdef USE_SPIFFS
        file.readBytes((char *)&data, 1);
#else
        EEPROM.get(offset, data);
#endif
        offset += sizeof(byte);
    }

    void writeString(int &offset, const char *data)
    {
        size_t len = strlen(data) + 1;

#ifdef USE_SPIFFS
        writeWord(offset, len);
        file.write((uint8_t *)data, len);
#else
        EEPROM.writeBytes(offset, data, len);
#endif
        offset += len * sizeof(char);
    }

    void readString(int &offset, char *data)
    {
#ifdef USE_SPIFFS
        size_t len;
        readWord(offset,len);
        file.read((uint8_t *)data, len);
        offset += len * sizeof(char);
#else

        for (int i = 0; i < MAX_NAME_LENGTH; ++i)
        {
            readByte(offset, data[i]);
            if (data[i] == '\0')
            {
                return;
            }
        }
#endif
    }

    void SaveRecord(int &offset)
    {
#ifdef USE_SPIFSS
        file = SPIFFS.open(configFileName, "a");
        if (!file)
        {
            Serial.println("Error opening file for saving new record.");
            return;
        }
        file.seek(offset);
#endif
        //Always reserve the maximum allowed size for a name so we don't accidentally wipe the data after it when we update only this section
        int begin = offset;
        writeString(offset, AllTimeRecord._name);
        offset = begin + (MAX_NAME_LENGTH * sizeof(char));
        writeULong(offset, AllTimeRecord._time);
        writeUInt(offset, AllTimeRecord._color);
        // writeULong(offset, AllTimeRecord._date);

        begin = offset;
        writeString(offset, EZAllTimeRecord._name);
        offset = begin + (MAX_NAME_LENGTH * sizeof(char));
        writeULong(offset, EZAllTimeRecord._time);
        writeUInt(offset, EZAllTimeRecord._color);
        // writeULong(offset, EZAllTimeRecord._date);

#ifdef USE_SPIFSS
        file.close();
#endif
    }

    void LoadRecord(int& offset)
    {
        int begin = offset;
        readString(offset, AllTimeRecord._name);
        offset = begin + (MAX_NAME_LENGTH * sizeof(char));
        readULong(offset, AllTimeRecord._time);
        readUInt(offset, AllTimeRecord._color);
        // readULong(offset, AllTimeRecord._date);
        AllTimeRecord._date = 0;

        begin = offset;
        readString(offset, EZAllTimeRecord._name);
        offset = begin + (MAX_NAME_LENGTH * sizeof(char));
        readULong(offset, EZAllTimeRecord._time);
        readUInt(offset, EZAllTimeRecord._color);
        // readULong(offset, EZAllTimeRecord._date);
        EZAllTimeRecord._date = 0;

        resetRecord(&CurrentRecord);
        resetRecord(&EZCurrentRecord);
    }

    void deleteRecord()
    {
        resetRecord( &AllTimeRecord );
        resetRecord( &CurrentRecord );
        resetRecord( &EZAllTimeRecord );
        resetRecord( &EZCurrentRecord );
        
        int offset = strlen(SAVE_FILE_VERSION)+1;
        SaveRecord(offset);
    }

    void Save()
    {
#ifdef USE_SPIFFS
        file = SPIFFS.open(configFileName, "w");
        if (!file)
        {
            Serial.println("Unable to open SPIFFS file for saving!");
            return;
        }
        file.seek(0);
#endif
        int eeAddress = 0;
        Serial.printf("Saving file version \"%s\".\n", SAVE_FILE_VERSION);
        writeString(eeAddress, SAVE_FILE_VERSION);
        
        Serial.println("Saving record.");
        SaveRecord(eeAddress);

        Serial.printf("Saving Max Loops %d.\n", MaxLoops);
        writeWord(eeAddress, MaxLoops);

        size_t count = MAX_PLAYERS;

        for (size_t i = 0; i < count; ++i)
        {
            Serial.printf("Saving Player %d:\n", i);
            Player &player = Players[i];
            
            Serial.printf("\tColor %d\n", player.getColor());
            writeUInt(eeAddress, player.getColor());
            
            Serial.printf("\tName \"%s\"\n", player.getName());
            writeString(eeAddress, player.getName());
        }
        
        count = Obstacles.Count();
        Serial.printf("Saving Obstacle Count %d.\n", count);
        writeWord(eeAddress, count);

        for (size_t i = 0; i < count; ++i)
        {
            Serial.printf("Saving Obstacle %d:\n", i);
            IObstacle *obstacle = Obstacles[i];

            Serial.printf("\tColor %d\n", obstacle->getColor());
            writeUInt(eeAddress, obstacle->getColor());

            Serial.printf("\tStart %d\n", obstacle->getStart());
            writeWord(eeAddress, obstacle->getStart());

            Serial.printf("\tEnd %d\n", obstacle->getEnd());
            writeWord(eeAddress, obstacle->getEnd());

            Serial.printf("\tType %d\n", obstacle->getType());
            writeWord(eeAddress, obstacle->getType());

            switch (obstacle->getType())
            {
            case IObstacle::OBSTACLE_OIL:
            {
                OilObstacle *oil = static_cast<OilObstacle *>(obstacle);
                
                Serial.printf("\tOil Press Delay %d\n", oil->getPressDelay());
                writeWord(eeAddress, oil->getPressDelay());
            }
            break;

            case IObstacle::OBSTACLE_RAMP:
            {
                RampObstacle *ramp = static_cast<RampObstacle *>(obstacle);

                Serial.printf("\tRamp Height %d\n", ramp->getHeight());
                writeByte(eeAddress, ramp->getHeight());

                Serial.printf("\tRamp Style %d\n", ramp->getStyle());
                writeWord(eeAddress, ramp->getStyle());
            }
            break;
            }
        }
#if defined(ESP32) && !defined(USE_SPIFFS)
        EEPROM.commit();
#endif

#ifdef USE_SPIFFS
        file.close();
#endif
        Serial.println("Saving done!");
    }

    bool Load()
    {
#ifdef USE_SPIFFS
        file = SPIFFS.open(configFileName, "r");

        if (!file)
        {
            Serial.println("Unable to open SPIFFS file for loading!");
            return false;
        }
        file.seek(0);
#endif
        Serial.println("Loading...");
        int eeAddress = 0;

        char version[MAX_NAME_LENGTH];
        readString(eeAddress, version);
        Serial.printf("Saved file is version \"%s\"; current supported version is \"%s\".\n", version, SAVE_FILE_VERSION);
        if (strcmp(version, SAVE_FILE_VERSION) != 0)
        {
            Serial.println("Wrong save file version.");
#ifdef USE_SPIFFS
            file.close();
#endif
            return false;
        }

        LoadRecord(eeAddress);
        
        readWord(eeAddress, MaxLoops);
        Serial.printf("Loaded Maxloops=%d\n", MaxLoops);

        MaxLED = DEFAULT_LED;
        track.updateLength(MaxLED);
        Serial.println("Track length updated");

        word count = MAX_PLAYERS;

        //Explicitly destroy current players contents to remove all internal pointers
        //This avoids memory leaks since the ponters are not automatically destroyed in ~Player() by design
        for (int i = 0; i < Players.Count(); ++i)
        {
            Serial.printf("Destroying player %d\n", i);
            Players[i].Destroy();
        }
        Players.Clear();

        for (int i = 0; i < count; ++i)
        {
            Serial.printf("Loading Player %d:\n", i);

            uint32_t color = 0;
            readUInt(eeAddress, color);
            Serial.printf("\tColor %d\n", color);

            char name[MAX_NAME_LENGTH];
            readString(eeAddress, name);
            Serial.printf("\tName \"%s\"\n", name);

            byte pin = 0;
            byte lightPin = 0;
            switch (i)
            {
            case 0:
                pin = PIN_P1;
                lightPin = LIGHT_PIN_P1;
                break;
            
            case 1:
                pin = PIN_P2;
                lightPin = LIGHT_PIN_P2;
                break;

            case 2:
                pin = PIN_P3;
                lightPin = LIGHT_PIN_P3;
                break;

            case 3:
                pin = PIN_P4;
                lightPin = LIGHT_PIN_P4;
                break;

            default:
                Serial.printf("Error loading player %d", i);
            }
            Serial.printf("\tControl Pin %d\n\tLight Pin %d\n", pin, lightPin);

            Players.Add(Player(color, pin, lightPin, name));
            Serial.println("Player loaded");
        }

        readWord(eeAddress, count);
        Serial.printf("Obstacle Count %d.\n", count);

        Obstacles.Clear();

        for (size_t i = 0; i < count; ++i)
        {
            Serial.printf("Loading Obstacle %d:\n", i);

            uint32_t color = 0;
            readUInt(eeAddress, color);
            Serial.printf("\tColor %d\n", color);

            word start = 0;
            readWord(eeAddress, start);
            Serial.printf("\tStart %d\n", start);

            word end = 0;
            readWord(eeAddress, end);
            Serial.printf("\tEnd %d\n", end);

            word type;
            readWord(eeAddress, type);
            Serial.printf("\tType %d\n", type);

            switch ((IObstacle::ObstacleType)type)
            {
            case IObstacle::OBSTACLE_OIL:
            {
                word pressDelay = 0;
                readWord(eeAddress, pressDelay);
                Serial.printf("\tOil Press Delay %d\n", pressDelay);

                OilObstacle *oil = new OilObstacle(start, end, color);
                Serial.println("Oil obstacle created");
                oil->setPressDelay(pressDelay);

                Obstacles.Add(oil);
                Serial.println("Oil obstacle loaded");
            }
            break;

            case IObstacle::OBSTACLE_RAMP:
            {
                byte height = 0;
                readByte(eeAddress, height);
                Serial.printf("\tRamp Height %d\n", height);

                word style;
                readWord(eeAddress, style);
                Serial.printf("\tRamp Style %d\n", style);

                RampObstacle *ramp = new RampObstacle(start, end, height, color, (RampObstacle::RampStyle)style);
                Serial.println("Ramp obstacle created");

                Obstacles.Add(ramp);
                Serial.println("Ramp obstacle loaded");
            }
            break;
            }
        }
        Serial.println("Loading done!");
#ifdef USE_SPIFFS
        file.close();
#endif
        return true;
    }

    bool checkAndSaveRecord(const Player* player, unsigned long racetime)
    {
        Record* allTimeRecord = NULL;
        Record* currentRecord = NULL;

        allTimeRecord = &AllTimeRecord;
        currentRecord = &CurrentRecord;

        int minutesRT = racetime / 60000;
        float secondsRT = (racetime - (minutesRT * 60000)) / 1000.f;
        Serial.println(racetime);
        Serial.printf("Chrono:  %02d:%.3f", minutesRT, secondsRT);
        Serial.println(" ");

        int minutesCR = currentRecord->_time/ 60000;
        float secondsCR = ((currentRecord->_time)- (minutesCR * 60000)) / 1000.f;
        Serial.println(currentRecord->_time);
        Serial.printf("Current Record:  %02d:%.3f", minutesCR, secondsCR);
        Serial.println(" ");

        int minutesAT = allTimeRecord->_time/ 60000;
        float secondsAT = ((allTimeRecord->_time)- (minutesAT * 60000)) / 1000.f;
        Serial.println(allTimeRecord->_time);
        Serial.printf("Alltime Record:  %02d:%.3f", minutesAT, secondsAT);
        Serial.println(" ");

      
        strcpy(currentRecord->_name, player->getName());
        currentRecord->_color = player->getColor();
        currentRecord->_time = racetime;
        currentRecord->_date = millis();

        if( currentRecord->_time < allTimeRecord->_time )
        {
            Serial.println("New All time record detected");
            strcpy(allTimeRecord->_name, currentRecord->_name);
            allTimeRecord->_color = currentRecord->_color;
            allTimeRecord->_time = currentRecord->_time;
            allTimeRecord-> _date = currentRecord->_date;

        }

        int offset = strlen(SAVE_FILE_VERSION) + 1;

        Serial.println("Saving record");
        SaveRecord(offset);

        return true;
        

    }

    void Delete()
    {
#ifdef USE_SPIFFS
        SPIFFS.remove(configFileName);
#endif
    }
};