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
