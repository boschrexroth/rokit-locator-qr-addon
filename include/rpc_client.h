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
