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

class RpcClientGLA {
 public:
  RpcClientGLA(std::shared_ptr<RpcInfo>, std::shared_ptr<DmInfo>,
               std::shared_ptr<Config>, std::shared_ptr<CustomerPackage>,
               std::shared_ptr<std::string>);
  void postCurl();
  double computeSloppy(std::shared_ptr<DM>, std::shared_ptr<DM>);

 private:
  void writeJsonGLA(std::shared_ptr<std::string>);
  static size_t writeCbGLA(void *, size_t, size_t, void *);
  static void PraseResponseGLA(rapidjson::Document &);

  std::shared_ptr<RpcInfo> rpcInfoPtr;
  std::shared_ptr<DmInfo> dmInfoPtr;
  std::shared_ptr<Config> configPtr;
  std::shared_ptr<CustomerPackage> deviInfo;
  std::shared_ptr<std::string> sessionIdPtr;
};

class RpcClientSetSeed {
 public:
  RpcClientSetSeed(std::shared_ptr<RpcInfo>, std::shared_ptr<DmInfo>,
                   std::shared_ptr<int32_t>, std::shared_ptr<DmInfo>,
                   std::shared_ptr<Config>, std::shared_ptr<std::string>);
  double computeSloppy(std::shared_ptr<DM>, std::shared_ptr<DM>);
  void setSeed();

 private:
  void writeJsonSetSeed(std::shared_ptr<std::string>);
  static size_t writeCbSETS(void *, size_t, size_t, void *);
  static void PraseResponseSETS(rapidjson::Document &);

  std::shared_ptr<RpcInfo> rpcInfoPtr;
  std::shared_ptr<DmInfo> dmInfoPtr;
  std::shared_ptr<int32_t> locStatPtr;
  std::shared_ptr<DmInfo> rokitPosePtr;
  std::shared_ptr<Config> configPtr;
  std::shared_ptr<std::string> sessionIdPtr;
};

class RpcClientSessLogIn {
 public:
  RpcClientSessLogIn(std::shared_ptr<RpcInfo>, std::shared_ptr<Account>);
  void sessionLogIn();
  static size_t writeCbLogIn(void *, size_t, size_t, void *);
  void writeJsonLogIn(std::shared_ptr<std::string>);
  static void PraseResponseLogIn(rapidjson::Document &);

  static std::shared_ptr<std::string> sessionIdPtr;

 private:
  std::shared_ptr<RpcInfo> rpcInfoPtr;
  std::shared_ptr<Account> accountPtr;
};
