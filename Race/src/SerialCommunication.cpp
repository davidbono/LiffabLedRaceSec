#include "SerialCommunication.h"

#include <Arduino.h>

SerialCommunication* SerialCommunication::_instance = NULL;

SerialCommunication::SerialCommunication() : _stream(&Serial),
                                             _receiveIndex(0)
{
}

SerialCommunication& SerialCommunication::instance()
{
    if( _instance == NULL )
    {
        _instance = new SerialCommunication();
    }
    return *_instance;
}

void SerialCommunication::SendCommand(const char *formattedString, ...)
{
    va_list args;

    va_start(args, formattedString);
    vsprintf(_transmitBuffer, formattedString, args);
    va_end(args);

    WriteToStream(_transmitBuffer);
}

const char *SerialCommunication::ReadSerial(word &length)
{
    while (_stream->available())
    {
        if (_receiveIndex < REC_COMMAND_BUFLEN - 2)
        {
            char data = _stream->read();
            if (data == EOL)
            {
                length = _receiveIndex+1;
                _receiveBuffer[length] = '\0';
                _receiveIndex = 0;
                break;
            }
            else
            {
                ++_receiveIndex;
                _receiveBuffer[_receiveIndex] = data;
            }
        }
        else
        {
            // buffer full
            // reset and retunn error
            _receiveBuffer[_receiveIndex+1] = '\0';
            _receiveIndex = 0;
            length = -1;
            break;
        }
    }

    return _receiveBuffer;
}

void SerialCommunication::WriteToStream(const char *command)
{
    int len = 0;
    for (; len < TX_COMMAND_BUFLEN; ++len)
    {
        if (*(command + len) == EOL)
        {
            ++len; // send EOL
            break;
        }
    }
    _stream->write(command, len);
}
