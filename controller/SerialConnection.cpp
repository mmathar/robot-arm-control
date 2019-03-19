#include "SerialConnection.h"

namespace RobotTools
{
    namespace // accessible from only this file
    {
        #define BUFFER_SIZE 256
    }; // namespace

    SerialConnection::SerialConnection()
        : buffer(new char[BUFFER_SIZE]())
        , currentReadPosition(buffer)
        , lastReadSize(0)
        , offset(0)
    {

    }

    SerialConnection::~SerialConnection()
    {
        if(buffer)
            delete[] buffer;
    }

    void SerialConnection::setup()
    {
        Serial.begin(9600);
        Serial.setTimeout(1);
        Serial.println("Controller initialized!;\n");
    }

    void SerialConnection::updateBuffer()
    {
        if(!Serial.available())
            return;

        leftAlignBuffer();
        readBytesToBuffer();
        handleOverflow();
    }

    // Remove consumed lines from the buffer. 
    // Moves the currentReadPosition and unconsumed contents to the beginning of the 
    // buffer and updates the offset accordingly.
    void SerialConnection::leftAlignBuffer()
    {
        char* bufferEnd = buffer + (lastReadSize + offset); 
        if(currentReadPosition < bufferEnd) {
            offset = bufferEnd - currentReadPosition;
            memmove(buffer, currentReadPosition, offset);
        } else {
            offset = 0;
        }
    }

    // Fetch available data from the serial connection (into the buffer).
    void SerialConnection::readBytesToBuffer()
    {
        lastReadSize = Serial.readBytes(buffer + offset, BUFFER_SIZE - 1 - offset);
        buffer[lastReadSize + offset] = 0;
    }

    // just for debugging -> write a warning to the serial port
    void SerialConnection::handleOverflow()
    {
        if(BUFFER_SIZE - 1 - offset <= 0) {
            Serial.write("Buffer overflow!;\n");
            Serial.write((String("Buffer: \"") + buffer + "\";\n").c_str());
        }
    }

    Line SerialConnection::readLine()
    {
        char* currentLine = currentReadPosition;
        char* separatorPosition = strchr(currentLine, ';'); // find next occurrence of ';' from currentLine start
        if(separatorPosition != 0)
        {
            *separatorPosition = 0; // remove ';' - separate into two different 0 terminated strings
            currentReadPosition = separatorPosition + 1; // jump to next line
            return Line(currentLine);
        }

        return Line(nullptr);
    }

    Line::Line(char* data)
        : current(data)
    {

    }

    char* Line::parseString()
    {      
        char* start = current;
        char* end = strchr(current, ' ');
        if (end != 0)
        {
            *end = 0;
            current = end + 1;
            return start;
        }
        return nullptr;
    }

    float Line::parseFloat()
    {
        char* str = parseString();
        if(str)
            return atof(str);
        return 0.0f;
    }

    int Line::parseInt()
    {
        char* str = parseString();
        if(str)
            return atoi(str);
        return -1;
    }

    short Line::countParts()
    {
        short count = 1;
        char* position = current;
        while(position = strchr(position, ' '))
        {
            count++;
            position = position + 1;
        }
        return count;
    }

    bool Line::isEmpty()
    {
        return *current == 0;
    }

    Line::operator bool() const
    {
      return current != nullptr && *current != 0;
    }
};
