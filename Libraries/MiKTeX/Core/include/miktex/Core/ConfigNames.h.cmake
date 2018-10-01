/* miktex/Core/ConfigNames.h:                           -*- C++ -*-

   Copyright (C) 2017-2018 Christian Schenk

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

#if !defined(D67BA8FF3D0844B281CB96F2D2FE7ED7)
#define D67BA8FF3D0844B281CB96F2D2FE7ED7

constexpr auto MIKTEX_CONFIG_SECTION_CORE = "${MIKTEX_CONFIG_SECTION_CORE}";
constexpr auto MIKTEX_CONFIG_SECTION_CORE_FILETYPES = "${MIKTEX_CONFIG_SECTION_CORE_FILETYPES}";
constexpr auto MIKTEX_CONFIG_SECTION_MPM = "${MIKTEX_CONFIG_SECTION_MPM}";
constexpr auto MIKTEX_CONFIG_SECTION_TEXANDFRIENDS = "${MIKTEX_CONFIG_SECTION_TEXANDFRIENDS}";

constexpr auto MIKTEX_CONFIG_VALUE_ALLOWEDSHELLCOMMANDS = "${MIKTEX_CONFIG_VALUE_ALLOWEDSHELLCOMMANDS}";
constexpr auto MIKTEX_CONFIG_VALUE_ALLOWUNSAFEINPUTFILES = "${MIKTEX_CONFIG_VALUE_ALLOWUNSAFEINPUTFILES}";
constexpr auto MIKTEX_CONFIG_VALUE_ALLOWUNSAFEOUTPUTFILES = "${MIKTEX_CONFIG_VALUE_ALLOWUNSAFEOUTPUTFILES}";
constexpr auto MIKTEX_CONFIG_VALUE_ALTEXTENSIONS = "${MIKTEX_CONFIG_VALUE_ALTEXTENSIONS}";
constexpr auto MIKTEX_CONFIG_VALUE_AUTOADMIN = "${MIKTEX_CONFIG_VALUE_AUTOADMIN}";
constexpr auto MIKTEX_CONFIG_VALUE_AUTOINSTALL = "${MIKTEX_CONFIG_VALUE_AUTOINSTALL}";
constexpr auto MIKTEX_CONFIG_VALUE_CREATEAUXDIRECTORY = "${MIKTEX_CONFIG_VALUE_CREATEAUXDIRECTORY}";
constexpr auto MIKTEX_CONFIG_VALUE_CREATEOUTPUTDIRECTORY = "${MIKTEX_CONFIG_VALUE_CREATEOUTPUTDIRECTORY}";
constexpr auto MIKTEX_CONFIG_VALUE_CSTYLEERRORS = "${MIKTEX_CONFIG_VALUE_CSTYLEERRORS}";
constexpr auto MIKTEX_CONFIG_VALUE_ENVVARS = "${MIKTEX_CONFIG_VALUE_ENVVARS}";
constexpr auto MIKTEX_CONFIG_VALUE_EXTENSIONS = "${MIKTEX_CONFIG_VALUE_EXTENSIONS}";
constexpr auto MIKTEX_CONFIG_VALUE_PATHS = "${MIKTEX_CONFIG_VALUE_PATHS}";
constexpr auto MIKTEX_CONFIG_VALUE_SHELLCOMMANDMODE = "${MIKTEX_CONFIG_VALUE_SHELLCOMMANDMODE}";

#endif
