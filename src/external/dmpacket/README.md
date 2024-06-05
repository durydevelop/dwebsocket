### DMPacket
Technically DMPacket is a vector of bytes with a set of extended functions to easy store and extract data types (primitives, strings, buffer of bytes).
 * Mainly created for transmit sensors data form mcu->mcu or mcu->sbc, data type as integer (8, 16 bit) float, byte, word, dword can be easly stored and extracted.
 * You can also use it to transmit and receive data over any type of comunication channel.

 ## Futures
* [x] Push data into buffer and shift out from the other side (FIFO).
* [ ] Push data into buffer and pop from the other side (FILO).
* [x] Write data into buffer (in any position) and read from the other side (from any position).

# platformio
This folder contains library.json file that permits to be added to platformio project as external lib:
lib_deps = 
	symlink://../../../../lib-mcu/dpptools-mcu/src/dmpacket

## Usage:

```cpp

```
