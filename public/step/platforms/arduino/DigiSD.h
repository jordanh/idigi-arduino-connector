/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#ifndef __DIGI_SD_H__
#define __DIGI_SD_H__

#include <Arduino.h>

#include <SD.h>
#include <utility/SdFat.h>
#include <utility/SdFatUtil.h>

#define DIGISD_MAX_FILENAME_LEN   13

class DigiSDClass {

protected:
  // These are required for initialisation and use of sdfatlib
  Sd2Card card;
  SdVolume volume;
  SdFile root;
  
  // my quick&dirty iterator, should be replaced
  SdFile getParentDir(const char *filepath, int *indx);
public:
  // This needs to be called to set up the connection to the SD card
  // before other methods are used.
  boolean begin(uint8_t csPin = SD_CHIP_SELECT_PIN);
  
  // Open the specified file/directory with the supplied mode (e.g. read or
  // write, etc). Returns a File object for interacting with the file.
  // Note that currently only one file can be open at a time.
  SdFile open(const char *filename, uint8_t mode = FILE_READ);

  // Methods to determine if the requested file path exists.
  boolean exists(char *filepath);

  // Create the requested directory heirarchy--if intermediate directories
  // do not exist they will be created.
  boolean mkdir(char *filepath);
  
  // Delete the file.
  boolean remove(char *filepath);
  
  boolean rmdir(char *filepath);

  SdFile openNextFile(SdFile *curDir, char *fileName, uint8_t mode = O_RDONLY);
  void rewindDirectory(SdFile *curDir);

private:
  // This is used to determine the mode used to open a file
  // it's here because it's the easiest place to pass the 
  // information through the directory walking function. But
  // it's probably not the best place for it.
  // It shouldn't be set directly--it is set via the parameters to `open`.
  friend boolean callback_openPath(SdFile&, char *, boolean, void *); 
};

extern DigiSDClass DigiSD;

#endif
