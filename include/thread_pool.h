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

class ThreadHandler {
public:
  ThreadHandler() = default;
  void execUserCommand();
  void listenMode(std::shared_ptr<uint16_t>);
  void diagSocket();
  void sendDiagMessage(uint32_t);
  void recCustomerPack(std::shared_ptr<CustomerPackage>);
  bool checkReturnValue(int32_t);
  bool dmTrigger(std::shared_ptr<DM>, std::shared_ptr<uint16_t>,
                 std::shared_ptr<int32_t>, std::shared_ptr<CustomerPackage>,
                 std::shared_ptr<DmInfo>);

  std::mutex myMutex;
  std::condition_variable myCond;
  int32_t diagSockFd = 0;
};
