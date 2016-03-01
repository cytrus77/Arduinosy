#ifndef SD_LIST_H
#define SD_LIST_H

#include "SD.h"

class SdList : public SDClass
{
public:
  SdList();

  bool tesset();
  bool chdir();
  bool chdir( const char* path );

  bool rename(char const*, char const*);

  bool nextFile( char * name, bool * pIsF = NULL, uint32_t * pSize = NULL );
  bool openFile( SdFile * pFile, const char* name, uint8_t oflag );

  float capacity();
  float free();
};

#endif // SD_LIST_H
