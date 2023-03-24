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

#ifndef MAINLOGIC
#define MAINLOGIC
#include <arpa/inet.h>
#include <curl/curl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <signal.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <sqlite3.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <yaml-cpp/yaml.h>

#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "binary_interface_client.h"
#include "binary_interface_pose_struct.h"
#include "binary_interface_recording_vis_structs.h"
#endif

constexpr uint16_t RECORDING = 0xff;
constexpr uint16_t LOCALIZATION = 0xfe;
constexpr uint16_t INIT = 0xfd;

struct RpcInfo {
  std::string method;
  uint64_t id;
};

class Account {
 public:
  Account() = default;
  static std::shared_ptr<Account> inputAccount();
  std::string userName;
  std::string passWord;
};

void getSessionId(std::shared_ptr<RpcInfo>, uint64_t &,
                  std::shared_ptr<std::string>, std::shared_ptr<Account>);

// Singleten Pattern
class LOG_T {
 public:
  void operator()(const char *log) { logger->info(log); }
  void writeLog(const char *log) { logger->info(log); }
  const static LOG_T &getLogPtr() {
    const static LOG_T LOG;
    return LOG;
  }

 private:
  LOG_T() {
    logger = spdlog::rotating_logger_mt("addOnLogger", "../Logs/add_on_log.log",
                                        1024 * 1024 * 5, 10);
    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::trace);
  }
  LOG_T &operator=(const LOG_T &) = delete;
  std::shared_ptr<spdlog::logger> logger;
};

struct CustomerPackage {
  uint32_t dmId;
  double deviationX = 0;
  double deviationY = 0;
  double deviationA = 0;
};

class Config {
 public:
  Config() = default;
  static std::shared_ptr<Config> getConfig() {
    std::shared_ptr<Config> configPtr = std::make_shared<Config>();
    YAML::Node configYaml = YAML::LoadFile("../config/config.yaml");
    configPtr->locErrThreshold = configYaml["loc_error_threshold"].as<double>();
    configPtr->mappRepeatNum = configYaml["mapping_repeat_num"].as<uint16_t>();
    configPtr->locRepeatNum = configYaml["loc_repeat_num"].as<uint16_t>();
    configPtr->observationType = configYaml["observationType"].as<uint16_t>();
    configPtr->calibrationType = configYaml["calibrationType"].as<uint16_t>();
    configPtr->hasOrientation = configYaml["hasOrientation"].as<bool>();
    configPtr->maxRowSelfDb =
        configYaml["maxrow_selfDb_reserve"].as<uint32_t>();
    configPtr->seedSloppyThreshold =
        configYaml["seed_sloppy_threshold"].as<double>();
    configPtr->lastBackupDate =
        configYaml["last_backup_time"].as<std::string>();
    configPtr->laserStaticCalibSensor_x =
        configYaml["LASERstaticCalibSENSOR_x"].as<double>();
    configPtr->laserStaticCalibSensor_y =
        configYaml["LASERstaticCalibSENSOR_y"].as<double>();
    configPtr->laserStaticCalibSensor_yaw =
        configYaml["LASERstaticCalibSENSOR_yaw"].as<double>();
    return configPtr;
  }
  // 0～65535x2+1 ， No additional boundary check is required for the whole
  // program
  uint16_t mappRepeatNum;
  uint16_t locRepeatNum;
  double locErrThreshold;
  uint16_t observationType;
  uint16_t calibrationType;
  bool hasOrientation;
  uint32_t maxRowSelfDb;
  double seedSloppyThreshold;
  std::string lastBackupDate;
  double laserStaticCalibSensor_x;
  double laserStaticCalibSensor_y;
  double laserStaticCalibSensor_yaw;
};

struct MyDate {
  int32_t year, month, day;
};

struct DmPose {
  double x = 0, y = 0, yaw = 0;
};

struct DmInfo {
  uint32_t id;
  DmPose pose;
  uint64_t cnt = 0;
  uint64_t sloppyCnt = 0;
  bool recommend = false;
  bool enable = false;
  bool haveSloppy = false;
};

struct DM {
 public:
  DM() = default;
  DM(DmPose pose, uint32_t id) {
    dmInfo.pose = pose;
    dmInfo.id = id;
  }
  void getInfo(std::shared_ptr<DmInfo> dmInfoPtr) const {
    (*dmInfoPtr) = dmInfo;
  }
  void updateCnt() { dmInfo.cnt++; }
  void setInfo(std::shared_ptr<DmInfo> dmInfoPtr) { dmInfo = (*dmInfoPtr); }
  void updateSloppyCnt() {
    dmInfo.sloppyCnt++;
    dmInfo.haveSloppy = true;
  }
  void setRecommend() { dmInfo.recommend = true; }
  void setHaveSloppy() { dmInfo.haveSloppy = true; }
  void clearRemainingInfo() {
    dmInfo.cnt = 0;
    dmInfo.enable = 0;
    dmInfo.haveSloppy = 0;
    dmInfo.recommend = 0;
    dmInfo.sloppyCnt = 0;
  }

  DmInfo dmInfo;
};

// function declare
