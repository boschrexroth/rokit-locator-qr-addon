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

#include "thread_pool.h"
#include "user_command_processing.h"
extern std::shared_ptr<LOG_T> LOG;
void ThreadHandler::execUserCommand() {
  signal(SIGINT, sigHandler);
  signal(SIGTERM, sigHandler);
  char quit;
  while (quit = getchar()) {
    if (quit == 'q' || quit == 'Q') {
      raise(SIGTERM);
      break;
    }
  }
}

void ThreadHandler::listenMode(std::shared_ptr<uint16_t> modePtr) {
  BinaryInterfaceClient client("localhost", 9004);
  uint32_t controlModeMessage;
  int32_t rv = 0;
  while (rv >= 0) {
    rv = client.read(&controlModeMessage, sizeof(controlModeMessage));

    if (rv > 0) {
      std::bitset<32> bit(controlModeMessage);

      int32_t index = 17;
      if (!bit.test(index--) && bit.test(index--) && !bit.test(index)) {
        (*modePtr) = RECORDING;
        continue;
      }
      index = 11;
      if (!bit.test(index--) && bit.test(index--) && !bit.test(index)) {
        (*modePtr) = LOCALIZATION;
        continue;
      }

      (*modePtr) = INIT;
    }
  }
}

void ThreadHandler::diagSocket() {
  int32_t fd;
  int32_t rc = 0;
  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd < 0) {
    LOG->writeLog("runtime error:socket create fail!");
    exit(0);
  }
  int32_t optval = 1;
  socklen_t optlen = sizeof(optval);
  rc = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
  if (rc != 0) {
    LOG->writeLog("runtime error:socket set alive fail!");
    exit(0);
  }
  timeval timeout;
  timeout.tv_usec = 0;
  timeout.tv_sec = 30;
  optlen = sizeof(timeout);
  rc = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, optlen);
  if (rc != 0) {
    LOG->writeLog("runtime error:socket set timeout fail!");
    close(fd);
    exit(0);
  }
  sockaddr_in servaddr4{};
  servaddr4.sin_family = AF_INET;
  servaddr4.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr4.sin_port = htons(9081);
  rc = ::bind(fd, (const sockaddr *)&servaddr4, sizeof(servaddr4));
  if (rc != 0) {
    LOG->writeLog("runtime error:diag socket bind fail!");
    LOG->writeLog(std::to_string(rc).c_str());
    close(fd);
    exit(0);
  }
  rc = listen(fd, 2);
  if (rc != 0) {
    LOG->writeLog("runtime error:socket listen fail!");
    close(fd);
    exit(0);
  }

  sockaddr_in remoteServaddr4{};
  socklen_t addrlen = sizeof(sockaddr_in);
  while (1) {
    int32_t tmpSockFd;
    tmpSockFd = accept(fd, (struct sockaddr *)&remoteServaddr4, &addrlen);
    diagSockFd = tmpSockFd;
    if (diagSockFd < 0) LOG->writeLog("diagnotics accept except");
  }
}

void ThreadHandler::sendDiagMessage(uint32_t qrDiagnostics) {
  int32_t sendrc = 0;
  if (diagSockFd > 0)
    sendrc = send(diagSockFd, &qrDiagnostics, sizeof(qrDiagnostics), 0);
  if (sendrc == -1) {
    LOG->writeLog("socket send message error!");
  }
}

void ThreadHandler::recCustomerPack(std::shared_ptr<CustomerPackage> deviInfo) {
  int32_t fd;
  int32_t rc = 0;
  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd < 0) {
    LOG->writeLog("runtime error:socket create fail!");
    exit(0);
  }
  int32_t optval = 1;
  socklen_t optlen = sizeof(optval);
  rc = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
  if (rc != 0) {
    LOG->writeLog("runtime error:socket set alive fail!");
    exit(0);
  }
  timeval timeout;
  timeout.tv_usec = 0;
  timeout.tv_sec = 30;
  optlen = sizeof(timeout);
  rc = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, optlen);
  if (rc != 0) {
    LOG->writeLog("runtime error:socket set timeout fail!");
    close(fd);
    exit(0);
  }
  sockaddr_in servaddr4{};
  servaddr4.sin_family = AF_INET;
  servaddr4.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr4.sin_port = htons(9080);
  rc = ::bind(fd, (const sockaddr *)&servaddr4, sizeof(servaddr4));
  if (rc != 0) {
    LOG->writeLog("runtime error:receive Customer socket bind fail!");
    LOG->writeLog(std::to_string(rc).c_str());
    close(fd);
    exit(0);
  }
  rc = listen(fd, 2);
  if (rc != 0) {
    LOG->writeLog("runtime error:socket listen fail!");
    close(fd);
    exit(0);
  }

  sockaddr_in remoteServaddr4{};
  socklen_t addrlen = sizeof(sockaddr_in);
  int32_t sockFd;
ACCEPT:
  sockFd = accept(fd, (struct sockaddr *)&remoteServaddr4, &addrlen);
  uint32_t idRecord = UINT32_MAX;
  CustomerPackage localPackage;
  while (1) {
    memset(&localPackage, 0, sizeof(localPackage));

    rc = recv(sockFd, (char *)&localPackage, sizeof(localPackage), 0);
    if (rc < 0) {
      LOG->writeLog("receive customer package fail!");
    }
    if (rc == 0) {
      goto ACCEPT;
    }
    if (rc > 0) {
      if (localPackage.dmId != idRecord) {
        std::unique_lock<std::mutex> lockGuard5(myMutex);
        idRecord = localPackage.dmId;
        *deviInfo = localPackage;
        deviInfo->deviationX *= -1;
        deviInfo->deviationY *= -1;
        deviInfo->deviationA *= -1;
        // The user needs to send the position and posture of the lens in the
        // QR code coordinate system
        // Here, the relative relationship is reversed
        myCond.notify_one();
      }
    }
  }
}

bool ThreadHandler::checkReturnValue(int32_t returnValue) {
  if (returnValue == 0) {
    return false;
  }
  return true;
}

bool ThreadHandler::dmTrigger(std::shared_ptr<DM> dmPtr,
                              std::shared_ptr<uint16_t> modePtr,
                              std::shared_ptr<int32_t> locStatPtr,
                              std::shared_ptr<CustomerPackage> deviInfo,
                              std::shared_ptr<DmInfo> rokitPosePtr) {
  std::unique_lock<std::mutex> lockGuard6(myMutex);
  myCond.wait(lockGuard6);
  std::shared_ptr<DmInfo> localDmInfoPtr = std::make_shared<DmInfo>();
  dmPtr->getInfo(localDmInfoPtr);
  double xdX = deviInfo->deviationX, xdY = deviInfo->deviationY,
         xdA = deviInfo->deviationA;
  uint32_t id = deviInfo->dmId;
  localDmInfoPtr->id = id;
  if ((*modePtr) == RECORDING) {
    std::shared_ptr<BinaryInterfaceClient> recBinPtr =
        std::make_shared<BinaryInterfaceClient>("localhost", 9008);
    RecordingVisualizationMessage message;
    int32_t rv = 0;
    while (rv >= 0) {
      rv = recBinPtr->read(&message.timestamp, sizeof(message.timestamp));
      checkReturnValue(rv);
      rv = recBinPtr->read(&message.visualizationId,
                           sizeof(message.visualizationId));
      checkReturnValue(rv);
      rv = recBinPtr->read(&message.status, sizeof(message.status));
      checkReturnValue(rv);
      rv = recBinPtr->read(&message.poseX, sizeof(message.poseX));
      if (checkReturnValue(rv) == false) continue;
      rv = recBinPtr->read(&message.poseY, sizeof(message.poseY));
      if (checkReturnValue(rv) == false) continue;
      rv = recBinPtr->read(&message.poseYaw, sizeof(message.poseYaw));
      if (checkReturnValue(rv) == false) continue;

      localDmInfoPtr->pose.x = message.poseX + xdX;
      localDmInfoPtr->pose.y = message.poseY + xdY;
      localDmInfoPtr->pose.yaw =
          message.poseYaw + xdA;  // TODO: maybe a question
      break;
    }
    dmPtr->setInfo(localDmInfoPtr);
  } else if ((*modePtr) == LOCALIZATION) {
    std::shared_ptr<BinaryInterfaceClient> locBinPtr =
        std::make_shared<BinaryInterfaceClient>("localhost", 9011);
    PoseMessage poseMessage;
    int32_t rv = 0;
    while (rv >= 0) {
      rv = locBinPtr->read(&poseMessage, sizeof(poseMessage));
      if (rv > 0) {
        localDmInfoPtr->pose.x = poseMessage.poseX;
        localDmInfoPtr->pose.y = poseMessage.poseY;
        localDmInfoPtr->pose.yaw = poseMessage.poseYaw;
        (*locStatPtr) = poseMessage.state;
        *rokitPosePtr = *localDmInfoPtr;
        break;
      }
    }
    localDmInfoPtr->pose.x += xdX;
    localDmInfoPtr->pose.y += xdY;
    localDmInfoPtr->pose.yaw += xdA;
    dmPtr->setInfo(localDmInfoPtr);
  }
  return true;
}
