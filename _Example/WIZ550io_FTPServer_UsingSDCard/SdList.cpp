#include "SdList.h"
#include "utility/SdFat.h"
#include "utility/SdFatUtil.h"
#include "SD.h"

//  Sd2Card card;
//  SdVolume volume;
//  SdFile root;
SdList::SdList()
{
}

bool SdList::chdir()
{
	SDClass::root.close();
	return root.openRoot(volume);
}

bool SdList::chdir( const char* path )
{
	SdFile dir;
	if( path[0] == '/' && path[1] == '\0')
	{
		return chdir();
	}
	if( !dir.open(root, path, O_READ) )
	{
		goto fail;
	}
	if( !dir.isDir() )
	{
		goto fail;
	}
	root = dir;

	return true;

fail:
	return false;
}


bool SdList::openFile( SdFile * pFile, const char* name, uint8_t oflag )
{
	return false;//pFile->open(root, name, oflag );                // file opened by its short name
}
//
//// return the capacity in Megabytes of the SD card
//
//float SdList::capacity()
//{
//  Sd2Card * p_card = sd.card();
//  return 0.000512 * p_card->cardSize();
//}
//
//// return the amount of free space in Megabytes in the SD card
//
//float SdList::free()
//{
//  SdVolume * p_vol = sd.vol();
//  return 0.000512 * p_vol->freeClusterCount() * p_vol->blocksPerCluster();
//}
