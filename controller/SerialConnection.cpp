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
        currentReadPosition = buffer;
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
            sendError("Buffer overflow!");
            sendError(String("Buffer: \"") + buffer + "\"");
        }
    }

    void SerialConnection::sendError(String msg)
    {
      Serial.write((String("[ERROR] \"") + msg + ";\"\n").c_str());
    }

    Line SerialConnection::readLine()
    {
        char* currentLine = currentReadPosition;
        char* separatorPosition = strchr(currentLine, ';'); // find next occurrence of ';' from currentLine start
        if(separatorPosition != 0)
        {
            *separatorPosition = 0; // remove ';' - separate into two different 0 terminated strings
            currentReadPosition = separatorPosition + 1; // jump to next line
            return Line(currentLine, separatorPosition);
        }

        return Line(currentLine, currentLine);
    }

    Line::Line(char* data, char* end)
        : current(data)
        , lineEnd(end)
    {

    }

    char* Line::parseString()
    {      
        if(current >= lineEnd)
            return nullptr;

        char* start = current;
        char* end = strchr(current, ' ');
        if (end != 0)
        {
            *end = 0;
            current = end + 1;
        } else {
            current = lineEnd;
        }
        return start;
    }

    float Line::parseFloat()
    {
        char* str = parseString();
        if(str)
            return atof(str);
        Serial.println("[ERROR:] Could not parse float!;\n");
        return 0.0f;
    }

    int Line::parseInt()
    {
        char* str = parseString();
        if(str)
            return atoi(str);
        return -1;
    }

    short Line::countParts() const
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

    bool Line::isEmpty() const
    {
        return current == nullptr || current >= lineEnd || *current == 0;
    }

    Line::operator bool() const
    {
      return !isEmpty();
    }
};
