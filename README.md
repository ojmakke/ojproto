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
}
````

What is a good way to send this?

Perhaps Google Protocol Buffers is a good way. That can be used whenever possible. However, using Google Protocol Buffers is not always practical. Say a 16 bytes frame is always sent, regardless of how much data is actually used in the 16 bytes frame. 

Or say that there will always be a header (or set of headers) of known size and format, such as IP header. What would be a quick way to serialize / deserialize the header to send it on the wire?

Also, dealing with Google Protocol Buffer is not always pleasant. Suppose you have a game engine and a server where you want to send data between the two, such as Unreal Engine 4. Integrating Google Protocol Buffer in Unreal Engine 4 may take more time than it took to create this tool. 

Also, having to install 10's of Megabytes if not hundreds to transmit few hundred bytes per message is rediculous sometimes, especially when there is enough bandwidth. 

This is not to claim that this tool is better. It is not, whatever that better means. It is just much easier to integrate and use. Here is how.

Run the ````compileme```` script. It will create a binary called ````ser````

Then, execute:

````ser struct_file output.c````

There are few simple rules.
In the ````struct_file```` , It must begin with ````// VERSION x```` where x is an integer. This is in case changes are made in the future.

Also, the structs to be serialized must be surrounded by: ````// OJPROTO  .....  // OJPROTOEND````

This is to allow you to have more structs *after* the ````// OJPROTOEND```` . You can always include header files at the top. 

See the ````sample.ojp```` for an example.

The ````output.c```` file will have few files. Say ````struct MyStruct```` is the struct name. 

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
 The functions: ````ntohs, ntohl, htons, htonl```` are used in ````ojp_ntohs, ojp_ntohl, ojp_htons, ojp_htonl ````. You can reimplement these for systems such as windows. Future plans include generating these functions automatically.
 
 ## Regarding supported types:
 1. ````uint32_t, int32_t, uint16_t, int16_t, uint8_t, int8_t, char```` and arrays of these types.
 2. Structure members and arrays of them, if they are already defined, which works for valid C code
 
 Enjoy!
