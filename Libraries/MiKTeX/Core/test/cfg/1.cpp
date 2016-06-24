/* 1.cpp:

   Copyright (C) 1996-2016 Christian Schenk

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

#include <miktex/Core/Test>

#include <memory>
#include <string>

#include <miktex/Core/Cfg>

using namespace MiKTeX::Core;
using namespace MiKTeX::Test;
using namespace std;

BEGIN_TEST_SCRIPT("cfg-1");

BEGIN_TEST_FUNCTION(1);
{
  shared_ptr<Cfg> cfg;
  TESTX(cfg = Cfg::Create());
  TESTX(cfg->PutValue("ABC", "xYz", "abrakadabraa"));
  TESTX(cfg->PutValue("AbC", "xyZ", "abrakadabra"));
  TEST(cfg->GetValue("abc", "xyz") == "abrakadabra");
  TESTX(cfg->Write("test.ini"));
}
END_TEST_FUNCTION();

BEGIN_TEST_FUNCTION(2);
{
  shared_ptr<Cfg> cfg;
  TESTX(cfg = Cfg::Create());
  TESTX(cfg->Read("test.ini"));
  TEST(cfg->GetValue("abc", "xyz") == "abrakadabra");
  TESTX(cfg->DeleteValue("abc", "xyz"));
  string value;
  TEST(!cfg->TryGetValue("abc", "xyz", value));
}
END_TEST_FUNCTION();

BEGIN_TEST_PROGRAM();
{
  CALL_TEST_FUNCTION(1);
  CALL_TEST_FUNCTION(2);
}
END_TEST_PROGRAM();

END_TEST_SCRIPT();

RUN_TEST_SCRIPT();
