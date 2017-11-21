#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ojprotos.h"

char struct_names[OJPMAX][OJPLEN];	// possible structures in a file, each 50 chars long
struct SMemberTypes members[OJPMAX]; 	// members in a struct.
char header_name[] = "ojp_headerauto.h\0";

void init_structs()
{
  int ii;
  for(ii = 0; ii < OJPMAX; ++ii)
    {
      memset(struct_names[ii], 0, OJPLEN);
    } 
}

void init_members()
{
  int ii;
  for(ii = 0; ii < OJPMAX; ++ii)
    {
      memset(&members[ii], 0, sizeof(members[ii]));
    } 
}

void init_all()
{
    int ii;
    init_structs();
    init_members();
}

int type_to_str(char* type)
{
  if(strcmp(type, "uint32_t\0") == 0) return MT_UINT32;
  else if(strcmp(type, "int32_t\0") == 0) return MT_INT32;
  else if(strcmp(type, "uint16_t\0") == 0) return MT_UINT16;
  else if(strcmp(type, "int16_t\0") == 0) return MT_INT16;
  else if(strcmp(type, "uint8_t\0") == 0) return MT_UINT8;
  else if(strcmp(type, "int8_t\0") == 0) return MT_INT8;
  else if(strcmp(type, "char\0") == 0) return MT_CHAR;
  else return MT_UNDEFINED;
}

int struct_exist(char* to_find)
{
  int ii;
  for(ii = 0; ii < OJPMAX; ++ii)
    {
      if(struct_names[ii][0] == 0)
	{
	  return OJPFALSE;
	}
      if(strcmp(struct_names[ii], to_find) == 0)
	{
	  return OJPTRUE;
	}
    }
  fprintf(stderr, "cannot add more structs.\n");
  return OJPERR;
}

int member_exist(char* to_find)
{
  int ii;
  for(ii = 0; ii < OJPMAX; ++ii)
    {
      if(members[ii].name[0] == 0)
	{
	  return OJPFALSE;
	}
      if(strcmp(members[ii].name, to_find) == 0)
	{
	  return OJPTRUE;
	}
    }
  fprintf(stderr, "cannot add more members.\n");
  return OJPERR;
}


int get_last_struct()
{
  int ii;
  for(ii = 0; ii < OJPMAX; ++ii)
    {
      if(struct_names[ii][0] == 0)
	{
	  return ii-1; /* last one was last one */
	}
    }
  fprintf(stderr, "structs are full\n");
  return OJPERR;
}

int get_last_member()
{
  int ii;
  for(ii = 0; ii < OJPMAX; ii++)
    {
      if(members[ii].name[0] == 0) return ii;
    }
  fprintf(stderr, "Too many members\n");
  return ii;
}

int add_struct(char* to_add)
{
  int ii;
  ii = get_last_struct();
  if(ii+1 < OJPMAX && ii+1 >= 0)
    {
      strcpy(struct_names[ii+1], to_add);
      return OJPOK;
    }
  else
    {
      printf("add struct error. ii is %d\n", ii+1);
      return OJPERR;
    }
}

int prepare_file(char* filename, char* to_include)
{
  FILE* fh;
  fh = fopen(filename, "w");
  fprintf(fh, "#include <stdio.h>\n");
  fprintf(fh, "#include <stdlib.h>\n");
  fprintf(fh, "#include <stdint.h>\n");
  fprintf(fh, "#include <string.h>\n");
  fprintf(fh, "#include \"%s\"\n", header_name);
  fprintf(fh, "#include \"%s\"\n", to_include);
  
  fprintf(fh, "\n\n");
  fprintf(fh, "extern uint32_t ojp_ntohl(uint32_t val);\n");
  fprintf(fh, "extern uint16_t ojp_htons(uint16_t val);\n");
  fprintf(fh, "extern uint32_t ojp_htonl(uint32_t val);\n");
  fprintf(fh, "extern uint16_t ojp_htons(uint16_t val);\n");
  fprintf(fh, "extern uint32_t insert_l_be_shifted(uint8_t * bufferOut, uint32_t bitOffset, uint32_t bitSize, uint32_t val);\n");
  fprintf(fh, "extern uint32_t extract_l_shifted(uint8_t *buffer, uint32_t bitOffset, int bitSize);\n");
  fprintf(fh, "\n");
  fprintf(fh, "\n");
  
  fclose(fh);
  return OJPOK;
}

int prepare_header()
{
  FILE *fh;
  fh = fopen(header_name, "w");
  fprintf(fh, "#ifndef __OJP_AUTOGENERATED_H__\n");
  fprintf(fh, "#define __OJP_AUTOGENERATED_H__\n\n");
  fprintf(fh, "#include \"replace_with_header_of_structs.h\"\n");
  fprintf(fh, "#include <stddef.h>\n");
  fprintf(fh, "#include <stdint.h>\n\n");
  fclose(fh);
  return OJPOK;
}

int end_header()
{
  FILE* fh;
  fh = fopen(header_name, "a");
  fprintf(fh, "#endif\n");
  fclose(fh);
  return OJPOK;
}

uint32_t get_next_field_size(int member_ii)
{

  int fieldSize = 0;
  uint32_t bytes;
  int whole;
  while(member_ii < OJPMAX && members[member_ii].isBitField == OJPTRUE)
    {
      fieldSize += members[member_ii].fieldSize;
      ++member_ii;
    }
  whole = fieldSize / 8; /* Field is this or more bytes */
  bytes = (whole * 8 < fieldSize) ? (whole+1) : whole; /* bytes is how many bytes the field is. */
  return bytes;
}

void print_struct_ser(FILE* fh, int lastii)
{
  int member_ii;
  FILE *header;
  header = fopen(header_name, "a");
  fprintf(header,"size_t %s_ser(struct %s *ins, uint8_t* buffer);\n",
	  struct_names[lastii], struct_names[lastii]);
  fclose(header);
  
  fprintf(fh, "size_t %s_ser(struct %s *ins, uint8_t* buffer){\n",
	  struct_names[lastii], struct_names[lastii]);
  fprintf(fh, "\tint offset = 0;\n");
  fprintf(fh, "\tuint32_t *ptr32;\n");
  fprintf(fh, "\tuint16_t *ptr16;\n");
  fprintf(fh, "\tuint8_t *ptr8;\n");
  fprintf(fh, "\tuint32_t val32;\n");
  fprintf(fh, "\tuint16_t val16;\n");
  fprintf(fh, "\tuint32_t bitOffset;\n");
  fprintf(fh, "\tuint8_t val8;\n\n");
  fprintf(fh, "\tif(ins == NULL || buffer == NULL){\n");
  fprintf(fh, "\t\tprintf(\"WARNIGN: Passed NULL pointer to %s_ser\\n\");\n", struct_names[lastii]);
  fprintf(fh, "\t}\n\n");

  member_ii = 0;
  while(member_ii < OJPMAX && members[member_ii].name[0] != 0)
    {
      int jj;
      jj = 0;
      if(members[member_ii].isStruct == OJPFALSE && members[member_ii].isBitField == OJPFALSE)
	{   
	  if(   members[member_ii].eMemberType == MT_UINT32
		|| members[member_ii].eMemberType == MT_INT32)
	    {
	      if(members[member_ii].isArray == OJPTRUE)
		{
		  fprintf(fh, "\tptr32 = &(ins->%s[0]);\n", members[member_ii].name);
		}
	      else
		{
		  fprintf(fh, "\tptr32 = &(ins->%s);\n", members[member_ii].name);
		}
	      
	      while(jj <= members[member_ii].arraySize)
		{
		  fprintf(fh, "\tval32 = (uint32_t) ojp_ntohl(ptr32[%d]);\n", jj);
		  fprintf(fh, "\tmemcpy(&buffer[offset], &val32, sizeof(val32));\n");
		  fprintf(fh, "\toffset += sizeof(val32);\n\n");
		  ++jj;
		}
	    }
	  else if( members[member_ii].eMemberType == MT_UINT16
		   || members[member_ii].eMemberType == MT_INT16)
			
	    {
	      if(members[member_ii].isArray == OJPTRUE)
		{
		  fprintf(fh, "\tptr16 = &(ins->%s[0]);\n", members[member_ii].name);
		}
	      else
		{
		  fprintf(fh, "\tptr16 = &(ins->%s);\n", members[member_ii].name);
		}
	      while(jj <= members[member_ii].arraySize)
		{
		  fprintf(fh, "\tval16 = (uint16_t) ojp_ntohs(ptr16[%d]);\n", jj);
		  fprintf(fh, "\tmemcpy(&buffer[offset], &val16, sizeof(val16));\n");
		  fprintf(fh, "\toffset += sizeof(val16);\n\n");
		  ++jj;
		}
	    }
	  else if(members[member_ii].isBitField == OJPFALSE)
	    {
	      /* For chars and uint8s, a simple copy will do */
	      if(members[member_ii].isArray == OJPTRUE)
		{
		  fprintf(fh, "\tmemcpy(&buffer[offset], &(ins->%s[0]), %d);\n", members[member_ii].name, members[member_ii].arraySize);
		  fprintf(fh, "\toffset += %d;\n", members[member_ii].arraySize);
		}
	      else
		{
		  fprintf(fh, "\tbuffer[offset] = ins->%s;\n", members[member_ii].name);
		}
	    }
	}
      /* Struct */
      else if(members[member_ii].isBitField == OJPFALSE)
	{
	  fprintf(fh, "\tstruct %s *%s_ptr;\n", members[member_ii].structName, members[member_ii].name);
	  if(members[member_ii].isArray == OJPTRUE)
	    {
	      fprintf(fh, "\t%s_ptr = &(ins->%s[0]);\n"
		      , members[member_ii].name
		      , members[member_ii].name);
	    }
	  else
	    {
	      fprintf(fh, "\t%s_ptr = &(ins->%s);\n"
		      , members[member_ii].name
		      , members[member_ii].name);
	    }
	  
	  while(jj <= members[member_ii].arraySize)
	    {
	      
	      fprintf(fh, "\toffset += %s_ser(&%s_ptr[%d], &buffer[offset]);\n\n",
		      members[member_ii].structName, members[member_ii].name, jj);
	      ++jj;
	    }
	  
	}

      /* bitField = true is the remaining else. This cannot be array */
      else
	{
	  uint32_t bytes = get_next_field_size(member_ii);
	  int fieldSize;
	  fprintf(fh, "\t/* Note that the leftmost bit (MSB) is always aligned at a byte. */\n");
	  fprintf(fh, "\tbitOffset = 0;\n");
	  
	  while(member_ii < OJPMAX && members[member_ii].isBitField == OJPTRUE)
	    {
	      fieldSize = members[member_ii].fieldSize;
	      if(fieldSize > 32)
		{
		  printf("Warning: Found fieldSize > 32: %s\n", members[member_ii].name);
		}
	      fprintf(fh, "\tbitOffset = insert_l_be_shifted(&buffer[offset], bitOffset, %d, ins->%s);\n", fieldSize, members[member_ii].name);	      
	      ++member_ii;
	    }	  
	  --member_ii; /* Undo last increment in loop */
	  /* Move offset to the next available byte */
	  fprintf(fh, "\toffset += ((bitOffset - 1) >> 3) + 1;\n");
	  fprintf(fh, "\tbitOffset = 0;\n\n");

	}
      member_ii++;
    }
  fprintf(fh, "\treturn offset;\n");
  fprintf(fh, "}\n\n");
}

void print_struct_size(FILE* fh, int lastii)
{
  int member_ii;
  FILE* header;
  header = fopen(header_name, "a");
  fprintf(header, "size_t %s_size();\n", struct_names[lastii]);
  fclose(header);
  /* now print the function which returns the size required from the buffer */
  fprintf(fh, "size_t %s_size(){\n", struct_names[lastii]);
  fprintf(fh, "\tsize_t offset;\n");
  fprintf(fh, "\tstruct %s dummy;\n", struct_names[lastii]);
  fprintf(fh, "\toffset = 0;\n\n");
  member_ii = 0;
  while(member_ii < OJPMAX && members[member_ii].name[0] != 0)
    {
      int jj;
      jj = 0;
      if(members[member_ii].isStruct == OJPFALSE && members[member_ii].isBitField == OJPFALSE)
	{
	  fprintf(fh, "\toffset += sizeof(dummy.%s);\n", members[member_ii].name);
	}
      else if(members[member_ii].isBitField == OJPFALSE)
	{
	  if(members[member_ii].isArray == OJPFALSE)
	    {
	       fprintf(fh, "\toffset += %s_size();\n", members[member_ii].structName);
	    }
	  else
	    {
	      fprintf(fh, "\toffset += %s_size()*%d;\n"
		      , members[member_ii].structName
		      , members[member_ii].arraySize);
	    }
	}
      /* bitFields. Find all consecutive bitFields, since they will be packed. */
      else
	{
	  uint32_t bytes;
	  bytes = get_next_field_size(member_ii);
	  /* For array shift, we need bytes + 1 array at most */
	  // uint8_t *toShift = malloc(bytes+1);
	  //memset(toShift, 0, bytes+1);
	  fprintf(fh, "\toffset += %d;\n", bytes);
	}
      ++member_ii;
    }
  fprintf(fh, "\treturn offset;\n");
  fprintf(fh, "}\n\n");
 
}

void print_struct_des(FILE* fh, int lastii)
{
  int member_ii;
  FILE* header;
  header = fopen(header_name, "a");
  fprintf(header, "size_t %s_des(struct %s *ins, uint8_t* buffer);\n",
	  struct_names[lastii], struct_names[lastii]);
  fclose(header);
	
  fprintf(fh, "size_t %s_des(struct %s *ins, uint8_t* buffer){\n",
	  struct_names[lastii], struct_names[lastii]);
  fprintf(fh, "\tuint32_t offset = 0;\n");
  fprintf(fh, "\tuint32_t bitOffset;\n");
  fprintf(fh, "\tuint32_t *ptr32;\n");
  fprintf(fh, "\tuint16_t *ptr16;\n");
  fprintf(fh, "\tuint8_t *ptr8;\n");
  fprintf(fh, "\tuint32_t val32;\n");
  fprintf(fh, "\tuint16_t val16;\n");
  fprintf(fh, "\tuint8_t val8;\n\n");

  fprintf(fh, "\tif(ins == NULL || buffer == NULL){\n");
  fprintf(fh, "\t\tprintf(\"WARNIGN: Passed NULL pointer to %s_des\\n\");\n", struct_names[lastii]);
  fprintf(fh, "\t}\n\n");

  fprintf(fh, "\tmemset(ins, 0, sizeof(*ins));\n");
  
  member_ii = 0;
  while(member_ii < OJPMAX && members[member_ii].name[0] != 0)
    {
      int jj;
      jj = 0;
      if(members[member_ii].isStruct == OJPFALSE && members[member_ii].isBitField == OJPFALSE)
	{   
	  if(   members[member_ii].eMemberType == MT_UINT32
	     || members[member_ii].eMemberType == MT_INT32)
	    {
	      while(jj <= members[member_ii].arraySize)
		{
		  fprintf(fh, "\tptr32 = &buffer[offset];\n");
		  fprintf(fh, "\tval32 = ojp_htonl(*ptr32);\n");
		  if(members[member_ii].isArray == OJPTRUE)
		    {
		      fprintf(fh, "\tptr32 = &(ins->%s[%d]);\n", members[member_ii].name, jj);
		    }
		  else
		    {
		      fprintf(fh, "\tptr32 = &(ins->%s);\n", members[member_ii].name);
		    }
		  /* ptr32 now points to either the variable, or the offset to where to copy data */
		  fprintf(fh, "\tmemcpy(ptr32, &val32, sizeof(val32));\n");
		  fprintf(fh, "\toffset += sizeof(val32);\n\n");
		  ++jj;
		}
	    }
	  else if(    members[member_ii].eMemberType == MT_UINT16
		   || members[member_ii].eMemberType == MT_INT16 )
	    {
	      while(jj <= members[member_ii].arraySize)
		{
		  fprintf(fh, "\tptr16 = &buffer[offset];\n");
		  fprintf(fh, "\tval16 = ojp_htons(*ptr16);\n");
		  if(members[member_ii].isArray == OJPTRUE)
		    {
		      fprintf(fh, "\tptr16 = &(ins->%s[%d]);\n", members[member_ii].name, jj);
		    }
		  else
		    {
		      fprintf(fh, "\tptr16 = &(ins->%s);\n", members[member_ii].name);
		    }
		  /* ptr16 now points to either the variable, or the offset to where to copy data */
		  fprintf(fh, "\tmemcpy(ptr16, &val16, sizeof(val16));\n");
		  fprintf(fh, "\toffset += sizeof(val16);\n\n");
		  ++jj;
		}
	    }
	  else if(members[member_ii].isArray == OJPTRUE)
	    {
	      /* For uint8_t or chars, we can simply do memcpy instead of copying 1 by 1. No endianess */
	      fprintf(fh, "\tmemcpy(&(ins->%s), &buffer[offset], %d);\n", members[member_ii].name, members[member_ii].arraySize);
	      fprintf(fh, "\toffset += %d;\n", members[member_ii].arraySize);
	    }
	  else
	    {
	      fprintf(fh, "\tins->%s = buffer[offset];\n", members[member_ii].name);
	      fprintf(fh, "\toffset += sizeof(val8);\n");
	    }
	}
    
      // Bit field. If it is struct, then C Compiler will complain later.
      else if(members[member_ii].isBitField == OJPTRUE)
	{
	  /* Apply all consecutive bit fields. Hence another loop */
	  fprintf(fh, "\n\tbitOffset = 0;\n");
	  while(member_ii < OJPMAX && members[member_ii].name[0] != 0 && members[member_ii].isBitField == OJPTRUE)
	    {
	      fprintf(fh, "\tins->%s = extract_l_shifted(&buffer[offset], bitOffset, %d);\n", members[member_ii].name, members[member_ii].fieldSize);
	      fprintf(fh, "\tbitOffset += %d;\n", members[member_ii].fieldSize);
	      ++member_ii;
	    }
	  --member_ii; /* Undo last one in loop */
	  /* Move offset to the next byte */
	  fprintf(fh, "\toffset += ((bitOffset -1) >> 3) + 1;\n");
	  fprintf(fh, "\t bitOffset = 0;\n\n");
	  
	}
      // Struct
      else
	{
	  fprintf(fh, "\tstruct %s *%s_ptr;\n", members[member_ii].structName, members[member_ii].name);
	  if(members[member_ii].isArray == OJPTRUE)
	    {
	      fprintf(fh, "\t%s_ptr = &(ins->%s[0]);\n"
		      , members[member_ii].name
		      , members[member_ii].name);
	    }
	  else
	    {
	      fprintf(fh, "\t%s_ptr = &(ins->%s);\n"
		      , members[member_ii].name
		      , members[member_ii].name);
	    }
	  
	  while(jj <= members[member_ii].arraySize)
	    {
	      
	      fprintf(fh, "\toffset += %s_des(&%s_ptr[%d], &buffer[offset]);\n\n",
		      members[member_ii].structName, members[member_ii].name, jj);
	      ++jj;
	    }

	}
      member_ii++;
    }
  fprintf(fh, "\treturn offset;\n");
  fprintf(fh, "}\n\n");

}

int print_last_struct(char* filename)
{
  FILE* fh;
  int lastii;
  int member_ii;
  
  fh = fopen(filename, "a");
  fprintf(fh, "\n\n");
  
  lastii = get_last_struct();
  print_struct_ser(fh, lastii);
  print_struct_size(fh, lastii);
  print_struct_des(fh, lastii);

  /* Now print the function for deserialize. It returns the offset of unused buffer. Tis can be otuside bound */
  /*****
   *****
   ****
   */
  fclose(fh);
  return 0;
}

int add_member(struct SMemberTypes *member)
{
  int ii;
  ii = get_last_member();
  memcpy(&members[ii], member, sizeof(struct SMemberTypes));
  return OJPOK;
}

int can_add_primitive_member(char* member)
{
  if(member_exist(member) == OJPTRUE)
    {
      printf("member %s exists\n", member);
      return OJPFALSE;
    }
  return OJPTRUE;
}

int can_add_struct_member(char* member)
{
  if(struct_exist(member) == OJPTRUE)
    {
      return OJPTRUE;
    }
  printf("member's type  %s does not exist\n", member);
  return OJPFALSE;
}

int can_add_struct(char* structname)
{
  if(struct_exist(structname) != OJPFALSE)
    {
      printf("Struct type %s already exists\n", structname);
      return OJPFALSE;
    }
  return OJPTRUE;
}

