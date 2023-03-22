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
