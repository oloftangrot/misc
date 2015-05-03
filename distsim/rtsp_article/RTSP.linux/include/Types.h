#ifndef TYPES_H
#define TYPES_H

#ifndef __GNUG__
// get rid of warning (due to STL): identifier truncated to '255' characters
#pragma warning(disable:4786)
#endif
#include <map>
#include <vector>
#include <string>

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

class Federate;
class Message;
class Struct;
class Element;

typedef std::map<std::string, Federate*>    FederateMapType;
typedef std::map<std::string, Message*>     MessageMapType;
typedef std::map<std::string, Struct*>      StructMapType;
typedef std::map<std::string, Element*>     ElementMapType;
typedef std::map<std::string, uint>         ConstMapType;
typedef std::map<uint, Struct*>             StructSeqType;
typedef std::vector<Message*>               MessageVecType;
#endif
