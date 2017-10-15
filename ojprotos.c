#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ojprotos.h"

char struct_names[OJPMAX][OJPLEN];	// possible structures in a file, each 50 chars long
struct SMemberTypes members[OJPMAX]; 	// members in a struct.

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
	  return OJPTRUE;
	}
      if(strcmp(struct_names[ii], to_find) == 0)
	{
	  return OJPFALSE;
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
	  return OJPTRUE;
	}
      if(strcmp(members[ii].name, to_find) == 0)
	{
	  return OJPFALSE;
	}
    }
  fprintf(stderr, "cannot add more members.\n");
  return OJPERR;
}


int get_last_struct()
{
  int ii;
  for(ii = 1; ii < OJPMAX; ++ii)
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
  if((ii = get_last_struct() ) != OJPERR && ii+1 < OJPMAX)
    {
      strcpy(struct_names[ii+1], to_add);
      return OJPOK;
    }
  else
    {
      return OJPERR;
    }
}

int prepare_file(char* filename)
{
  FILE* fh;
  fh = fopen(filename, "w");
  fprintf(fh, "#include <stdio.h>\n");
  fprintf(fh, "#include <arpa/inet.h>\n");
  fprintf(fh, "#include <string.h>\n");
  fprintf(fh, "\n\n\n");
  fprintf(fh, "short ojp_ntohs(short val){\n");
  fprintf(fh, "\treturn ntohs(val);\n");
  fprintf(fh, "}\n");
  fprintf(fh, "\n\n\n");
  fprintf(fh, "long ojp_ntohl(long val){\n");
  fprintf(fh, "\treturn ntohl(val);\n");
  fprintf(fh, "}\n");
  fprintf(fh, "\n\n\n");
  fprintf(fh, "short ojp_htons(short val){\n");
  fprintf(fh, "\treturn htons(val);\n");
  fprintf(fh, "}\n");
  fprintf(fh, "\n\n\n");
  fprintf(fh, "long ojp_htonl(long val){\n");
  fprintf(fh, "\treturn htonl(val);\n");
  fprintf(fh, "}\n");
  fprintf(fh, "\n\n\n");
  
  fclose(fh);
  return 0;
}

int print_last_struct(char* filename)
{
  FILE* fh;
  int lastii;
  
  fh = fopen(filename, "a");
  fprintf(fh, "\n\n");
  
  lastii = get_last_struct();
  fprintf(fh, "%s_ser(struct %s const *ins, char* buffer){\n",
	  struct_names[lastii], struct_names[lastii]);
  fprintf(fh, "\tint offset = 0;\n");
  fprintf(fh, "\tuint32_t *ptr32;\n");
  fprintf(fh, "\tuint16_t *ptr16;\n");
  fprintf(fh, "\tuint8_t *ptr8;\n");
  fprintf(fh, "\tuint32_t val32;\n");
  fprintf(fh, "\tuint16_t val16;\n");
  fprintf(fh, "\tuint8_t val8;\n\n");
  int member_ii;
  member_ii = 0;
  while(members[member_ii].name[0] != 0 && member_ii < OJPMAX)
    {
      int jj;
      jj = 0;
     
      if(members[member_ii].isStruct == OJPFALSE)
	{   
	  if(   members[member_ii].eMemberType == MT_UINT32
	     || members[member_ii].eMemberType == MT_INT32)
	    {
	      while(jj <= members[member_ii].arraySize)
		{
		  fprintf(fh, "\tval32 = (uint32_t) ojp_ntohl(ptr32[%d]);\n", jj);
		  fprintf(fh, "\tmemcpy(&buffer[offset], &val32, sizeof(val32));\n");
		  fprintf(fh, "\toffset += sizeof(val)\n\n");
		  ++jj;
		}
	    }
	  else if(    members[member_ii].eMemberType == MT_UINT16
		   || members[member_ii].eMemberType == MT_INT16 )
	    {
	      while(jj <= members[member_ii].arraySize)
		{
		  fprintf(fh, "\tval16 = (uint16_t) ojp_ntohs(ptr16[%d]);\n", jj);
		  fprintf(fh, "\tmemcpy(&buffer[offset], &val16, sizeof(val));\n");
		  fprintf(fh, "\toffset += sizeof(val16)\n\n");
		  ++jj;
		}
	    }
	  else
	    {
	      while(jj <= members[member_ii].arraySize)
		{
		  fprintf(fh, "\tval8 = ptr8[%d];\n", jj);
		  fprintf(fh, "\tmemcpy(&buffer[offset], &val8, sizeof(val8));\n");
		  fprintf(fh, "\toffset += sizeof(val8)\n\n");
		  ++jj;
		}
	    }
	}
      else
	{
	  fprintf(fh, "\tstruct %s *%s_ptr;\n", members[member_ii].structName, members[member_ii].name);
	  fprintf(fh, "\t%s_ptr = &(ins->%s[%d]);\n"
		 , members[member_ii].name
		 , members[member_ii].name
		 , jj);
	  while(jj <= members[member_ii].arraySize)
	    {
	      
	      fprintf(fh, "\toffset += %s_ser(&ins->%s, &buffer[offset]);\n\n",
		  members[member_ii].structName, members[member_ii].name);
	      ++jj;
	    }

	}
      member_ii++;
    }
  fprintf(fh, "\treturn offset;\n");
  fprintf(fh, "}\n\n");
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
      return OJPTRUE;
    }
  return OJPFALSE;
}

int can_add_struct_member(char* member)
{
  printf("Checking member %s\n", member);
  if(struct_exist(member) == OJPTRUE)
    {
      return OJPTRUE;
    }
  return OJPFALSE;
}

int can_add_struct(char* structname)
{
  printf("Checking struct %s\n", structname);
  if(struct_exist(structname) != OJPFALSE)
    {
      return OJPFALSE;
    }
  return OJPTRUE;
}

