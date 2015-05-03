#ifndef PARSER_H
#define PARSER_H

#include <Types.h>
#include <Federate.h>
#include <Struct.h>

#include <string>

#ifndef EXCLUDE_WINSOCK2_H
//#include <winsock2.h>
#endif

extern struct in_addr IP_BaseAddr;
extern uint IP_BasePort, MulticastTtl, ErrorCount;
extern FederateMapType FederateMap;
extern StructMapType StructMap;
extern std::string MDF_File;
extern void (*ErrorMsg)(const char* msg);

void ParseMDFFile(std::string file_name,
                  void (*error_msg_func)(const char* msg) = 0);

void GenerateCppCode( void );
void GenerateMDFData( void );

#endif
