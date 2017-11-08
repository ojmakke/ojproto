# ojproto

This program is a serializer / deserializer for C structures. A possible usecase isto send data over network, receive it, and put it back into the structure.

Say for example the following structure has to be serialized:

````C
struct SBasicData
{
  uint8_t member1;
  uint32_t member2[10];
};

struct SHeader
{
  uint16_t version;
  uint32_t msg_type;
};
````

What is a good way to send this?

Perhaps Google Protocol Buffers is a good way. That can be used whenever possible. However, using Google Protocol Buffers is not always practical. Say a 16 bytes frame is always sent, regardless of how much data is actually used in the 16 bytes frame. 

Or say that there will always be a header (or set of headers) of known size and format, such as IP header. What would be a quick way to serialize / deserialize the header to send it on the wire?

Also, dealing with Google Protocol Buffer is not always pleasant. Suppose you have a game engine and a server where you want to send data between the two, such as Unreal Engine 4. Integrating Google Protocol Buffer in Unreal Engine 4 may take more time than it took to create this tool. 

Also, having to install 10's of Megabytes if not hundreds to transmit few hundred bytes per message is rediculous sometimes, especially when there is enough bandwidth. 

This is not to claim that this tool is better. It is not, whatever that better means. It is just much easier to integrate and use. Here is how.

## How to compile

Run the ````compileme```` script. It will create a binary called ````ser````

Then, execute:

````ser struct_file output.c````

For example, a sample.ojp is included. The file name *sample.ojp" is normally a file called something like "messages.h", and is included in other C files.

````ser sample.ojp sample.c````

## Some Rules For struct_file
There are few simple rules.
In the ````struct_file```` , It must begin with ````// VERSION x```` where x is an integer. This is in case changes are made in the future.

Also, the structs to be serialized must be surrounded by: ````// OJPROTO  .....  // OJPROTOEND````

This is to allow you to have more structs *after* the ````// OJPROTOEND```` . You can always include header files at the top. 

See the ````sample.ojp```` for an example.

## What is Generated
The ````output.c```` file will have 3 functions per defined structure. Say ````struct MyStruct```` is the struct name in sample.ojp.

Then there will be:
````size_t MyStruct_ser(struct MyStruct *ins, char* buffer)````

1. *ins is a pointer to the structure to serialize
2. buffer is a pointer to array to put the "bytes" of the structure into.
3. Returns an integer pointing to the first unused byte in the buffer, or end of buffer. Use ````MyStruct_size()```` to see if there are more bytes.

````size_t MyStruct_des(struct MyStruct *ins, char* buffer)````

1. *ins is a pointer to the structure to deserialize
2. buffer is a pointer to array which contains the bytes.
3. Returns an integer pointing to the first unused byte in the buffer, or end of buffer. Use ````MyStruct_size()```` to see if there are more bytes.

````size_t MyStruct_size()````

1. This returns the size of the structure. This can be used to ensure that buffer is of correct size, before passing it to the serializer/deserializer.
 
 So, given a series of bytes, we can call serializer or deserializer multiple types to serialize, deserialize multiple structures. 
 
 ## Regarding Endianess
 The functions: ```` ojp_ntohs, ojp_ntohl, ojp_htons, ojp_htonl ```` are used instead of ```` ntohs, ntohl, htons, htonl ````.   They may not be as efficient, but they are simple and readable.
 
 Data is stored as Network Order (Big Endian). For example, a ```` uint32_t x = 1; ```` is stored in the buffer as:
 ````
 buffer[0] = 0;
 buffer[1] = 0;
 buffer[2] = 0;
 buffer[3] = 1;
 ````
 
 Regarding the bitfields, they are stored so that the left most bit is the most significant, and the rightmost bit is the least significant. 
 
  
 
 ## Regarding supported types:
 1. ````uint32_t, int32_t, uint16_t, int16_t, uint8_t, int8_t, char```` and arrays of these types.
 2. Structure members and arrays of them, if they are already defined, which works for valid C code
 3. Bit fields.
 
 Enjoy!
