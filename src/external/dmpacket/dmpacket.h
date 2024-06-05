#ifndef DMPACKET_H
#define DMPACKET_H

#ifdef ARDUINO
	// Arduino inlcudes
    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "wiring.h"
        #include "WProgram.h"
    #endif
#endif

#include <vector>
#include <string>

class DMPacket {
    public:
        DMPacket();
        DMPacket(uint8_t buff[], uint16_t buffSize);
        DMPacket(const std::vector<uint8_t>& buffVec);
        ~DMPacket();

        void clear();
        uint8_t size(void);
        uint8_t* rawBuffer(void);
        std::vector<uint8_t>& buffer(void);

        void setBuffer(const uint8_t buff[], uint16_t buffLen);
        void setBuffer(const char buff[], uint16_t buffLen);
        void setBuffer(const std::vector<uint8_t>& buffVec);

        uint8_t readByte(uint16_t offset);
        uint16_t readWord(uint16_t offset);
        uint32_t readDWord(uint16_t offset);
        int8_t readInt8(uint16_t offset);
        int16_t readInt16(uint16_t offset);
        float readFloat(uint16_t offset);
        std::vector<uint8_t> readBytes(uint16_t offset, uint16_t count = 0);
        uint16_t readBytes(std::vector<uint8_t>& dest, uint16_t offset, uint16_t count = 0);
        std::vector<uint16_t> readWords(uint16_t offset, uint16_t count = 0);
        std::vector<uint32_t> readDWords(uint16_t offset, uint16_t count = 0);
        std::string readString(uint16_t offset = 0, uint16_t lenght = 0);

        void writeByte(uint8_t Byte, uint16_t offset);
        void writeWord(uint16_t Word, uint16_t offset);
        void writeDWord(uint32_t DWord, uint16_t offset);
        void writeInt16(int16_t Int, uint16_t offset);
        void writeFloat(float Float, uint16_t offset);
        void writeString(std::string Str, uint16_t offset);
        void writeData(const std::vector<uint8_t>& buffVec, uint16_t offset);
        void writeData(const uint8_t buff[], const uint16_t buffSize, uint16_t offset);

        void pushByte(uint8_t Byte);
        void pushWord(uint16_t Word);
        void pushDWord(uint32_t DWord);
        void pushInt16(int16_t Int);
        void pushFloat(float Float);
        void pushString(std::string str);
        void pushData(const std::vector<uint8_t>& buffVec);
        void pushData(const uint8_t buff[], const uint16_t buffSize);

        uint8_t shiftByte(void);
        uint16_t shiftWord(void);
        uint32_t shiftDWord(void);
        std::string shiftString(uint16_t lenght = 0);

        std::string toHexString(uint16_t offset = 0);
        std::string toAsciiString(uint16_t offset = 0);

    private:
    	std::vector<uint8_t>packetBuff;
        uint16_t shiftIndex;

};

#endif // DMPACKET_H
