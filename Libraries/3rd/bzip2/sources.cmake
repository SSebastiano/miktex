## CMakeLists.txt					-*- CMake -*-
##
## Copyright (C) 2006-2016 Christian Schenk
## 
## This file is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation; either version 2, or (at your
## option) any later version.
## 
## This file is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this file; if not, write to the Free Software
## Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
## USA.

set(bzip2_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/source/blocksort.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/bzlib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/bzlib.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/bzlib_private.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/compress.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/crctable.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/decompress.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/huffman.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/randtable.c

  ${CMAKE_CURRENT_SOURCE_DIR}/libbz2-version.h
)
