/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.x                          */
/*                 Copyright (C)2015-2016 WWIV Software Services          */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/
#include "core/os.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>
#include <VersionHelpers.h>

#endif  // _WIN32

#include "core/strings.h"
#include "core/file.h"

#pragma comment (lib, "DbgHelp.lib")

using std::numeric_limits;
using std::chrono::milliseconds;
using std::string;
using std::stringstream;
using namespace wwiv::strings;

namespace wwiv {
namespace os {

void sleep_for(milliseconds d) {
  int64_t count = d.count();
  if (count > numeric_limits<uint32_t>::max()) {
    count = numeric_limits<uint32_t>::max();
  }
  ::Sleep(static_cast<uint32_t>(count));
}

void sound(uint32_t frequency, std::chrono::milliseconds d) {
  ::Beep(frequency, static_cast<uint32_t>(d.count()));
}

std::string os_version_string() {
  bool server = IsWindowsServer();
  if (IsWindows10OrGreater()) {
    // TODO(rushfan): Sort out Windows 10 SDK issues on build server.
    return server ? "Windows 2016 Server" : "Windows 10";
  }
  if (IsWindows8Point1OrGreater()) {
    return server ? "Windows Server 2012R2" : "Windows 8.1";
  }
  if (IsWindows8OrGreater()) {
    return server ? "Windows Server 2012" : "Windows 8.0";
  }
  if (IsWindows7SP1OrGreater()) {
    return server ? "Windows Server 2008" : "Windows 7 SP1";
  }
  if (IsWindows7OrGreater()) {
    return server ? "Windows Server 2008" : "Windows 7";
  }
  return "WIN32";
}

bool set_environment_variable(const std::string& variable_name, const std::string value) {
  return ::SetEnvironmentVariable(variable_name.c_str(), value.c_str()) ? true : false;
}

std::string environment_variable(const std::string& variable_name) {
  // See http://techunravel.blogspot.com/2011/08/win32-env-variable-pitfall-of.html
  // Use Win32 functions to get since we do to set...
  char buffer[4096];
  DWORD size = GetEnvironmentVariable(variable_name.c_str(), buffer, 4096);
  if (size == 0) {
    // error or does not exits.
    return "";
  }
  return string(buffer);
}

string stacktrace() {
  HANDLE process = GetCurrentProcess();
  SymInitialize(process, NULL, TRUE);
  void* stack[100];
  uint16_t frames = CaptureStackBackTrace(0, 100, stack, NULL);
  SYMBOL_INFO* symbol = (SYMBOL_INFO *) calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
  symbol->MaxNameLen = 255;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

  stringstream out;
  // start at one to skip this current frame.
  for(std::size_t i = 1; i < frames; i++) {
    if (SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol)) {
      out << frames - i - 1 << ": " << symbol->Name << " = " << std::hex << symbol->Address;
    }
    IMAGEHLP_LINE64 line;
    DWORD displacement;
    if (SymGetLineFromAddr64(process, (DWORD64)stack[i], &displacement, &line)) {
      out << " (" << line.FileName << ": " << std::dec << line.LineNumber << ") ";
    }
    out << std::endl;
  }
  free(symbol);
  return out.str();
}


}  // namespace os
}  // namespace wwiv
