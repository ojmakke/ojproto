#ifndef __OJ_PROTOS_H__
#define __OJ_PROTOS_H__

#include <stdint.h>

#define OJPMAX (4096)
#define OJPLEN (50)
#define OJPOK (1)
#define OJPERR (-1)
#define OJPTRUE (1)
#define OJPFALSE (0)


enum EMemberTypes
  {
    MT_UNDEFINED = 0,
    MT_UINT32,
    MT_INT32,
    MT_UINT16,
    MT_INT16,
    MT_UINT8,
    MT_INT8,
    MT_CHAR,
    MT_STRUCT,
  };

struct SMemberTypes
{
  char name[OJPLEN];
  char structName[OJPLEN];
  uint32_t eMemberType;
  int isArray;
  int isStruct;
  uint32_t arraySize;  
};

// Exposed functoins to the world (public)
void init_members();
void init_all();
int add_struct(char* to_add);
int add_member(struct SMemberTypes *member);
int prepare_file(char* filename, char* to_include);
int print_last_struct(char* filename);
int can_add_primitive_member(char* member);
int can_add_struct_member(char* member);
int can_add_struct(char* structname);
int type_to_str(char* type);



#endif
