/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics
 (C) Copyright 2012 Jordan Husney


 This library provides four key benefits:

   * Including `SD.h` automatically creates a global
     `SD` object which can be interacted with in a similar
     manner to other standard global objects like `Serial` and `Ethernet`.

   * Boilerplate initialisation code is contained in one method named 
     `begin` and no further objects need to be created in order to access
     the SD card.

   * Calls to `open` can supply a full path name including parent 
     directories which simplifies interacting with files in subdirectories.

   * Utility methods are provided to determine whether a file exists
     and to create a directory heirarchy.


  Note however that not all functionality provided by the underlying
  sdfatlib library is exposed.

 */

/*

  Implementation Notes

  In order to handle multi-directory path traversal, functionality that 
  requires this ability is implemented as callback functions.

  Individual methods call the `digiSD_walkPath` function which performs the actual
  directory traversal (swapping between two different directory/file handles
  along the way) and at each level calls the supplied callback function.

  Some types of functionality will take an action at each level (e.g. exists
  or make directory) which others will only take an action at the bottom
  level (e.g. open).

 */

#include "DigiSD.h"

// Used by `digiSD_getNextPathComponent`
#define MAX_COMPONENT_LEN 12 // What is max length?
#define PATH_COMPONENT_BUFFER_LEN MAX_COMPONENT_LEN+1

bool digiSD_getNextPathComponent(char *path, unsigned int *p_offset,
			  char *buffer)
{
  /*

    Parse individual path components from a path.

      e.g. after repeated calls '/foo/bar/baz' will be split
           into 'foo', 'bar', 'baz'.

    This is similar to `strtok()` but copies the component into the
    supplied buffer rather than modifying the original string.


    `buffer` needs to be PATH_COMPONENT_BUFFER_LEN in size.

    `p_offset` needs to point to an integer of the offset at
    which the previous path component finished.

    Returns `true` if more components remain.

    Returns `false` if this is the last component.
      (This means path ended with 'foo' or 'foo/'.)

   */

  // TODO: Have buffer local to this function, so we know it's the
  //       correct length?

  int bufferOffset = 0;

  int offset = *p_offset;

  // Skip root or other separator
  if (path[offset] == '/') {
    offset++;
  }
  
  // Copy the next next path segment
  while (bufferOffset < MAX_COMPONENT_LEN
	 && (path[offset] != '/')
	 && (path[offset] != '\0')) {
    buffer[bufferOffset++] = path[offset++];
  }

  buffer[bufferOffset] = '\0';

  // Skip trailing separator so we can determine if this
  // is the last component in the path or not.
  if (path[offset] == '/') {
    offset++;
  }

  *p_offset = offset;

  return (path[offset] != '\0');
}



boolean digiSD_walkPath(char *filepath, SdFile& parentDir,
		 boolean (*callback)(SdFile& parentDir,
				     char *filePathComponent,
				     boolean isLastComponent,
				     void *object),
		 void *object = NULL)
{
  /*
     
     When given a file path (and parent directory--normally root),
     this function traverses the directories in the path and at each
     level calls the supplied callback function while also providing
     the supplied object for context if required.

       e.g. given the path '/foo/bar/baz'
            the callback would be called at the equivalent of
	    '/foo', '/foo/bar' and '/foo/bar/baz'.

     The implementation swaps between two different directory/file
     handles as it traverses the directories and does not use recursion
     in an attempt to use memory efficiently.

     If a callback wishes to stop the directory traversal it should
     return false--in this case the function will stop the traversal,
     tidy up and return false.

     If a directory path doesn't exist at some point this function will
     also return false and not subsequently call the callback.

     If a directory path specified is complete, valid and the callback
     did not indicate the traversal should be interrupted then this
     function will return true.

   */


  SdFile subfile1;
  SdFile subfile2;

  char buffer[PATH_COMPONENT_BUFFER_LEN]; 

  unsigned int offset = 0;

  SdFile *p_parent;
  SdFile *p_child;

  SdFile *p_tmp_sdfile;  
  
  p_child = &subfile1;
  
  p_parent = &parentDir;

  while (true) {

    boolean moreComponents = digiSD_getNextPathComponent(filepath, &offset, buffer);

    boolean shouldContinue = callback((*p_parent), buffer, !moreComponents, object);

    if (!shouldContinue) {
      // TODO: Don't repeat this code?
      // If it's one we've created then we
      // don't need the parent handle anymore.
      if (p_parent != &parentDir) {
        (*p_parent).close();
      }
      return false;
    }
    
    if (!moreComponents) {
      break;
    }
    
    boolean exists = (*p_child).open(*p_parent, buffer, O_RDONLY);

    // If it's one we've created then we
    // don't need the parent handle anymore.
    if (p_parent != &parentDir) {
      (*p_parent).close();
    }
    
    // Handle case when it doesn't exist and we can't continue...
    if (exists) {
      // We alternate between two file handles as we go down
      // the path.
      if (p_parent == &parentDir) {
        p_parent = &subfile2;
      }

      p_tmp_sdfile = p_parent;
      p_parent = p_child;
      p_child = p_tmp_sdfile;
    } else {
      return false;
    }
  }
  
  if (p_parent != &parentDir) {
    (*p_parent).close(); // TODO: Return/ handle different?
  }

  return true;
}



/*

   The callbacks used to implement various functionality follow.

   Each callback is supplied with a parent directory handle,
   character string with the name of the current file path component,
   a flag indicating if this component is the last in the path and
   a pointer to an arbitrary object used for context.

 */

boolean digiSD_callback_pathExists(SdFile& parentDir, char *filePathComponent, 
			    boolean isLastComponent, void *object)
{
  /*

    Callback used to determine if a file/directory exists in parent
    directory.

    Returns true if file path exists.

  */
  SdFile child;

  boolean exists = child.open(parentDir, filePathComponent, O_RDONLY);
  
  if (exists) {
     child.close(); 
  }
  
  return exists;
}

boolean digiSD_callback_makeDirPath(SdFile& parentDir, char *filePathComponent, 
			     boolean isLastComponent, void *object)
{
  /*

    Callback used to create a directory in the parent directory if
    it does not already exist.

    Returns true if a directory was created or it already existed.

  */
  boolean result = false;
  SdFile child;
  
  result = digiSD_callback_pathExists(parentDir, filePathComponent, isLastComponent, object);
  if (!result) {
    result = child.makeDir(parentDir, filePathComponent);
  } 
  
  return result;
}

boolean digiSD_callback_remove(SdFile& parentDir, char *filePathComponent, 
			boolean isLastComponent, void *object)
{
  if (isLastComponent) {
    return SdFile::remove(parentDir, filePathComponent);
  }
  return true;
}

boolean digiSD_callback_rmdir(SdFile& parentDir, char *filePathComponent, 
			boolean isLastComponent, void *object)
{
  if (isLastComponent) {
    SdFile f;
    if (!f.open(parentDir, filePathComponent, O_READ)) return false;
    return f.rmDir();
  }
  return true;
}



/* Implementation of class used to create `SDCard` object. */



boolean DigiSDClass::begin(uint8_t csPin)
{
  /*

    Performs the initialisation required by the sdfatlib library.

    Return true if initialization succeeds, false otherwise.

   */
  return card.init(SPI_FULL_SPEED, csPin) &&
         volume.init(card) &&
         root.openRoot(volume);
}



// this little helper is used to traverse paths
SdFile DigiSDClass::getParentDir(const char *filepath, int *index)
{
  // get parent directory
  SdFile d1 = root; // start with the mostparent, root!
  SdFile d2;

  // we'll use the pointers to swap between the two objects
  SdFile *parent = &d1;
  SdFile *subdir = &d2;
  
  const char *origpath = filepath;

  while (strchr(filepath, '/')) {

    // get rid of leading /'s
    if (filepath[0] == '/') {
      filepath++;
      continue;
    }
    
    if (! strchr(filepath, '/')) {
      // it was in the root directory, so leave now
      break;
    }

    // extract just the name of the next subdirectory
    uint8_t idx = strchr(filepath, '/') - filepath;
    if (idx > 12)
      idx = 12;    // dont let them specify long names
    char subdirname[13];
    strncpy(subdirname, filepath, idx);
    subdirname[idx] = 0;

    // close the subdir (we reuse them) if open
    subdir->close();
    if (! subdir->open(parent, subdirname, O_READ)) {
      // failed to open one of the subdirectories
      return SdFile();
    }
    // move forward to the next subdirectory
    filepath += idx;

    // we reuse the objects, close it.
    parent->close();

    // swap the pointers
    SdFile *t = parent;
    parent = subdir;
    subdir = t;
  }

  *index = (int)(filepath - origpath);
  // parent is now the parent diretory of the file!
  return *parent;
}


SdFile DigiSDClass::open(const char *filepath, uint8_t mode)
{
  /*

     Open the supplied file path for reading or writing.

     The file content can be accessed via the `file` property of
     the `DigiSDClass` object--this property is currently
     a standard `SdFile` object from `sdfatlib`.

     Defaults to read only.

     If `write` is true, default action (when `append` is true) is to
     append data to the end of the file.

     If `append` is false then the file will be truncated first.

     If the file does not exist and it is opened for writing the file
     will be created.

     An attempt to open a file for reading that does not exist is an
     error.

   */

  int pathidx;

  // do the interative search
  SdFile parentdir = getParentDir(filepath, &pathidx);
  // no more subdirs!

  filepath += pathidx;

  if (! filepath[0]) {
    // it was the directory itself!
    return parentdir;
  }

  // Open the file itself
  SdFile file;

  // failed to open a subdir!
  if (!parentdir.isOpen())
    return file;

  // there is a special case for the Root directory since its a static dir
  if (parentdir.isRoot()) {
    if ( ! file.open(DigiSD.root, filepath, mode)) {
      // failed to open the file :(
      return file;
    }
    // dont close the root!
  } else {
    if ( ! file.open(parentdir, filepath, mode)) {
      return file;
    }
    // close the parent
    parentdir.close();
  }

  if (mode & (O_APPEND | O_WRITE)) 
    file.seekSet(file.fileSize());
  return file;
}


boolean DigiSDClass::exists(char *filepath)
{
  /*

     Returns true if the supplied file path exists.

   */
  return digiSD_walkPath(filepath, root, digiSD_callback_pathExists);
}

//boolean DigiSDClass::exists(char *filepath, SdFile& parentDir) {
//  /*
//
//     Returns true if the supplied file path rooted at `parentDir`
//     exists.
//
//   */
//  return digiSD_walkPath(filepath, parentDir, digiSD_callback_pathExists);
//}


boolean DigiSDClass::mkdir(char *filepath)
{
  /*
  
    Makes a single directory or a heirarchy of directories.

    A rough equivalent to `mkdir -p`.
  
   */
  return digiSD_walkPath(filepath, root, digiSD_callback_makeDirPath);
}

boolean DigiSDClass::rmdir(char *filepath) 
{
  /*
  
    Makes a single directory or a heirarchy of directories.

    A rough equivalent to `mkdir -p`.
  
   */
  return digiSD_walkPath(filepath, root, digiSD_callback_rmdir);
}

boolean DigiSDClass::remove(char *filepath)
{
  return digiSD_walkPath(filepath, root, digiSD_callback_remove);
}

SdFile DigiSDClass::openNextFile(SdFile *curDir, char *fileName, uint8_t mode)
{
  dir_t p;
  SdFile f;

  //Serial.print("\t\treading dir...");
  while (curDir->readDir(&p) > 0) {

    // done if past last used entry
    if (p.name[0] == DIR_NAME_FREE) {
      //Serial.println("end");
      return f;
    }

    // skip deleted entry and entries for . and  ..
    if (p.name[0] == DIR_NAME_DELETED || p.name[0] == '.') {
      //Serial.println("dots");
      continue;
    }

    // only list subdirectories and files
    if (!DIR_IS_FILE_OR_SUBDIR(&p)) {
      //Serial.println("notafile");
      continue;
    }

    curDir->dirName(p, fileName);
    //Serial.print("try to open file ");
    //Serial.println(fileName);

    if (f.open(curDir, fileName, mode)) {
      //Serial.println("OK!");
      return f;    
    } else {
      //Serial.println("ugh");
      return f;
    }
  }

  //Serial.println("nothing");
  return f;
}

void DigiSDClass::rewindDirectory(SdFile *curDir)
{  
  if (curDir != NULL && curDir->isDir())
    curDir->rewind();
}

DigiSDClass DigiSD;
