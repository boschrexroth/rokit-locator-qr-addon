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

#include "database_controller.h"
#include "rpc_client.h"
#include "thread_pool.h"
#include "user_command_processing.h"
#ifndef MAINLOGICHEADER
#define MAINLOGICHEADER
#include "main_logic.h"
#endif

auto LOG = std::make_unique<LOG_T>(LOG_T::getLogPtr());

void getSessionId(std::shared_ptr<RpcInfo> rpcInfoPtr, uint64_t &globalId,
                  std::shared_ptr<std::string> sessionIdPtr,
                  std::shared_ptr<Account> accountPtr) {
  rpcInfoPtr->id = globalId++;
  rpcInfoPtr->method = "sessionLogin";
  (*sessionIdPtr) = "";
  (*RpcClientSessLogIn::sessionIdPtr) = "";
  RpcClientSessLogIn sessionLogInClient(rpcInfoPtr, accountPtr);
  sessionLogInClient.sessionLogIn();
  (*sessionIdPtr) = (*RpcClientSessLogIn::sessionIdPtr);
  if (*sessionIdPtr == "") {
    LOG->writeLog("session log in failed-- get sessionId failed!");
    std::cout << "session log in failed-- get sessionId failed!" << std::endl;
  }
}

std::shared_ptr<Account> Account::inputAccount() {
  std::shared_ptr<Account> accountPtr = std::make_shared<Account>();
  YAML::Node accountYaml = YAML::LoadFile("../config/account.yaml");
  accountPtr->userName = accountYaml["UserName"].as<std::string>();
  accountPtr->passWord = accountYaml["PassWord"].as<std::string>();
  if (accountPtr->userName == "" || accountPtr->passWord == "") {
  INPUTACCOUNT:
    std::cout << "Please input username:" << std::endl;
    std::cin >> accountPtr->userName;
    std::cout << "Please input password:" << std::endl;
    std::cin >> accountPtr->passWord;
    std::shared_ptr<RpcInfo> localRpcInfoPtr = std::make_shared<RpcInfo>();
    std::shared_ptr<std::string> localSessionIdPtr =
        std::make_shared<std::string>("");
    std::uint64_t localId = 3333;
    getSessionId(localRpcInfoPtr, localId, localSessionIdPtr, accountPtr);
    if (*localSessionIdPtr == "") {
      std::cout << "input error,plz type again:" << std::endl;
      LOG->writeLog("username or password input error!");
      goto INPUTACCOUNT;
    }
    std::ofstream fout("../config/account.yaml");
    accountYaml["UserName"] = accountPtr->userName;
    accountYaml["PassWord"] = accountPtr->passWord;
    fout << accountYaml;
    fout.close();
    return accountPtr;
  } else {  // If the user name and password are not empty, they have been
            // entered successfully before. The password change will inform the
            // authorized personnel of the location of the yaml file and modify
            // it manually
    // No more verification of correctness
    return accountPtr;
  }
}

int main() {
  std::cout << "add_on Start!\n";
  std::cout << "Press "
               "q"
               " or "
               "Q"
               " to quit.\n";
  LOG->writeLog("add on start!---------------");

  std::shared_ptr<Account> accountPtr = Account::inputAccount();
  if (!accountPtr) {
    LOG->writeLog("account has some error, plz connect to your admin");
    std::cout << "account has some error, plz connect to your admin\n";
    exit(0);
  }

  ThreadHandler threadHandler;
  std::thread execUserCommandThread(&ThreadHandler::execUserCommand,
                                    std::ref(threadHandler));
  std::thread diagThread(&ThreadHandler::diagSocket,
                         std::ref(threadHandler));  // 9081
  std::shared_ptr<CustomerPackage> deviInfo =
      std::make_shared<CustomerPackage>();
  std::thread recCusThread(&ThreadHandler::recCustomerPack,
                           std::ref(threadHandler), deviInfo);  // 9080
  signal(SIGPIPE, SIG_IGN);
  std::shared_ptr<uint16_t> modePtr = std::make_shared<uint16_t>(INIT);
  std::thread listenModeThread(&ThreadHandler::listenMode,
                               std::ref(threadHandler), modePtr);

  uint32_t qrDiagnostics = 0;
  std::bitset<32> binBit(qrDiagnostics);
  CURLcode glbCurlFd = curl_global_init(CURL_GLOBAL_ALL);
  uint64_t globalId = 1;  // for rpc call

  std::shared_ptr<Config> configPtr = Config::getConfig();
  UserDbController userDB;
  SelfDbController selfDB;
  BackupDbController backupDB;

  userDB.openDb();
  selfDB.openDb();
  userDB.createTable();
  selfDB.createTable();
  selfDB.createTrigger(configPtr);
  if (backupDB.checkDate(configPtr)) {
    backupDB.openDb();
    std::shared_ptr<sqlite3 *> backupFromPtr = userDB.getDbPtr();
    backupDB.backupUsrDb(backupFromPtr);
    backupDB.closeDb();
  }

  std::shared_ptr<DM> dmPtr = std::make_shared<DM>();
  std::shared_ptr<DmInfo> dmInfoPtr = std::make_shared<DmInfo>();
  std::shared_ptr<RpcInfo> rpcInfoPtr = std::make_shared<RpcInfo>();
  std::shared_ptr<int32_t> locStatPtr = std::make_shared<int32_t>(2);
  std::shared_ptr<DmInfo> rokitPosePtr = std::make_shared<DmInfo>();
  std::shared_ptr<std::string> sessionIdPtr = std::make_shared<std::string>("");

  /*main while*/
  while (threadHandler.dmTrigger(dmPtr, modePtr, locStatPtr, deviInfo,
                                 rokitPosePtr)) {
    std::unique_lock<std::mutex> lockGuard7(threadHandler.myMutex);
    std::shared_ptr<DM> oriTriggerDmPtr = std::make_shared<DM>();
    oriTriggerDmPtr->dmInfo = dmPtr->dmInfo;
    if (RECORDING == (*modePtr)) {
      if (userDB.retrieveUsrDb(dmPtr)) {
        dmPtr->updateCnt();
        dmPtr->getInfo(dmInfoPtr);
        if (dmInfoPtr->cnt == configPtr->mappRepeatNum &&
            !(dmInfoPtr->enable)) {
          dmPtr->setRecommend();
          oriTriggerDmPtr->setRecommend();
          selfDB.addContent(oriTriggerDmPtr);
          selfDB.fitPose(configPtr->mappRepeatNum, dmPtr);
        } else if (!(dmInfoPtr->enable)) {
          selfDB.addContent(oriTriggerDmPtr);
        }
        if (dmInfoPtr->enable) {
          dmPtr->setRecommend();
          getSessionId(rpcInfoPtr, globalId, sessionIdPtr, accountPtr);
          rpcInfoPtr->id = globalId++;
          rpcInfoPtr->method = "clientGlobalAlignAddObservation";
          RpcClientGLA rpcClientGLA(rpcInfoPtr, dmInfoPtr, configPtr, deviInfo,
                                    sessionIdPtr);
          rpcClientGLA.postCurl();
          if (rpcClientGLA.computeSloppy(dmPtr, oriTriggerDmPtr) >
              configPtr->locErrThreshold) {
            dmPtr->updateSloppyCnt();
            oriTriggerDmPtr->updateSloppyCnt();
            oriTriggerDmPtr->setHaveSloppy();
            binBit.reset();
            binBit.set(2);
            qrDiagnostics = binBit.to_ulong();
            threadHandler.sendDiagMessage(qrDiagnostics);
          }
          oriTriggerDmPtr->setRecommend();
          selfDB.addContent(oriTriggerDmPtr);
        }
        userDB.updateDataBase(dmPtr);
      } else {
        dmPtr->clearRemainingInfo();
        oriTriggerDmPtr->clearRemainingInfo();
        dmPtr->updateCnt();
        oriTriggerDmPtr->updateCnt();
        userDB.addContent(dmPtr);
        binBit.reset();
        binBit.set(0);
        qrDiagnostics = binBit.to_ulong();
        threadHandler.sendDiagMessage(qrDiagnostics);
        selfDB.addContent(oriTriggerDmPtr);
      }
    }
    if (LOCALIZATION == (*modePtr)) {
      if (userDB.retrieveUsrDb(dmPtr)) {
        dmPtr->updateCnt();
        oriTriggerDmPtr->updateCnt();
        dmPtr->getInfo(dmInfoPtr);
        if (dmInfoPtr->enable) {
          getSessionId(rpcInfoPtr, globalId, sessionIdPtr, accountPtr);
          rpcInfoPtr->id = globalId++;
          rpcInfoPtr->method = "clientLocalizationSetSeed";
          RpcClientSetSeed rpcClientSetSeed(rpcInfoPtr, dmInfoPtr, locStatPtr,
                                            rokitPosePtr, configPtr,
                                            sessionIdPtr);
          rpcClientSetSeed.setSeed();
          if (rpcClientSetSeed.computeSloppy(dmPtr, oriTriggerDmPtr) >
              configPtr->locErrThreshold) {
            dmPtr->updateSloppyCnt();
            oriTriggerDmPtr->updateSloppyCnt();
            oriTriggerDmPtr->setHaveSloppy();
            binBit.reset();
            binBit.set(2);
            qrDiagnostics = binBit.to_ulong();
            threadHandler.sendDiagMessage(qrDiagnostics);
          }
          selfDB.addContent(oriTriggerDmPtr);
        }
        if (dmInfoPtr->cnt == configPtr->locRepeatNum && !(dmInfoPtr->enable)) {
          dmPtr->setRecommend();
          oriTriggerDmPtr->setRecommend();
          selfDB.addContent(oriTriggerDmPtr);
          selfDB.fitPose(configPtr->locRepeatNum, dmPtr);
        } else if (!(dmInfoPtr->enable)) {
          selfDB.addContent(oriTriggerDmPtr);
        }
        userDB.updateDataBase(dmPtr);
      } else {
        dmPtr->clearRemainingInfo();
        oriTriggerDmPtr->clearRemainingInfo();
        dmPtr->updateCnt();
        userDB.addContent(dmPtr);
        binBit.reset();
        binBit.set(0);
        qrDiagnostics = binBit.to_ulong();
        threadHandler.sendDiagMessage(qrDiagnostics);
        selfDB.addContent(oriTriggerDmPtr);
      }
    }
  } /*main while*/

  diagThread.join();
  recCusThread.join();
  listenModeThread.join();
  execUserCommandThread.join();

  return 0;
}