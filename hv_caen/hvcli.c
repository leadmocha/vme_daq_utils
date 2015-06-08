///////////////////////////////////////////////////////////////////////////////
// A command line interface to the CAEN V6533N HV Module
//
// History:
//
// * Wed Mar 04, 2015 Juan Carlos Cornejo <cornejo@jlab.org>
// - Modified to work with complete CAEN library
//
// * Fri Feb 27, 2015 Juan Carlos Cornejo <cornejo@jlab.org>
// - Initial creation
#include <caenv6533.h>
#include <jvme.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// This is the board address
// Set to -1 for board default
int board_addr = -1;

char *chstatusText[14] = {
  "ON",
  "RAMP UP",
  "RAMP DOWN",
  "OVER CURRENT",
  "OVER VOLTAGE",
  "UNDER VOLTAGE",
  "MAXV",
  "MAXI",
  "TRIP",
  "OVER POWER",
  "OVER TEMPERATURE",
  "DISABLED",
  "INTERLOCK",
  "UNCALIBRATED"
};

char *statusText[12] = {
  "CHANNEL 0 ALARM",
  "CHANNEL 1 ALARM",
  "CHANNEL 2 ALARM",
  "CHANNEL 3 ALARM",
  "CHANNEL 4 ALARM",
  "CHANNEL 5 ALARM",
  "RESERVED",
  "RESERVED",
  "BOARD POWER FAIL",
  "BOARD OVER POWER",
  "BOARD MAXV UNCALIBRATED",
  "BOARD MAXI UNCALIBRATED"
};


int processChannelSet(char *chan, char *type, char *val);
int processChannelGet(char *chan, char *type);
int processBoardSet(char *type, char *val);
int processBoardGet(char *type);
unsigned short int createMask(unsigned short int a, unsigned short int b);  // Creates mask of bits we want
unsigned short int getInt(unsigned short int val, unsigned short int a, unsigned short int b);  // Creates mask of bits we want
void print16Bit(unsigned short int val);

void printUsage() {
  printf("Usage:\n");
  printf("\tset board/channel# type value\n");
  printf("\tget board/channel# type\n");
}

int processCmnds(int argc, char** argv) {
  // Parse command line parameters
  if( argc < 2 ) { // No parameters - nothing to do, exit
    printf("Invalid number of parameters specified.\n");
    printUsage();
    return -1;
  }

  // The second parameter must be one of
  // set
  // get
  if( strcmp(argv[1], "set" ) == 0 ) {
    if(argc < 5 ) {
      printf("Error, invalid usage of set.\n");
      printUsage();
      return -1;
    }
    if( strcmp(argv[2],"board") == 0 ) {
      return processBoardSet(argv[3],argv[4]);
    } else {
      return processChannelSet(argv[2],argv[3],argv[4]);
    }
  } else if ( strcmp(argv[1], "get") == 0 ) {
    if(argc < 4 ) {
      printf("Error, invalid usage of get.\n");
      printUsage();
      return -1;
    }
    if( strcmp(argv[2],"board") == 0 ) {
      return processBoardGet(argv[3]);
    } else {
      return processChannelGet(argv[2],argv[3]);
    }
  } else { // No valid parameter
    printf("Error, unknown parameters.\n");
    printUsage();
    return -4;
  }

  return 0;
}


int processChannelSet(char *chan, char *type, char *val)
{
  short int channel = atoi(chan);
  if (channel > 5 || channel < 0 ) {
    printf("Error, invalid channel (%d) specified.\n",channel);
    return -2;
  }

  if( strcmp(type, "vset") == 0 ) { // caenv6533Set current
    float voltage = atof(val);
    printf("SET channel %d VSET = %f V\n", channel,
        voltage);
    caenv6533SetVSET(board_addr,channel,voltage);
  } else if ( strcmp(type, "iset" ) == 0 ) {
    float current = atof(val);
    printf("SET channel %d ISET = %f uA\n", channel,
        current);
    caenv6533SetISET(board_addr,channel,current);
  } else if ( strcmp(type, "pw" ) == 0 ) {
    short int turnOn = atoi(val);
    caenv6533SetPW(board_addr,channel,turnOn);
    printf("caenv6533Set channel %d PW = %s\n", channel,  turnOn == 0 ? "OFF" : "ON" );
  } else if ( strcmp(type, "svmax") == 0 ) {
    float voltage = atof(val);
    printf("SET channel %d SVMAX = %f V\n", channel,
        voltage);
    caenv6533SetSVMAX(board_addr,channel,voltage);
  }


  return 0;
}

int processChannelGet(char *chan, char *type)
{
  short int channel = atoi(chan);
  if (channel > 5 || channel < 0 ) {
    printf("Error, invalid channel (%d) specified.\n",channel);
    return -2;
  }

  if( strcmp(type,"vmon") == 0 ) { // Current voltage
    printf("Channel %d VMON = %f V\n",channel,caenv6533GetVMON(board_addr,channel));
  } else if ( strcmp(type, "chstatus") == 0 ) { // Get status

    unsigned short int status = caenv6533GetCHSTATUS(board_addr,channel);

    printf("Channel %d CHSTATUS bits set:\n|| ",channel );
    // Check if on or off
    int i;
    for( i = 0; i < 14; i++ ) {
      //if( ( status & (1 << i)) >> i ) {
      if( status & createMask(i,i) ) {
        printf("%s || ",chstatusText[i]);
      }
    }
    printf("\n");
  } else if ( strcmp(type, "vset" ) == 0 ) {
    printf("Channel %d VSET = %f V\n",channel,caenv6533GetVSET(board_addr,channel));
  } else if ( strcmp(type, "pw" ) == 0 ) {
    printf("Channel %d PW = %s\n",channel,  caenv6533GetPW(board_addr,channel) == 0 ? "OFF" : "ON" );
  } else if ( strcmp(type, "svmax" ) == 0 ) {
    printf("Channel %d SVMAX = %f V\n",channel,  caenv6533GetSVMAX(board_addr,channel) );
  } else if ( strcmp(type, "trip_time" ) == 0 ) {
    printf("Channel %d TRIP_TIME = %f s\n",channel,  caenv6533GetTRIP_TIME(board_addr,channel) );
  } else if ( strcmp(type, "imonh" ) == 0 ) {
    printf("Channel %d ImonH = %f uA\n",channel,  caenv6533GetImonH(board_addr,channel) );
  } else if ( strcmp(type, "imonl" ) == 0 ) {
    printf("Channel %d ImonL = %f uA\n",channel,  caenv6533GetImonL(board_addr,channel) );
  } else if ( strcmp(type, "iset" ) == 0 ) {
    printf("Channel %d ISET = %f uA\n",channel,  caenv6533GetISET(board_addr,channel) );
  } else if ( strcmp(type, "polarity" ) == 0 ) {
    printf("Channel %d POLARITY = %s\n",channel,  caenv6533GetPOLARITY(board_addr,channel) == 0 ? "NEGATIVE" : "POSITIVE" );
  } else if ( strcmp(type, "imon_range" ) == 0 ) {
    printf("Channel %d IMON_RANGE = %s\n",channel,  caenv6533GetIMON_RANGE(board_addr,channel) == 0 ? "HIGH" : "LOW" );
  } else {
    return -3;
  }

  return 0;
}

int processBoardSet(char *type, char *val)
{
  return 4;
}

int processBoardGet(char *type)
{
  if( strcmp(type, "vmax") == 0 ) {
    printf("Board VMAX = %f V.\n", caenv6533GetVMAX(board_addr)*1.0);
  } else if ( strcmp(type, "imax") == 0 ) {
    printf("Board IMAX = %d uA.\n", caenv6533GetIMAX(board_addr));
  } else if ( strcmp(type, "status") == 0 ) {
    short int status = caenv6533GetSTATUS(board_addr);

    printf("Board STATUS bits set:\n|| " );
    // Check if on or off
    int i;
    for( i = 0; i < 12; i++ ) {
      //if( ( status & (1 << i)) >> i ) {
      if( status & createMask(i,i) ) {
        printf("%s || ",statusText[i]);
      }
    }
    printf("\n");
  } else if ( strcmp(type, "fwrel") == 0 ) {
    short int fwrel = caenv6533GetFWREL(board_addr);
    short int minor = fwrel & 0xFF;
    short int major = fwrel >> 8;
    printf("Board Firwmare %d.%d\n",major, minor);
  } else if ( strcmp(type, "chnum") == 0 ) {
    printf("Board Configuration Number of Channels: %d\n ", caenv6533GetCHNUM(board_addr));
  } else if ( strcmp(type, "descr") == 0 ) {
    char desc[20];
    caenv6533GetDESCR(board_addr,desc);
    printf("Board Description: '%s'\n",desc);
  } else {
    return -3;
  }
  return 4;
}

unsigned short int createMask(unsigned short int a, unsigned short int b)
{
  unsigned short int r = 0;
  unsigned short int i = 0;
  for(i=a; i<=b; i++) {
    r |= 1 << i;
  }

  return r;
}


unsigned short int getInt(unsigned short int val,unsigned short int a, unsigned short int b)
{
  return val & createMask(a, b) >> (a+2);
}

void print16Bit(unsigned short int val)
{
  int i;
  printf("Printing bit info for value %hu %d\n",val,(short)val);
  for( i = 0; i < 16; i++ ) {
    printf("Bit %d is %d\n",i, ( val & (1 << i)) >> i );
  }
  printf("Done printing...\n");

}


int main( int argc, char** argv)
{
  // Set the board address to the default
  if( board_addr == -1) // Set to the default
    board_addr = caenv6533_default_board_addr;

  // Silence the VME functions
  vmeSetQuietFlag(1);

  // Open up the VME bus
  vmeOpenDefaultWindows();

  int result = processCmnds(argc, argv);

  // Close the VME bus before leaving
  vmeCloseDefaultWindows();

  // Return
  return result;
}


