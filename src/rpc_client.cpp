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

#include "rpc_client.h"
extern std::shared_ptr<LOG_T> LOG;
/*GLA*/
RpcClientGLA::RpcClientGLA(std::shared_ptr<RpcInfo> rpcInfoPtr,
                           std::shared_ptr<DmInfo> dmInfoPtr,
                           std::shared_ptr<Config> configPtr,
                           std::shared_ptr<CustomerPackage> deviInfo,
                           std::shared_ptr<std::string> sessionIdPtr) {
  this->rpcInfoPtr = rpcInfoPtr;
  this->dmInfoPtr = dmInfoPtr;
  this->configPtr = configPtr;
  this->deviInfo = deviInfo;
  this->sessionIdPtr = sessionIdPtr;
}

void RpcClientGLA::postCurl() {
  struct curl_slist *slist1;
  slist1 = NULL;  // necessary step
  slist1 = curl_slist_append(slist1, "Content-Type: application/json");

  std::shared_ptr<std::string> dataPtr = std::make_shared<std::string>();
  writeJsonGLA(dataPtr);
  const char *postNeed = (*dataPtr).c_str();
  std::cout << postNeed << std::endl;

  CURL *curlFd;
  curlFd = curl_easy_init();
  curl_easy_setopt(curlFd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(curlFd, CURLOPT_URL, "http://localhost:8080");
  curl_easy_setopt(curlFd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curlFd, CURLOPT_POSTFIELDSIZE, strlen(postNeed));
  curl_easy_setopt(curlFd, CURLOPT_POSTFIELDS, postNeed);
  curl_easy_setopt(curlFd, CURLOPT_POSTFIELDSIZE_LARGE,
                   (curl_off_t)(strlen(postNeed)));
  curl_easy_setopt(curlFd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(curlFd, CURLOPT_USERAGENT, "curl/7.68.0");
  curl_easy_setopt(curlFd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(curlFd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(curlFd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(curlFd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(curlFd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(curlFd, CURLOPT_WRITEFUNCTION, writeCbGLA);
  curl_easy_perform(curlFd);
  curl_easy_cleanup(curlFd);
}

void RpcClientGLA::writeJsonGLA(std::shared_ptr<std::string> strBufPtr) {
  rapidjson::StringBuffer jsBuf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(jsBuf);

  writer.StartObject();  // curl data start
  writer.Key("jsonrpc");
  writer.String("2.0");
  writer.Key("method");
  writer.String((rpcInfoPtr->method).c_str());
  writer.Key("params");
  writer.StartObject();  // query start
  writer.Key("query");
  writer.StartObject();  // msg start
  writer.Key("sessionId");

  writer.String((*sessionIdPtr).c_str());
  writer.Key("observation");
  writer.StartObject();  // observation start
  writer.Key("landmarkName");
  writer.String((*sessionIdPtr).c_str());
  writer.Key("sensorName");
  writer.String(std::to_string(dmInfoPtr->id).c_str());
  writer.Key("VEHICLEstaticCalibSENSOR");
  writer.StartObject();  // LASERstaticCalibSENSOR start
  writer.Key("x");
  writer.Double(configPtr->laserStaticCalibSensor_x);
  writer.Key("y");
  writer.Double(configPtr->laserStaticCalibSensor_y);
  writer.Key("a");
  writer.Double(configPtr->laserStaticCalibSensor_yaw);  // in rad
  writer.EndObject();  // LASERstaticCalibSENSOR end
  writer.Key("SENSORlandmark");
  writer.StartObject();  // SENSORlandmark start
  writer.Key("x");
  writer.Double(deviInfo->deviationX);
  writer.Key("y");
  writer.Double(deviInfo->deviationY);
  writer.Key("a");
  writer.Double(deviInfo->deviationA);  // in rad
  writer.EndObject();                   // SENSORlandmark end
  writer.Key("MAPlandmark");
  writer.StartObject();  // MAPlandmark start
  writer.Key("x");
  writer.Double(dmInfoPtr->pose.x);
  writer.Key("y");
  writer.Double(dmInfoPtr->pose.y);
  writer.Key("a");
  writer.Double(dmInfoPtr->pose.yaw);  // in rad
  writer.EndObject();                  // MAPlandmark end
  writer.Key("observationType");
  writer.Int(configPtr->observationType);
  writer.Key("calibrationType");
  writer.Int(configPtr->calibrationType);
  writer.Key("hasOrientation");
  writer.Bool(configPtr->hasOrientation);
  // writer.Key("timestamp"); // optional
  writer.EndObject();  // observation end
  writer.EndObject();  // msg end
  writer.EndObject();  // query end
  writer.Key("id");
  writer.Int64(rpcInfoPtr->id);
  writer.EndObject();  // curl data end
  *strBufPtr = jsBuf.GetString();
}

size_t RpcClientGLA::writeCbGLA(void *ptr, size_t size, size_t nmemb,
                                void *userp) {
  rapidjson::Document doc;
  size_t realsize = size * nmemb;
  doc.Parse((char *)ptr);
  if (doc.HasParseError()) {
    fprintf(stderr, "\nError(offset %u): %s\n", (unsigned)doc.GetErrorOffset(),
            rapidjson::GetParseError_En(doc.GetParseError()));
    LOG->writeLog("writerCbGLA prase doc error");
    return realsize;
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  // Here, usrp has been implemented and can point to files or other C-style
  // handles
  char *favor = (char *)userp;
  std::string tmpStr = buffer.GetString();
  favor = const_cast<char *>(tmpStr.c_str());
  PraseResponseGLA(doc);
  return realsize;
}

void RpcClientGLA::PraseResponseGLA(rapidjson::Document &doc) {
  if (doc.HasMember("error")) {
    rapidjson::Value &err1 = doc["error"]["code"];
    rapidjson::Value &err2 = doc["error"]["message"];
    std::cout << "Error code:  " << err1.GetInt() << std::endl;
    LOG->writeLog("RPC Error code:  ");
    LOG->writeLog(std::to_string(err1.GetInt()).c_str());
    std::cout << "Error message:  " << err2.GetString() << std::endl;
    LOG->writeLog("RPC Error message:  ");
    std::string tmpErrStr = err2.GetString();
    LOG->writeLog(tmpErrStr.c_str());
    return;
  }
  rapidjson::Value &res1 = doc["result"]["response"]["observationId"];
  std::cout << "observationId:  " << res1.GetInt64() << std::endl;
  LOG->writeLog("RPC observationId: ");
  LOG->writeLog(std::to_string(res1.GetInt64()).c_str());
  rapidjson::Value &res2 = doc["result"]["response"]["responseCode"];
  std::cout << "responseCode:  " << res2.GetUint64() << std::endl;
  LOG->writeLog("RPC responseCode: ");
  LOG->writeLog(std::to_string(res2.GetUint64()).c_str());
}

double RpcClientGLA::computeSloppy(std::shared_ptr<DM> dmPtr,
                                   std::shared_ptr<DM> oriDmPtr) {
  std::shared_ptr<DmInfo> dmInfoPtr = std::make_shared<DmInfo>();
  std::shared_ptr<DmInfo> oriDmInfoPtr = std::make_shared<DmInfo>();
  dmPtr->getInfo(dmInfoPtr);
  oriDmPtr->getInfo(oriDmInfoPtr);
  double result = 0;
  result = sqrt(pow(dmInfoPtr->pose.x - oriDmInfoPtr->pose.x, 2) +
                pow(dmInfoPtr->pose.y - oriDmInfoPtr->pose.y, 2));
  return result;
}

/*GLA*/

/*SetSeed*/
RpcClientSetSeed::RpcClientSetSeed(std::shared_ptr<RpcInfo> rpcInfoPtr,
                                   std::shared_ptr<DmInfo> dmInfoPtr,
                                   std::shared_ptr<int32_t> locStatPtr,
                                   std::shared_ptr<DmInfo> rokitPosePtr,
                                   std::shared_ptr<Config> configPtr,
                                   std::shared_ptr<std::string> sessionIdPtr) {
  this->rpcInfoPtr = rpcInfoPtr;
  this->dmInfoPtr = dmInfoPtr;
  this->locStatPtr = locStatPtr;
  this->rokitPosePtr = rokitPosePtr;
  this->configPtr = configPtr;
  this->sessionIdPtr = sessionIdPtr;
}

void RpcClientSetSeed::setSeed() {
  struct curl_slist *slist1;
  slist1 = NULL;  // necessary
  slist1 = curl_slist_append(slist1, "Content-Type: application/json");

  std::shared_ptr<std::string> dataPtr = std::make_shared<std::string>();
  writeJsonSetSeed(dataPtr);
  const char *postNeed = (*dataPtr).c_str();
  std::cout << postNeed << std::endl;

  CURL *curlFd;
  curlFd = curl_easy_init();
  curl_easy_setopt(curlFd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(curlFd, CURLOPT_URL, "http://localhost:8080");
  curl_easy_setopt(curlFd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curlFd, CURLOPT_POSTFIELDSIZE, strlen(postNeed));
  curl_easy_setopt(curlFd, CURLOPT_POSTFIELDS, postNeed);
  curl_easy_setopt(curlFd, CURLOPT_POSTFIELDSIZE_LARGE,
                   (curl_off_t)(strlen(postNeed)));
  curl_easy_setopt(curlFd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(curlFd, CURLOPT_USERAGENT, "curl/7.68.0");
  curl_easy_setopt(curlFd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(curlFd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(curlFd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(curlFd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(curlFd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(curlFd, CURLOPT_WRITEFUNCTION, writeCbSETS);
  curl_easy_perform(curlFd);
  curl_easy_cleanup(curlFd);
}

double RpcClientSetSeed::computeSloppy(std::shared_ptr<DM> dmPtr,
                                       std::shared_ptr<DM> oriDmPtr) {
  std::shared_ptr<DmInfo> dmInfoPtr = std::make_shared<DmInfo>();
  std::shared_ptr<DmInfo> oriDmInfoPtr = std::make_shared<DmInfo>();
  dmPtr->getInfo(dmInfoPtr);
  oriDmPtr->getInfo(oriDmInfoPtr);
  double result = 0;
  result = sqrt(pow(dmInfoPtr->pose.x - oriDmInfoPtr->pose.x, 2) +
                pow(dmInfoPtr->pose.y - oriDmInfoPtr->pose.y, 2));
  return result;
}

void RpcClientSetSeed::writeJsonSetSeed(
    std::shared_ptr<std::string> strBufPtr) {
  rapidjson::StringBuffer jsBuf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(jsBuf);

  writer.StartObject();  // curl data start
  writer.Key("jsonrpc");
  writer.String("2.0");
  writer.Key("method");
  writer.String((rpcInfoPtr->method).c_str());
  writer.Key("params");
  writer.StartObject();  // query start
  writer.Key("query");
  writer.StartObject();  // msg start
  writer.Key("sessionId");
  writer.String((*sessionIdPtr).c_str());

  std::pair<bool, bool> seedArg;
  if (*locStatPtr < 0) {
    std::shared_ptr<DM> rokitTmptr = std::make_shared<DM>();
    rokitTmptr->setInfo(rokitPosePtr);
    std::shared_ptr<DM> dbPoseTmptr = std::make_shared<DM>();
    dbPoseTmptr->setInfo(dmInfoPtr);
    double localSloppy = computeSloppy(rokitTmptr, dbPoseTmptr);
    if (localSloppy > configPtr->seedSloppyThreshold)
      seedArg = std::make_pair(true, false);
    else
      seedArg = std::make_pair(false, true);
  } else if (*locStatPtr == 1) {
    seedArg = std::make_pair(false, true);
  } else {
    seedArg = std::make_pair(false, false);
  }

  writer.Key("enforceSeed");
  writer.Bool(seedArg.first);
  writer.Key("uncertainSeed");
  writer.Bool(seedArg.second);
  writer.Key("seedPose");
  writer.StartObject();  // seedPose start
  writer.Key("x");
  writer.Double(dmInfoPtr->pose.x + configPtr->laserStaticCalibSensor_x);
  writer.Key("y");
  writer.Double(dmInfoPtr->pose.y + configPtr->laserStaticCalibSensor_y);
  writer.Key("a");
  writer.Double(dmInfoPtr->pose.yaw +
                configPtr->laserStaticCalibSensor_yaw);  // in rad
  writer.EndObject();                                    // seedPose end
  writer.EndObject();                                    // msg end
  writer.EndObject();                                    // query end
  writer.Key("id");
  writer.Int64(rpcInfoPtr->id);
  writer.EndObject();  // curl data end
  *strBufPtr = jsBuf.GetString();
}

size_t RpcClientSetSeed::writeCbSETS(void *ptr, size_t size, size_t nmemb,
                                     void *userp) {
  rapidjson::Document doc;
  size_t realsize = size * nmemb;
  doc.Parse((char *)ptr);
  if (doc.HasParseError()) {
    fprintf(stderr, "\nError(offset %u): %s\n", (unsigned)doc.GetErrorOffset(),
            rapidjson::GetParseError_En(doc.GetParseError()));
    LOG->writeLog("writeCbSETSeed prase doc error");
    return realsize;
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  char *favor = (char *)userp;
  std::string tmpStr = buffer.GetString();
  favor = const_cast<char *>(tmpStr.c_str());
  PraseResponseSETS(doc);
  return realsize;
}

void RpcClientSetSeed::PraseResponseSETS(rapidjson::Document &doc) {
  if (doc.HasMember("error")) {
    rapidjson::Value &err1 = doc["error"]["code"];
    rapidjson::Value &err2 = doc["error"]["message"];
    std::cout << "Error code:  " << err1.GetInt() << std::endl;
    LOG->writeLog("RPC Error code:  ");
    LOG->writeLog(std::to_string(err1.GetInt()).c_str());
    std::cout << "Error message:  " << err2.GetString() << std::endl;
    LOG->writeLog("RPC Error message:  ");
    const char *tmpErr2 = err2.GetString();
    LOG->writeLog(tmpErr2);
    return;
  }
  rapidjson::Value &res = doc["result"]["response"]["responseCode"];
  std::cout << "responseCode:  " << res.GetUint64() << std::endl;
  LOG->writeLog("RPC responseCode: ");
  LOG->writeLog(std::to_string(res.GetUint64()).c_str());
}
/*SetSeed*/

/*SessionLogIn*/

std::shared_ptr<std::string> RpcClientSessLogIn::sessionIdPtr =
    std::make_shared<std::string>();

RpcClientSessLogIn::RpcClientSessLogIn(std::shared_ptr<RpcInfo> rpcInfoPtr,
                                       std::shared_ptr<Account> accountPtr) {
  this->rpcInfoPtr = rpcInfoPtr;
  this->accountPtr = accountPtr;
}

void RpcClientSessLogIn::sessionLogIn() {
  struct curl_slist *slist1;
  slist1 = NULL;  // necessary
  slist1 = curl_slist_append(slist1, "Content-Type: application/json");

  std::shared_ptr<std::string> dataPtr = std::make_shared<std::string>();
  writeJsonLogIn(dataPtr);
  const char *postNeed = (*dataPtr).c_str();

  CURL *curlFd;
  curlFd = curl_easy_init();
  curl_easy_setopt(curlFd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(curlFd, CURLOPT_URL, "http://localhost:8080");
  curl_easy_setopt(curlFd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curlFd, CURLOPT_POSTFIELDSIZE, strlen(postNeed));
  curl_easy_setopt(curlFd, CURLOPT_POSTFIELDS, postNeed);
  curl_easy_setopt(curlFd, CURLOPT_POSTFIELDSIZE_LARGE,
                   (curl_off_t)(strlen(postNeed)));
  curl_easy_setopt(curlFd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(curlFd, CURLOPT_USERAGENT, "curl/7.68.0");
  curl_easy_setopt(curlFd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(curlFd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(curlFd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(curlFd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(curlFd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(curlFd, CURLOPT_WRITEFUNCTION, writeCbLogIn);
  curl_easy_perform(curlFd);
  curl_easy_cleanup(curlFd);
}

void RpcClientSessLogIn::writeJsonLogIn(
    std::shared_ptr<std::string> strBufPtr) {
  rapidjson::StringBuffer jsBuf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(jsBuf);

  writer.StartObject();  // curl data start
  writer.Key("jsonrpc");
  writer.String("2.0");
  writer.Key("method");
  writer.String((rpcInfoPtr->method).c_str());
  writer.Key("params");
  writer.StartObject();  // query start
  writer.Key("query");
  writer.StartObject();  // msg start
  writer.Key("timeout");
  writer.StartObject();  // timeout start
  writer.Key("valid");
  writer.Bool(true);
  writer.Key("time");
  writer.Int64(30);
  writer.Key("resolution");
  writer.Int(1);
  writer.EndObject();  // timeout end
  writer.Key("userName");
  writer.String(accountPtr->userName.c_str());
  writer.Key("password");
  writer.String(accountPtr->passWord.c_str());
  writer.EndObject();  // msg end
  writer.EndObject();  // query end
  writer.Key("id");
  writer.Int64(rpcInfoPtr->id);
  writer.EndObject();  // curl data end
  *strBufPtr = jsBuf.GetString();
}

size_t RpcClientSessLogIn::writeCbLogIn(void *ptr, size_t size, size_t nmemb,
                                        void *userp) {
  rapidjson::Document doc;
  size_t realsize = size * nmemb;
  doc.Parse((char *)ptr);
  if (doc.HasParseError()) {
    fprintf(stderr, "\nError(offset %u): %s\n", (unsigned)doc.GetErrorOffset(),
            rapidjson::GetParseError_En(doc.GetParseError()));
    LOG->writeLog("writeCbSETSeed prase doc error");
    return realsize;
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  char *favor = (char *)userp;
  std::string tmpStr = buffer.GetString();
  favor = const_cast<char *>(tmpStr.c_str());
  PraseResponseLogIn(doc);
  return realsize;
}

void RpcClientSessLogIn::PraseResponseLogIn(rapidjson::Document &doc) {
  if (doc.HasMember("error")) {
    rapidjson::Value &err1 = doc["error"]["code"];
    rapidjson::Value &err2 = doc["error"]["message"];
    std::cout << "Error code:  " << err1.GetInt() << std::endl;
    LOG->writeLog("RPC Error code:  ");
    LOG->writeLog(std::to_string(err1.GetInt()).c_str());
    std::cout << "Error message:  " << err2.GetString() << std::endl;
    LOG->writeLog("RPC Error message:  ");
    const char *tmpErr2 = err2.GetString();
    LOG->writeLog(tmpErr2);
    return;
  }
  rapidjson::Value &res1 = doc["result"]["response"]["responseCode"];
  std::cout << "RPC session log in responseCode:  " << res1.GetUint64()
            << std::endl;
  LOG->writeLog("RPC session log in responseCode: ");
  LOG->writeLog(std::to_string(res1.GetUint64()).c_str());
  rapidjson::Value &res2 = doc["result"]["response"]["sessionId"];
  std::cout << "session log in sessionId:  " << res2.GetString() << std::endl;
  (*sessionIdPtr) = res2.GetString();
  LOG->writeLog("session log in sessionId: ");
  std::string tmpLogSesStr = res2.GetString();
  LOG->writeLog(tmpLogSesStr.c_str());
}
/*SessionLogIn*/
