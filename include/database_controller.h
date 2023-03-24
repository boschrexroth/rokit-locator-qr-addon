//
// MIT License

// Copyright(c) 2022 Shanghai Bosch Rexroth Hydraulics &Automation Ltd.

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish , distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#ifndef MAINLOGICHEADER
#define MAINLOGICHEADER
#include "main_logic.h"
#endif
static int retrieveCallBack(void *, int32_t, char **, char **);

class DataBaseController {
 public:
  DataBaseController() { dataBasePtr = std::make_shared<sqlite3 *>(); }
  virtual void openDb() = 0;
  virtual void createTable() = 0;
  virtual void addContent(std::shared_ptr<DM>) = 0;
  std::shared_ptr<sqlite3 *> getDbPtr();
  void closeDb();
  virtual ~DataBaseController() = default;

 protected:
  std::shared_ptr<sqlite3 *> dataBasePtr;
};

class UserDbController : public DataBaseController {
 public:
  UserDbController() = default;
  virtual void openDb() final;
  virtual void createTable() final;

  bool retrieveUsrDb(std::shared_ptr<DM>);
  virtual void addContent(std::shared_ptr<DM>) final;
  void updateDataBase(std::shared_ptr<DM>);
};

class SelfDbController : public DataBaseController {
 public:
  SelfDbController() = default;
  virtual void openDb() final;
  virtual void createTable() final;
  virtual void addContent(std::shared_ptr<DM>) final;
  void createTrigger(std::shared_ptr<Config>);
  void fitPose(const uint16_t, std::shared_ptr<DM>);
  static int32_t fitCallBack(void *, int32_t, char **, char **);
};

class BackupDbController {
 public:
  BackupDbController() { dataBasePtr = std::make_shared<sqlite3 *>(); }
  void openDb();
  void backupUsrDb(std::shared_ptr<sqlite3 *>);
  bool checkDate(std::shared_ptr<Config>);
  void closeDb();

 private:
  std::shared_ptr<sqlite3 *> dataBasePtr;
};
