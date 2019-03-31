#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#include <HardwareSerial.h>

namespace RobotTools
{
    // A line returned by RobotTools::SerialConnection
    // Consists of one or multiple parts (separated by ' ' in the original data).
    // Parsing a part of the line using one of the provided functions consumes that part.
    class Line
    {
    public:
        // data points to the beginning of the line, end to the address after the last readable character
        Line(char* data, char* end);
        char* parseString();
        float parseFloat();
        int parseInt();
        short countParts() const; // how many parts separated by ' ' does this line have?
        bool isEmpty() const; // no further part left?

        // is this line valid? (not null)
        operator bool() const;

    private:
        char* current;
        char* lineEnd;
    };

    // Reads *whole* lines (terminated by ';') from the serial connection.
    // This is different from the standard Serial implementation in that 
    // no partial lines will be returned. (They are buffered until they are complete).
    // ';' is used as line ending character because the Java library used (on the other end of the connection)
    // does not allow for discerning \n (new line in input) from \0 (no further data in the serial connection).
    class SerialConnection
    {
    public:
        SerialConnection();
        ~SerialConnection();

        // initialize the connection
        void setup();
        
        // Read from the serial connection into the buffer.
        // Needs to be called regularly.
        void updateBuffer();

        // Returns the next line from the buffer or nullptr if none is available.
        // The line is terminated by \0.
        // CAREFUL: The memory region is still owned by this instance of the class.
        //          DON'T delete it. 
        //          DON'T keep the pointer for future use - the content may change. (if necessary copy)
        Line readLine();

        // sends an error message
        void sendError(String message);

    private:
        void leftAlignBuffer();
        void readBytesToBuffer();
        void handleOverflow();

    private:
        char* buffer;
        char* currentReadPosition;
        size_t lastReadSize;
        int offset;
    }; // class SerialConnection

}; // namespace RobotTools

#endif // SERIALCONNECTION_H
