#include "dmpacket.h"
#ifdef ARDUINO
	#define HIBYTE(w) highByte(w)
	#define LOBYTE(w) lowByte(w)
	#define WORD(hb,lb) word(hb,lb)
	#define LOWORD(dw) ((uint16_t)(dw))
    #define HIWORD(dw) ((uint16_t)(((uint32_t)(dw) >> 16) & 0xFFFF))
	#define DWORD_B(hb,next_hb,next_lb,lb) (((uint32_t)hb << 24) | ((uint32_t)next_hb << 16) | ((uint32_t)next_lb << 8) | lb)
#else
    //typedef uint8_t BYTE;
    //typedef uint16_t WORD;
    //typedef uint32_t DWORD;
    #include <sstream>
    #include <iomanip>
    #include <algorithm>
    #define bitRead(x, n) (((x) >> (n)) & 0x01)
    #define LOWORD(l) ((unsigned short)(l))
    #define HIWORD(l) ((unsigned short)(((unsigned int)(l) >> 16) & 0xFFFF))
    #define LOBYTE(w) ((char)(w&0x00FF))
    #define HIBYTE(w) ((char)((w>>8)&0x00FF))
    #define WORD(msb,lsb) ((msb << 8) | lsb)
    #define DWORD(msw,lsw) ((msw << 16) | lsw)
    #define DWORD_B(msb,next_msb,next_lsb,lsb) (msb << 24) | (next_msb << 16) | (next_lsb << 8) | lsb
#endif

/**
 * @brief Crea un pacchetto vuoto
 */
DMPacket::DMPacket() {
	clear();
};

/**
 * @brief Crea un pacchetto eseguendo il parsing del buffer passato
 * @param Buff		->	buffer di bytes.
 */
DMPacket::DMPacket(const std::vector<uint8_t>& buffVec) {
	setBuffer(buffVec);
};

/**
 * @brief Crea un pacchetto eseguendo il parsing del buffer passato
 * @param buffVec	->  un std::vector<uint8_t>.
 */
DMPacket::DMPacket(uint8_t buff[], uint16_t buffSize) {
	setBuffer(buff,buffSize);
};

//! Distruttore
DMPacket::~DMPacket() {
};

/**
 * @brief Resetta il pacchetto.
 * -Riporta la dimensione a HDR_SIZE.
 * -Azzera il contenuto.
 * -Azzera li shiftIndex
 */
void DMPacket::clear() {
	packetBuff.resize(0);
	packetBuff.shrink_to_fit();
    shiftIndex=0;
}

//! @return la lunghezza in bytes del paccketto
uint8_t DMPacket::size(void) {
	return(packetBuff.size());
}

/**
 * @brief Copia Buff direttamente nel buffer del pacchetto.
 * 
  * @param Buff		->  buffer di bytes.
 * @param BuffSize	->	lunghezza del buffer (max 256).
 *
 **/
void DMPacket::setBuffer(const uint8_t buff[], uint16_t buffSize) {
	// Set buffer size
	packetBuff.resize(buffSize);
	// Fill it
	//packetBuff.assign(Buff,BuffLen);
	for (uint8_t ixB=0; ixB<buffSize; ixB++) {
		packetBuff[ixB]=buff[ixB];
	}
    // Zeros shift index
    shiftIndex=0;
}

void DMPacket::setBuffer(const char buff[], uint16_t buffSize) {
    setBuffer((uint8_t *) buff,buffSize);
}

/**
 * @brief Copia buffVect direttamente nel buffer del pacchetto.
 * 
 * @param buffVec	->  un std::vector<uint8_t>.
 * @return true 
 * @return false 
 */
void DMPacket::setBuffer(const std::vector<uint8_t>& buffVec) {
	packetBuff=buffVec;
    shiftIndex=0;
}

//! @return un puntatore all'intero buffer del pacchetto.
/**
 * @brief 
 * Non usare per inserire dati
 * @return uint8_t* 
 */
uint8_t* DMPacket::rawBuffer() {
	return(packetBuff.data());
}

//! @return Il riferimeto al buffer totale del pacchetto come vector.
std::vector<uint8_t>& DMPacket::buffer() {
	return(packetBuff);
}

/**
 * @brief Convert packet content into an hex string.
 * 
 * @param offset    ->  offset in buffer.
 * @return std::string containing an hex rappresentation of packet content like 01:F2:FF:E6
 */
std::string DMPacket::toHexString(uint16_t offset) {
    if (packetBuff.empty()) {
        return std::string();
    }
    uint16_t MemSize=(packetBuff.size()-offset)*3; // 2 bytes + ':' for each byte
    char *HexStr=new char[MemSize];
    char *itr=&HexStr[0]; // iterator pointer
    //size_t ixP;
    for (size_t ixP=offset; ixP<packetBuff.size(); ixP++) {
        itr+=sprintf(itr,"%02X:",packetBuff[ixP]);
    }
    std::string Ret=std::string(HexStr,MemSize-1); // (-1 because last byte have no ':' to the end)
    delete HexStr;
    return Ret;
}

/**
 * @brief Convert packet content to an ascii string.
 * 
 * @param offset    ->  offset in buffer.
 * @return std::string a string containing ascii convertion of all bytes in buffer (characters that can not be printed are replaced with '.')
 */
std::string DMPacket::toAsciiString(uint16_t offset) {
    if (packetBuff.empty()) {
        return std::string();
    }
    uint16_t MemSize=(packetBuff.size()-offset)*3;
    char *AsciiStr=new char[MemSize];
    char *itr=&AsciiStr[0]; // iterator pointer
    //size_t ixP;
    for (size_t ixP=offset; ixP<packetBuff.size(); ixP++) {
        itr+=sprintf(itr,"%c  ",isprint(packetBuff[ixP]) ? packetBuff[ixP] : '.');
    }
    std::string Ret=std::string(AsciiStr,MemSize);
    delete AsciiStr;
    return Ret;
}

// ********************************************************************************************************
// ****************************************** Read...ing methods ******************************************
// ********************************************************************************************************

/**
 * @brief Read a byte from data payload.
 * @param Index ->  offset of byte to read in the payload.
 * @return 8 bit value.
 */
uint8_t DMPacket::readByte(uint16_t offset)
{
    if (offset < packetBuff.size()) {
        return packetBuff[offset];
    }
    else {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            return 0xFF;
        #endif
    }
}

/**
 * @brief Read a word from data payload.
 * @param Index ->  offset of byte in the payload where to start reading.
 * @return 16 bit value.
 */
uint16_t DMPacket::readWord(uint16_t offset)
{
    if (offset+1 < packetBuff.size()) {
	    return WORD(packetBuff[offset],packetBuff[offset+1]);
    }
    else {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            return 0xFFFF;
        #endif
    }
}

/**
 * @brief Read a double-word from data payload.
 * @param Index ->  offset of byte in the payload where to start reading.
 * @return 32 bit value.
 */
uint32_t DMPacket::readDWord(uint16_t offset)
{
    if (offset+3 < packetBuff.size()) {
	    return DWORD_B(packetBuff[offset],packetBuff[offset+1],packetBuff[offset+2],packetBuff[offset+3]);
    }
    else {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            return 0xFFFFFFFF;
        #endif
    }
}

/**
 * @brief Read a 8 bit signed integer from data payload.
 * @param Index ->  offset of byte to read in the payload.
 * @return integer value.
 */
int8_t DMPacket::readInt8(uint16_t offset)
{
	return((int8_t)readByte(offset));
}

/**
 * @brief Read a 16 bit signed integer from data payload.
 * @param Index ->  offset of byte to read in the payload.
 * @return 16 bit integer value.
 */
int16_t DMPacket::readInt16(uint16_t offset)
{
	return((int16_t)readWord(offset));
}

/**
 * @brief Read a 32 bit float from data.
 * @param Index ->  Number of the float to read (first is 1).
 * @return float value.
 */
float DMPacket::readFloat(uint16_t offset)
{
	uint32_t DWord=readDWord(offset);
	// Important cast from dword to float
	//return(*(float*)&DWord);
	//return(float(DWord));
    union {
        float ff;
        uint32_t ii;
    }d;
    d.ii=DWord;
    return d.ff;
}

/**
 * @brief Read a boolean from data payload.
 * @param Index ->  offset of byte to read in the payload.
 * @return true if readed byte is non-zero otherwise false.
 */
bool DMPacket::readBool(uint16_t offset)
{
    uint8_t Byte=readByte(offset);
    return Byte == 0 ? false : true;
}

/**
 * @brief 
 * 
 * @param offset 
 * @param Len 
 * @return std::string 
 */
std::string DMPacket::readString(uint16_t offset, uint16_t lenght) {
    if (lenght == 0) {
        // To the end of packetBuffer
        lenght=packetBuff.size()-offset;
    }
    else if ((offset+lenght) > packetBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // tronca
            lenght=packetBuff.size()-offset;
        #endif
    }

    std::string sData;
    sData.resize(lenght);
    for (size_t ixS=0; ixS<lenght; ixS++) {
        sData[ixS]=packetBuff[ixS+offset];
    }

    return(sData);
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di bytes (uint8_t)
 * 
 * @return un vector<uint8_t> contenente il payload
 */
std::vector<uint8_t> DMPacket::readBytes(uint16_t offset, uint16_t count)
{
    if (count == 0) {
        // To the end of buffer
        count=packetBuff.size()-offset;
    }
    else if ((offset+count) > packetBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // Truncate to the end of buffer
            count=packetBuff.size()-offset;
        #endif
    }

	std::vector<uint8_t> data;
    data.resize(count);
    for (size_t ixB=0; ixB<count; ixB++) {
        data[ixB]=packetBuff[ixB+offset];
    }

    return data;
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di bytes (uint8_t)
 * 
 * @param Datadest	->	Vettore di bytes in cui mettere i dati
 * @return false se i dati del pacchetto non sono di tipo byte o se è un pacchetto senza senza dati.
 */
uint16_t DMPacket::readBytes(std::vector<uint8_t>& dest, uint16_t offset, uint16_t count)
{
    if (count == 0) {
        // To the end of buffer
        count=packetBuff.size()-offset;
    }
    else if ((offset+count) > packetBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // Truncate to the end of buffer
            count=packetBuff.size()-offset;
        #endif
    }
    
	dest.resize(count);

    for (size_t ixB=0; ixB<count; ixB++) {
        dest[ixB]=packetBuff[ixB+offset];
    }

	return(dest.size());
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di words (uint16_t)
 * 
 * @param Data	->	Vettore di words in cui mettere i dati
 * @return false se i dati del pacchetto non sono di tipo byte o se è un pacchetto senza senza dati.
 */
std::vector<uint16_t> DMPacket::readWords(uint16_t offset, uint16_t count)
{
    if (count == 0) {
        // Truncate to the last block
        uint16_t FreeSpace=packetBuff.size()-offset;
        count=FreeSpace/2;
    }
    else if ((offset+(count*2)) > packetBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // Truncate to the last block
            uint16_t FreeSpace=packetBuff.size()-offset;
            count=FreeSpace/2;
        #endif
    }

    std::vector<uint16_t> data;
	data.reserve(count);

    // Read 2 bytes at time
    for (uint16_t ixD=0; ixD<count; ixD++) {
        data[ixD]=WORD(packetBuff[offset],packetBuff[offset+1]);
        offset+=2;
    }

	return data;
}

/**
 * @brief Legge i dati del pacchetto e li mette in un vettore di double words (uint32_t)
 * 
 * @param Data	->	Vettore di words in cui mettere i dati
 * @return false se i dati del pacchetto non sono di tipo byte o se è un pacchetto senza senza dati.
 */
std::vector<uint32_t> DMPacket::readDWords(uint16_t offset, uint16_t count)
{
    if (count == 0) {
        // Truncate to the last block
        uint16_t FreeSpace=packetBuff.size()-offset;
        count=FreeSpace/4;
    }
    else if (offset+(count*4) > packetBuff.size()) {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            // Truncate to the last block
            uint16_t FreeSpace=packetBuff.size()-offset;
            count=FreeSpace/4;
        #endif
    }

    std::vector<uint32_t> data;
	data.reserve(count);

    // Read 4 bytes at time
    for (uint16_t ixD=count; ixD<packetBuff.size(); ixD++) {
        data[ixD]=DWORD_B(packetBuff[offset],packetBuff[offset+1],packetBuff[offset+2],packetBuff[offset+3]);
        offset+=4;
    }

	return data;
}

// ********************************************************************************************************
// ***************************************** Write...ing methods ******************************************
// ********************************************************************************************************

/**
 * @brief Write a single byte to a specific index to the packet.
 * 
 * @param Byte      ->  byte to write.
 * @param offset    ->  index offset in packetBuffer.
 * 
 * N.B. if offset is out of range, nothing will be done.
 */
void DMPacket::writeByte(uint8_t Byte, uint16_t offset)
{
    if (offset >= packetBuff.size()) {
        return;
    }
	packetBuff[offset]=Byte;
}

/**
 * @brief Write a WORD to a specific index to the packet.
 * 
 * @param Word      ->  byte to write.
 * @param offset    ->  index offset in packetBuffer.
 * 
 * N.B.
 * - If offset is out of range, nothing will be done.
 */
void DMPacket::writeWord(uint16_t Word, uint16_t offset)
{
    if ((offset+1) >= packetBuff.size()) {
        return;
    }
	packetBuff[offset]=HIBYTE(Word);
	packetBuff[offset]=LOBYTE(Word);
}

/**
 * @brief Write a Double Word to a specific index to the packet.
 * 
 * @param DWord     ->  byte to Double Word.
 * @param offset    ->  index offset in packetBuffer.
 * 
 * N.B.
 * - If offset is out of range or data overflows, nothing will be done.
 */
void DMPacket::writeDWord(uint32_t DWord, uint16_t offset)
{
    if ((offset+3) >= packetBuff.size()) {
        return;
    }
	packetBuff[offset]=HIBYTE(HIWORD(DWord));
	packetBuff[offset]=LOBYTE(HIWORD(DWord));
	packetBuff[offset]=HIBYTE(LOWORD(DWord));
	packetBuff[offset]=LOBYTE(LOWORD(DWord));
}

/**
 * @brief Write a 16 bit integer value to a specific index to the packet.
 * 
 * @param Int       ->  integer value to add.
 * @param offset    ->  index offset in packetBuffer.
 * 
 * N.B.
 * - If offset is out of range or data overflows, nothing will be done.
 */
void DMPacket::writeInt16(int16_t Int, uint16_t offset)
{
    if ((offset+1) >= packetBuff.size()) {
        return;
    }
	writeWord(Int,offset);
}

/**
 * @brief Write a 32 bit floating point value to the packet.
 * 
 * @param Float     -> 32 bit float to add.
 * @param offset    ->  index offset in packetBuffer.
 * 
 * N.B.
 * - If offset is out of range or data overflows, nothing will be done.
 */
void DMPacket::writeFloat(float Float, uint16_t offset)
{
    if (offset+3 >= packetBuff.size()) {
        return;
    }
	// Important: cast from float to dword
	//uint32_t f=*(uint32_t*)&Float;
	//uint32_t f=float(Float);
    union {
        float ff;
        uint32_t ii;
    }d;
    d.ff=Float;
	packetBuff[offset]=HIBYTE(HIWORD(d.ii));
	packetBuff[offset]=LOBYTE(HIWORD(d.ii));
	packetBuff[offset]=HIBYTE(LOWORD(d.ii));
	packetBuff[offset]=LOBYTE(LOWORD(d.ii));
}

/**
 * @brief Write a boolean value as byte to a specific index to the packet.
 * 
 * @param Bool      ->  bool value.
 * @param offset    ->  index offset in packetBuffer.
 * 
 * N.B. if offset is out of range, nothing will be done.
 */
void DMPacket::writeBool(bool Bool, uint16_t offset)
{
    if (offset >= packetBuff.size()) {
        return;
    }
	packetBuff[offset]=Bool ? 0x01 : 0x00;
}

/**
 * @brief Write a string as a sequence of bytes into the packet.
 * 
 * @param str       ->  string to add.
 * @param offset    ->  index offset in packetBuffer.
 * 
 * N.B.
 * - If offset is out of range, nothing will be done.
 * - If data overflows, string will be truncated to the end of buffer.
 */
void DMPacket::writeString(std::string str, uint16_t offset)
{
    if (offset >= packetBuff.size()) {
        return;
    }

    // @todo Whats the fastest?
    #ifdef ARDUINO
        for (size_t ixB=offset; ixB<maxOffset; ixB++) {
            packetBuff[ixB]=str[ixB-offset];
        }
    #else
        const auto maxSize = packetBuff.size()-offset;
        std::transform(str.begin(), str.end(), std::next(packetBuff.begin()+offset, maxSize), [](uint8_t c) {
            return c;
        });
    #endif
}

/**
 * @brief Write a vector of bytes into the packet.
 * 
 * @param buffVect  ->  vector of bytes to add.
 * @param offset    ->  index offset in packetBuffer.
 * 
 * N.B.
 * - If offset is out of range, nothing will be done.
 * - If data overflows, buffVect will be truncated to the end of buffer.
 */
void DMPacket::writeData(const std::vector<uint8_t>& buffVec, uint16_t offset) {
    auto maxOffset=offset+buffVec.size();
    if (maxOffset >= packetBuff.size()) {
        maxOffset=packetBuff.size();
    }
    for(uint16_t ixB=offset; ixB<maxOffset; ixB++) {
        packetBuff[ixB]=buffVec[ixB-offset];
    }
}

/**
 * @brief Write a buffer into the packet.
 * 
 * @param buff      ->  buffer pointer.
 * @param buffSize  ->  size of buffer.
 * @param offset    ->  index offset in packetBuffer.
 * 
 * N.B.
 * - If offset is out of range, nothing will be done.
 * - If data overflows, buffVect will be truncated to the end of buffer.
 */
void DMPacket::writeData(const uint8_t buff[], const uint16_t buffSize, uint16_t offset) {
    auto maxOffset=offset+buffSize;
    if (maxOffset >= packetBuff.size()) {
        maxOffset=packetBuff.size();
    }
    for (uint8_t ixB=offset; ixB<maxOffset; ixB++) {
		packetBuff[ixB]=*buff++;
	}
}

// ********************************************************************************************************
// ****************************************** Push...ing methods ******************************************
// ********************************************************************************************************

/**
 * @brief Add a single byte to the packet.
 * 
 * @param Byte  ->  byte to add.
 */
void DMPacket::pushByte(uint8_t Byte)
{
	packetBuff.emplace_back(Byte);
}

/**
 * @brief Add a WORD (2 bytes) to the packet.
 * 
 * @param Word  -> WORD to add.
 */
void DMPacket::pushWord(uint16_t Word)
{
	uint8_t Ix=packetBuff.size();
	packetBuff.resize(Ix+2);
	packetBuff[Ix]=HIBYTE(Word);
	packetBuff[Ix+1]=LOBYTE(Word);
}

/**
 * @brief Add a DWORD (4 bytes) to the packet.
 * 
 * @param DWord ->  DWORD to add.
 */
void DMPacket::pushDWord(uint32_t DWord)
{
	uint8_t Ix=packetBuff.size();
	packetBuff.resize(Ix+4);
	packetBuff[Ix]=HIBYTE(HIWORD(DWord));
	packetBuff[Ix+1]=LOBYTE(HIWORD(DWord));
	packetBuff[Ix+2]=HIBYTE(LOWORD(DWord));
	packetBuff[Ix+3]=LOBYTE(LOWORD(DWord));
}

/**
 * @brief Add a 16 bit integer value to the packet.
 * 
 * @param Int16 
 */
void DMPacket::pushInt16(int16_t Int16)
{
	pushWord(Int16);
}

/**
 * @brief Add a 32 bit floating point value to the packet.
 * 
 * @param Float -> 32 bit float to add.
 */
void DMPacket::pushFloat(float Float)
{
	uint8_t Ix=packetBuff.size();
	packetBuff.resize(Ix+4);
	// Important: cast from float to dword
	//uint32_t f=*(uint32_t*)&Float;
	//uint32_t f=float(Float);
    union {
        float ff;
        uint32_t ii;
    }d;
    d.ff=Float;
	packetBuff[Ix]=HIBYTE(HIWORD(d.ii));
	packetBuff[Ix+1]=LOBYTE(HIWORD(d.ii));
	packetBuff[Ix+2]=HIBYTE(LOWORD(d.ii));
	packetBuff[Ix+3]=LOBYTE(LOWORD(d.ii));
}

/**
 * @brief Add a boolean value as byte to the packet.
 * 
 * @param Bool  ->  bool value.
 */
void DMPacket::pushBool(bool Bool)
{
	packetBuff.emplace_back(Bool ? 0x01 : 0x00);
}

/**
 * @brief Add a string as a sequence of bytes to the packet.
 * 
 * @param str   ->  string to add.
 */
void DMPacket::pushString(std::string str)
{
    #ifdef ARDUINO
        size_t currSize=packetBuff.size();
	    packetBuff.resize(currSize+Str.size());
        for (size_t ixP=currSize; ixP<packetBuff.size(); ixP++) {
            packetBuff[ixP]=Str[ixP-currSize];
        }
    #else
        const auto currSize = packetBuff.size();
        packetBuff.resize(currSize + str.size());
        std::transform(str.begin(), str.end(), std::next(packetBuff.begin(), currSize), [](uint8_t c) {
            return c;
        });
/*
        response.reserve(response.size() + headers.size());  // Optional
std::transform(headers.begin(), headers.end(), std::back_inserter(response),
    [](unsigned char c) { return std::byte{c}; }              // or `encode(c)`
);
*/
    #endif
}

/**
 * @brief Add a vector of bytes to the packet.
 * 
 * @param buffVec   ->  vector of bytes to add.
 */
void DMPacket::pushData(const std::vector<uint8_t>& buffVec) {
    packetBuff.reserve(packetBuff.size() + buffVec.size());
    for(uint16_t ixV=0; ixV<buffVec.size(); ixV++) {
        packetBuff.emplace_back(buffVec[ixV]);
    }
    /*
    for (uint8_t b : buffVec) {
        packetBuff.emplace_back(b);
    }
    */
}

/**
 * @brief Add a vector of bytes to the packet.
 * 
 * @param buff      ->  buffer pointer.
 * @param buffSize  ->  size of buffer.
 */
void DMPacket::pushData(const uint8_t buff[], const uint16_t buffSize) {
    uint16_t startByte=packetBuff.size();
    packetBuff.resize(startByte + buffSize);
    for (uint8_t ixB=startByte; ixB<buffSize; ixB++) {
		packetBuff[ixB]=*buff++;
	}
}

// ********************************************************************************************************
// ***************************************** Shift...ing methods ******************************************
// ********************************************************************************************************

/**
 * @brief Pop a byte from begin of the buffer.
 * It does not really shift the buffer: it use a shift index to set the current pop posision.
 * Really shifting a vector can be an expensive operation.
 * 
 * @return uint8_t 
 */
uint8_t DMPacket::shiftByte(void) {
    if (packetBuff.size() > 0) {
        //return packetBuff[shiftIndex++];
        uint8_t data=packetBuff[shiftIndex];
        shiftIndex++;
        return data;
    }
    else {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            return 0xFF;
        #endif
    }
}

/**
 * @brief Pop a WORD from begin of the buffer.
 * It does not really shift the buffer: it use a shift index to set the current pop posision.
 * Really shifting a vector can be an expensive operation.
 * 
 * @return uint16_t 
 */
uint16_t DMPacket::shiftWord(void) {
    if (packetBuff.size() >= 2) {
        uint16_t data=readWord(shiftIndex);
        shiftIndex+=2;
        return data;
    }
    else {
        #ifdef TROWS_EXCEPTION_ON_READ_OVERFLOW
            throw("Reading over buffer operation not permitted");
        #else
            return 0xFFFF;
        #endif
    }
}

/**
 * @brief Pop a Double Word from begin of the buffer.
 * It does not really shift the buffer: it use a shift index to set the current pop posision.
 * Really shifting a vector can be an expensive operation.
 * 
 * @return uint32_t 
 */
uint32_t DMPacket::shiftDWord(void) {
    if (packetBuff.size() > 0) {
        uint16_t data=readDWord(shiftIndex);
        shiftIndex+=4;
        return data;
    }
    return 0;
}

/**
 * @brief Pop a byte as boolean value from begin of the buffer.
 * It does not really shift the buffer: it use a shift index to set the current pop posision.
 * Really shifting a vector can be an expensive operation.
 * 
 * @return true if readed byte is non-zero otherwise false.
 */
bool DMPacket::shiftBool(void) {
    uint8_t Byte=shiftByte();
    return Byte == 0x00 ? false : true;
}

/**
 * @brief Pop a String from begin of the buffer.
 * It does not really shift the buffer: it use a shift index to set the current pop posision.
 * Really shifting a vector can be an expensive operation.
 * 
 * @param lenght    ->  number of characters to read.
 * @return uint32_t 
 */
std::string DMPacket::shiftString(uint16_t lenght) {
    std::string s=readString(shiftIndex,lenght);
    shiftIndex+=s.size();
    // @todo needs?
    //if (shiftIndex >= packetBuff.size()) {
    //    shiftIndex=packetBuff.size()-1;
    //}
    return s;
}
