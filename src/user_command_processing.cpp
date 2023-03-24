//
// MIT License
//
// Copyright(c) 2022 Shanghai Bosch Rexroth Hydraulics &Automation Ltd.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish , distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "database_controller.h"
#include "user_command_processing.h"

extern std::unique_ptr<LOG_T> LOG;

void sigHandler(int32_t sig) {
  curl_global_cleanup();
  LOG->writeLog("run end--- ");

  sqlite3 **tmpDistructor = (sqlite3 **)sqlite3_malloc(sizeof(sqlite3 *));
  int32_t ret;
  ret = sqlite3_open("../DmList.db", tmpDistructor);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't open customer destructor database pointer: %s\n",
            sqlite3_errmsg(*tmpDistructor));
    LOG->writeLog("Can't open customer destructor database pointer ");
    exit(0);
  } else {
    sqlite3_close(*tmpDistructor);
  }

  ret = sqlite3_open("../SelfDmList.db", tmpDistructor);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't open private destructor database pointer: %s\n",
            sqlite3_errmsg(*tmpDistructor));
    LOG->writeLog("Can't open private destructor database pointer ");
    exit(0);
  } else {
    sqlite3_close(*tmpDistructor);
  }

  std::cout << "add_on End!\n";
  exit(0);
}
