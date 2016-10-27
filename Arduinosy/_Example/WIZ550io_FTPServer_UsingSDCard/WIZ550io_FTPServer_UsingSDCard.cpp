// Do not remove the include below
/*
 * This sketch demostrate the use of FTP server library
 * Copyright (c) 2014 by kaizen8501
 */
#include "WIZ550io_FTPServer_UsingSDCard.h"
#include <SPI.h>
#include <Ethernet.h>
#include "SdList.h"
#include "FtpServer.h"

#define SPI_SCK 13
#define SPI_DI  12
#define SPI_DO  11
#define SD_CS    4  // Chip select line for SD card

#define FTP_CTRL_PORT	21
#define FTP_DATA_PORT_DFLT 20    // Default data port in active mode
#define FTP_DATA_PORT_PASV 55600 // Data port in passive mode

#define FTP_TIME_OUT 5           // Disconnect client after 5 minutes of inactivity
#define FTP_CMD_SIZE 256         // max size of a command
#define FTP_CWD_SIZE 256         // max size of a directory name
#define FTP_FIL_SIZE 128         // max size of a file name
#define FTP_BUF_SIZE 512         // size of file buffer for read/write

// this must be unique
byte mac[] = { 0x00, 0x08, 0xDC, 0x11, 0x21, 0x33 };

// change to your network settings
IPAddress ip( 192, 168, 15, 101 );
IPAddress gateway( 192, 168, 15, 1 );
IPAddress subnet( 255, 255, 255, 0 );
// fill in your Domain Name Server address here:
IPAddress myDns(8, 8, 8, 8); // google puble dns

SdList sdl;
FtpServer ftpSrv;


void setup()
{
	delay(3000); // for embedded MCU in WIZ550IO
	Serial.begin(9600);

#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
	Ethernet.begin(ip, myDns, gateway, subnet);
#else
	Ethernet.begin(mac, ip, myDns, gateway, subnet);
#endif

	delay(2000);

	if(sdl.begin(4) == 0)
	{
		Serial.println(("SD init fail"));
	}

	Serial.println("FtpSrv Init\r\n");
	ftpSrv.init();
}

void loop()
{
	ftpSrv.service();
}






