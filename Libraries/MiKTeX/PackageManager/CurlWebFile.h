/* CurlWebFile.h:                                       -*- C++ -*-

   Copyright (C) 2001-2018 Christian Schenk

   This file is part of MiKTeX Package Manager.

   MiKTeX Package Manager is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2, or
   (at your option) any later version.

   MiKTeX Package Manager is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with MiKTeX Package Manager; if not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA. */

#if defined(_MSC_VER)
#  pragma once
#endif

#if defined(HAVE_LIBCURL) && !defined(DB88FC1DC9B0497C965F3CFB916FA93A)
#define DB88FC1DC9B0497C965F3CFB916FA93A

#include "WebFile.h"
#include "CurlWebSession.h"

BEGIN_INTERNAL_NAMESPACE;

class CurlWebFile :
  public WebFile
{
public:
  CurlWebFile(std::shared_ptr<CurlWebSession> webSession, const std::string& url, const std::unordered_map<std::string, std::string>& formData);

public:
  ~CurlWebFile() override;

public:
  size_t Read(void* data, size_t n) override;

public:
  void Close() override;

private:
  static size_t WriteCallback(char* data, size_t elemSize, size_t numElements, void* pv);

private:
  void TakeData(const void* data, size_t size);

private:
  void Initialize();

private:
  bool initialized = false;

private:
  std::shared_ptr<CurlWebSession> webSession;

private:
  std::string url;

private:
  std::string urlEncodedpostFields;

private:
  std::vector<char> buffer;

private:
  std::unique_ptr<MiKTeX::Trace::TraceStream> trace_mpm;
};

END_INTERNAL_NAMESPACE;

#endif // libCURL
