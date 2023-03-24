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
extern std::shared_ptr<LOG_T> LOG;
void DataBaseController::closeDb() { sqlite3_close(*dataBasePtr); }

std::shared_ptr<sqlite3 *> DataBaseController::getDbPtr() {
  return dataBasePtr;
}

/* user database*/
void UserDbController::openDb() {
  int32_t ret;
  ret = sqlite3_open("../DmList.db", &(*dataBasePtr));
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't open user database: %s\n",
            sqlite3_errmsg(*dataBasePtr));
    LOG->writeLog("Can't open user database: ");
    exit(0);
  } else {
    fprintf(stderr, "Opened user database successfully\n");
    LOG->writeLog("Opened user database successfully");
  }
}

void UserDbController::createTable() {
  int32_t ret;
  char *sqlErrMsg;
  std::string strSql;
  strSql += "create table if not exists DmList(";
  strSql += "DM_id INTEGER not null,";
  strSql += "DM_x real not null,";
  strSql += "DM_y real not null,";
  strSql += "DM_yaw real not null,";
  strSql += "DM_cnt INTEGER not null,";
  strSql += "sloppy_cnt INTEGER not null,";
  strSql += "recommend BOOLEAN not null,";
  strSql += "enable BOOLEAN not null,";
  strSql += "primary key(DM_id))";
  ret = sqlite3_exec(*dataBasePtr, strSql.c_str(), 0, 0, &sqlErrMsg);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't create user table: %s\n",
            sqlite3_errmsg(*dataBasePtr));
    LOG->writeLog("Can't create user table: ");
    sqlite3_free(sqlErrMsg);
    exit(0);
  } else {
    fprintf(stderr, "Create user table successfully\n");
    LOG->writeLog("Create user table successfully");
    sqlite3_free(sqlErrMsg);
  }
}

bool whetherCb = false;

int retrieveCallBack(void *arg, int32_t col, char **values, char **names) {
  DmInfo *cbDmInfoPtr = (DmInfo *)arg;
  whetherCb = true;
  cbDmInfoPtr->id = atoi(values[0]);
  cbDmInfoPtr->pose.x = atof(values[1]);
  cbDmInfoPtr->pose.y = atof(values[2]);
  cbDmInfoPtr->pose.yaw = atof(values[3]);
  cbDmInfoPtr->cnt = atoi(values[4]);
  cbDmInfoPtr->sloppyCnt = atoi(values[5]);
  cbDmInfoPtr->recommend = (bool)atoi(values[6]);
  cbDmInfoPtr->enable = (bool)atoi(values[7]);
  return 0;
}

bool UserDbController::retrieveUsrDb(std::shared_ptr<DM> dmPtr) {
  int32_t ret;
  char *sqlErrMsg;
  std::string strSql;
  DmInfo localDmInfo;
  std::shared_ptr<DmInfo> localDmInfoPtr = std::make_shared<DmInfo>();
  dmPtr->getInfo(localDmInfoPtr);
  strSql += "select * from DmList";
  strSql += " where DM_id=";
  strSql += std::to_string(localDmInfoPtr->id);
  strSql += " limit 1";
  ret = sqlite3_exec(*dataBasePtr, strSql.c_str(), retrieveCallBack,
                     &localDmInfo, &sqlErrMsg);
  sqlite3_free(sqlErrMsg);
  if (ret != SQLITE_OK) {
    whetherCb = false;
    return false;
  } else {
    if (localDmInfo.id == 0) {
      whetherCb = false;
      return false;
    }
    if (!whetherCb) return false;
    (*localDmInfoPtr) = localDmInfo;
    dmPtr->setInfo(localDmInfoPtr);
    whetherCb = false;
    return true;
  }
}

void UserDbController::addContent(std::shared_ptr<DM> dmPtr) {
  int32_t ret;
  char *sqlErrMsg;
  std::string strSql;
  std::shared_ptr<DmInfo> localDmInfoPtr = std::make_shared<DmInfo>();
  dmPtr->getInfo(localDmInfoPtr);
  strSql += "insert into DmList values (";
  strSql += std::to_string(localDmInfoPtr->id) + ",";
  strSql += std::to_string(localDmInfoPtr->pose.x) + ",";
  strSql += std::to_string(localDmInfoPtr->pose.y) + ",";
  strSql += std::to_string(localDmInfoPtr->pose.yaw) + ",";
  strSql += std::to_string(localDmInfoPtr->cnt) + ",";
  strSql += std::to_string(localDmInfoPtr->sloppyCnt) + ",";
  strSql += std::to_string(localDmInfoPtr->recommend) + ",";
  strSql += std::to_string(localDmInfoPtr->enable) + "";
  strSql += ");";
  ret = sqlite3_exec(*dataBasePtr, strSql.c_str(), 0, 0, &sqlErrMsg);
  if (ret != SQLITE_OK) {
    LOG->writeLog("add item user database error");
  }
  sqlite3_free(sqlErrMsg);
}

void UserDbController::updateDataBase(std::shared_ptr<DM> dmPtr) {
  int32_t ret;
  char *sqlErrMsg;
  std::string strSql;
  std::shared_ptr<DmInfo> localDmInfoPtr = std::make_shared<DmInfo>();
  dmPtr->getInfo(localDmInfoPtr);
  strSql += "update DmList set DM_x=" + std::to_string(localDmInfoPtr->pose.x);
  strSql += ", DM_y=" + std::to_string(localDmInfoPtr->pose.y);
  strSql += ", DM_yaw=" + std::to_string(localDmInfoPtr->pose.yaw);
  strSql += ",DM_cnt=" + std::to_string(localDmInfoPtr->cnt);
  strSql += ",sloppy_cnt=" + std::to_string(localDmInfoPtr->sloppyCnt);
  strSql += ",recommend=" + std::to_string(localDmInfoPtr->recommend);
  strSql += ",enable=" + std::to_string(localDmInfoPtr->enable);
  strSql += " where DM_id=" + std::to_string(localDmInfoPtr->id);
  ret = sqlite3_exec(*dataBasePtr, strSql.c_str(), 0, 0, &sqlErrMsg);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't update table: %s\n", sqlite3_errmsg(*dataBasePtr));
    LOG->writeLog("Can't update table: ");
    sqlite3_free(sqlErrMsg);
  } else {
    fprintf(stderr, "Update table successfully\n");
    LOG->writeLog("Update table successfully");
    sqlite3_free(sqlErrMsg);
  }
}
/* user database*/

/* self database*/
void SelfDbController::openDb() {
  int32_t ret;
  ret = sqlite3_open("../SelfDmList.db", &(*dataBasePtr));
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't open self database: %s\n",
            sqlite3_errmsg(*dataBasePtr));
    LOG->writeLog("Can't open self database:");
    exit(0);
  } else {
    fprintf(stderr, "Opened self database successfully\n");
    LOG->writeLog("Opened self database successfully");
  }
}

void SelfDbController::createTable() {
  int32_t ret;
  char *sqlErrMsg;
  std::string strSql;
  strSql += "create table if not exists DmList(";
  strSql += "DM_id INTEGER not null,";
  strSql += "DM_x real not null,";
  strSql += "DM_y real not null,";
  strSql += "DM_yaw real not null,";
  strSql += "DM_cnt INTEGER not null,";
  strSql += "sloppy_cnt INTEGER not null,";
  strSql += "recommend BOOLEAN not null,";
  strSql += "enable BOOLEAN not null,";

  strSql += "HaveSloppy BOOLEAN not null)";

  // strSql += ",primary key(DM_id))";
  ret = sqlite3_exec(*dataBasePtr, strSql.c_str(), 0, 0, &sqlErrMsg);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't create self table: %s\n",
            sqlite3_errmsg(*dataBasePtr));
    LOG->writeLog("Can't create self table: ");
    sqlite3_free(sqlErrMsg);
    exit(0);
  } else {
    fprintf(stderr, "Create self table successfully\n");
    LOG->writeLog("Create self table successfully");
    sqlite3_free(sqlErrMsg);
  }
}

void SelfDbController::addContent(std::shared_ptr<DM> dmPtr) {
  int32_t ret;
  char *sqlErrMsg;
  std::string strSql;
  std::shared_ptr<DmInfo> localDmInfoPtr = std::make_shared<DmInfo>();
  dmPtr->getInfo(localDmInfoPtr);
  strSql += "insert into DmList values (";
  strSql += std::to_string(localDmInfoPtr->id) + ",";
  strSql += std::to_string(localDmInfoPtr->pose.x) + ",";
  strSql += std::to_string(localDmInfoPtr->pose.y) + ",";
  strSql += std::to_string(localDmInfoPtr->pose.yaw) + ",";
  strSql += std::to_string(localDmInfoPtr->cnt) + ",";
  strSql += std::to_string(localDmInfoPtr->sloppyCnt) + ",";
  strSql += std::to_string(localDmInfoPtr->recommend) + ",";
  strSql += std::to_string(localDmInfoPtr->enable) + ",";
  strSql += std::to_string(localDmInfoPtr->haveSloppy) + "";
  strSql += ");";
  ret = sqlite3_exec(*dataBasePtr, strSql.c_str(), 0, 0, &sqlErrMsg);
  if (ret != SQLITE_OK) {
    LOG->writeLog("add self database item error");
  }
  sqlite3_free(sqlErrMsg);
}

void SelfDbController::createTrigger(std::shared_ptr<Config> configPtr) {
  int32_t ret;
  char *sqlErrMsg;
  std::string strSql;
  strSql = "DROP TRIGGER IF EXISTS keep_rows_size;";
  strSql += "CREATE TRIGGER keep_rows_size AFTER INSERT ON DmList\n";
  strSql += "    WHEN (SELECT COUNT(*) FROM DmList) > ";
  strSql += std::to_string(configPtr->maxRowSelfDb) + "\n";
  strSql += "    BEGIN\n";
  strSql += "        DELETE FROM DmList WHERE ROWID=";
  strSql += "(SELECT ROWID FROM DmList ORDER BY ROWID ASC LIMIT 100);\n";
  strSql += "    END;";

  ret = sqlite3_exec(*dataBasePtr, strSql.c_str(), 0, 0, &sqlErrMsg);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't add trigger: %s\n", sqlite3_errmsg(*dataBasePtr));
    LOG->writeLog("Can't add trigger: ");
    sqlite3_free(sqlErrMsg);
    exit(0);
  } else {
    fprintf(stderr, "Add trigger successfully\n");
    LOG->writeLog("Add trigger successfully");
    sqlite3_free(sqlErrMsg);
  }
}

int32_t SelfDbController::fitCallBack(void *arg, int32_t col, char **values,
                                      char **names) {
  DmInfo *cbDmInfoPtr = (DmInfo *)arg;
  cbDmInfoPtr->pose.x += atof(values[1]);
  cbDmInfoPtr->pose.y += atof(values[2]);
  cbDmInfoPtr->pose.yaw += atof(values[3]);
  return 0;
}

void SelfDbController::fitPose(const uint16_t threshold,
                               std::shared_ptr<DM> dmPtr) {
  int32_t ret;
  char *sqlErrMsg;
  std::string strSql;
  DmInfo localDmInfo;
  std::shared_ptr<DmInfo> localDmInfoPtr = std::make_shared<DmInfo>();
  dmPtr->getInfo(localDmInfoPtr);
  strSql += "select * from DmList";
  strSql += " where DM_id=";
  strSql += std::to_string(localDmInfoPtr->id);
  strSql += " limit " + std::to_string(threshold);
  ret = sqlite3_exec(*dataBasePtr, strSql.c_str(), fitCallBack,
                     (void *)&localDmInfo, &sqlErrMsg);
  sqlite3_free(sqlErrMsg);
  if (ret != SQLITE_OK) {
    std::cout << "查找不到,无法计算推荐位姿!" << std::endl;
    LOG->writeLog("Compute sloppy error: con't find DM in the list");
    return;
  }

  localDmInfoPtr->pose.x = localDmInfo.pose.x / threshold;
  localDmInfoPtr->pose.y = localDmInfo.pose.y / threshold;
  localDmInfoPtr->pose.yaw = localDmInfo.pose.yaw / threshold;
  dmPtr->setInfo(localDmInfoPtr);
}
/* self database*/

/* backup database*/
void BackupDbController::openDb() {
  int32_t ret;
  ret = sqlite3_open("../Recover/UsrBackupDb.db", &(*dataBasePtr));
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Can't open backup database: %s\n",
            sqlite3_errmsg(*dataBasePtr));
    LOG->writeLog("Can't open backup database: ");
    exit(0);
  } else {
    fprintf(stderr, "Opened backup database successfully\n");
    LOG->writeLog("Opened backup database successfully");
  }
}

void BackupDbController::backupUsrDb(std::shared_ptr<sqlite3 *> fromPtr) {
  sqlite3_backup *pBackup;
  pBackup = sqlite3_backup_init(*dataBasePtr, "main", *fromPtr, "main");
  if (pBackup) {
    sqlite3_backup_step(pBackup, -1);
    sqlite3_backup_finish(pBackup);

  } else {
    LOG->writeLog("user database backup create fail");
  }
}

bool BackupDbController::checkDate(std::shared_ptr<Config> configPtr) {
  std::string lastDate = configPtr->lastBackupDate;
  time_t nowDate;
  time(&nowDate);
  tm *p;
  p = localtime(&nowDate);
  MyDate originDate, tmpDate;
  originDate.year = stoi(lastDate.substr(0, 4));
  originDate.month = stoi(lastDate.substr(4, 2));
  originDate.day = stoi(lastDate.substr(6, 2));
  tmpDate.year = p->tm_year + 1900;
  tmpDate.month = p->tm_mon + 1;
  tmpDate.day = p->tm_mday;
  if (originDate.year != tmpDate.year) {
    std::string newTime =
        std::to_string(tmpDate.year) +
        (tmpDate.month < 10 ? "0" + std::to_string(tmpDate.month)
                            : std::to_string(tmpDate.month)) +
        (tmpDate.day < 10 ? "0" + std::to_string(tmpDate.day)
                          : std::to_string(tmpDate.day));
    YAML::Node configYaml = YAML::LoadFile("../config/config.yaml");
    std::ofstream fout("../config/config.yaml");
    configYaml["last_backup_time"] = newTime;
    fout << configYaml;
    fout.close();
    return true;
  }
  if (tmpDate.month - originDate.month > 1) {
    std::string newTime =
        std::to_string(tmpDate.year) +
        (tmpDate.month < 10 ? "0" + std::to_string(tmpDate.month)
                            : std::to_string(tmpDate.month)) +
        (tmpDate.day < 10 ? "0" + std::to_string(tmpDate.day)
                          : std::to_string(tmpDate.day));
    YAML::Node configYaml = YAML::LoadFile("../config/config.yaml");
    std::ofstream fout("../config/config.yaml");
    configYaml["last_backup_time"] = newTime;
    fout << configYaml;
    fout.close();
    return true;
  }

  if (tmpDate.day > originDate.day && tmpDate.month - originDate.month == 1) {
    std::string newTime =
        std::to_string(tmpDate.year) +
        (tmpDate.month < 10 ? "0" + std::to_string(tmpDate.month)
                            : std::to_string(tmpDate.month)) +
        (tmpDate.day < 10 ? "0" + std::to_string(tmpDate.day)
                          : std::to_string(tmpDate.day));
    YAML::Node configYaml = YAML::LoadFile("../config/config.yaml");
    std::ofstream fout("../config/config.yaml");
    configYaml["last_backup_time"] = newTime;
    fout << configYaml;
    fout.close();
    return true;
  }

  return false;
}

void BackupDbController::closeDb() { sqlite3_close(*dataBasePtr); }
/* backup database*/
