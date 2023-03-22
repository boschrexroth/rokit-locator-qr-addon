//==================================================================================
//  *  C O P Y R I G H T
//  *
//----------------------------------------------------------------------------------
//  *  Copyright (c) 2022 by the Shanghai Bosch Rexroth Hydraulics & Automation
// Ltd.. All rights reserved.   *   *  This file is property of the Shanghai
// Bosch Rexroth Hydraulics & Automation Ltd..   *  Any unauthorized copy or use
// or distribution is an offensive act against   *  international law and may be
// prosecuted under federal law.   *  Its content is company confidential.   *
//==================================================================================
//  *  I N I T I A L   A U T H O R   I D E N T I T Y
//  *
//----------------------------------------------------------------------------------
//  *  Created on: 15.12.2022
//  *  Author    : Xu Zihang
//  *
//==================================================================================

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
