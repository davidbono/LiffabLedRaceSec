#pragma once

#include "Defines.h"
#include <Arduino.h>

class Stream;

#define EOL '\n' // End of Command char used in Protocol
#define REC_COMMAND_BUFLEN 32
#define TX_COMMAND_BUFLEN 64

class SerialCommunication
{
private:
    static SerialCommunication* _instance;
    SerialCommunication();

protected:
    char _receiveBuffer[REC_COMMAND_BUFLEN];
    char _transmitBuffer[TX_COMMAND_BUFLEN];

    Stream *_stream;
    word _receiveIndex;

public:
    static SerialCommunication& instance();

    void SendCommand(const char *formattedString, ...);
    const char* ReadSerial(word& length);

protected:
    void WriteToStream(const char *command);
};