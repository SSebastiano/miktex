/* fndbmem.h: fndb file format                          -*- C++ -*-

   Copyright (C) 1996-2018 Christian Schenk

   This file is part of the MiKTeX Core Library.

   The MiKTeX Core Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2, or
   (at your option) any later version.

   The MiKTeX Core Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the MiKTeX Core Library; if not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA. */

#pragma once

#if !defined(A0FEBF8A9A7A419BB1230D6A7C07C5FA)
#define A0FEBF8A9A7A419BB1230D6A7C07C5FA

#include "miktex/Core/Debug.h"

BEGIN_INTERNAL_NAMESPACE;

#define MIKTEX_FNDB_VERSION 4

#if MIKTEX_FNDB_VERSION == 4
const size_t FNDB_GRAN = 1024 * 1024;
const size_t FNDB_EXTRA = 5 * FNDB_GRAN;
#endif

typedef uint32_t FndbWord;
typedef FndbWord FndbByteOffset;

struct FileNameDatabaseHeader
{
  static const FndbWord Signature = 0x42444e46; // 'FNDB' (the x86 way)
  static const FndbWord Version = MIKTEX_FNDB_VERSION;

  class FndbFlags
  {
  public:
    enum {
      Frozen = 1,               // database cannot be modified
      FileNameInfo = 2,         // extra file name info
    };
  };

  // signature of fndb file
  FndbWord signature;

  // format version number
  FndbWord version;

  // flag word (bits see above)
  FndbWord flags;

  // pointer to path name
  FndbByteOffset foPath;

#if MIKTEX_FNDB_VERSION == 5
  // pointer to first record
  FndbByteOffset foTable;
#endif

#if MIKTEX_FNDB_VERSION == 4
  // pointer to root directory
  FndbByteOffset foTopDir;
#endif

  // number of directories
  FndbWord numDirs;

  // number of files (records)
  FndbWord numFiles;

  // time of last refresh
  FndbWord timeStamp;

  // max directory depth
  FndbWord depth;

  // size (in bytes) of fndb; includes header size
  FndbWord size;

  void Init()
  {
    MIKTEX_ASSERT(sizeof(*this) % 8 == 0);
    signature = Signature;
    version = Version;
    flags = 0;
    size = sizeof(*this);
  }
};

struct FileNameDatabaseRecord
{
  FndbByteOffset foFileName;
  FndbByteOffset foDirectory;
  FndbByteOffset foInfo;
  FndbByteOffset reserved;
};

#if MIKTEX_FNDB_VERSION == 4
struct FileNameDatabaseDirectory
{
  // pointer to directory name
  FndbByteOffset foName;

  // pointer to parent directory
  FndbByteOffset foParent;

  // number of files in this directory
  FndbWord numFiles;

  // number of sub-directories
  FndbWord numSubDirs;

  // pointer to directory extension
  FndbByteOffset foExtension;

  // capacity of pointer table
  FndbWord capacity;

  // table of pointers to
  //     numFiles file names
  //   + numSubDirs sub directory names
  //   + numSubDirs sub directories
  //   + numFiles file infos (if FNDB_FLAG_FILE_NAME_INFO is set)
  FndbByteOffset table[1];

  FndbByteOffset GetFileName(FndbWord idx) const
  {
    MIKTEX_ASSERT(idx < numFiles);
    return table[idx];
  }

  FndbByteOffset GetSubDir(FndbWord idx) const
  {
    MIKTEX_ASSERT(idx < numSubDirs);
    return table[numFiles + numSubDirs + idx];
  }

  FndbByteOffset GetFileNameInfo(FndbWord idx) const
  {
    MIKTEX_ASSERT(idx < numFiles);
    return table[numFiles + (2 * numSubDirs) + idx];
  }

  void Init()
  {
    foExtension = 0;
  }
};
#endif

END_INTERNAL_NAMESPACE;

#endif
