# Purpose 

​	This repo is a special additional software for ‘’**ROKIT**‘’ product.  You can click **[here](https://www.boschrexroth.com.cn/zh/cn/components-for-mobile-robotics/)** to know more about "ROKIT". If you have some QR codes(DM code is also fine) deployed in your site, and your mobile robot happens to have a camera or code reader that can read the QR code. These information can be combined to **improve the mapping accuracy and localization accuracy** in some scenarios. 

​	One sentence summary :  Collect the position and posture of the QR code, and Assist SLAM in **Mapping** and **Localization**.

#  Installation

- Just clone this repo

  ```shell
  git clone xxxx
  ```

- Install "**rapidjson**"

   This is a very **lightweight** C++ library, which is very easy to install and use. Click [here](https://github.com/Tencent/rapidjson/) to jump to its github page. 

   I believe you can easily install it in two minutes.		 

- Install "**sqlite3**"

   https://www.sqlite.org/download.html  Find "source code" in this link. There are two packages under this title. Select the one with the 'configuration' script(The larger one). 

```shell
tar -xzvf sqlite-autoconf-3410000.tar.gz
cd sqlite-autoconf-3410000
sudo ./configure
sudo make
sudo make install  
```

- Install "**curl**"

  ```shell
  sudo apt install curl
  ```

- Install "**spdlog**"

  ```shell
  git clone https://github.com/gabime/spdlog.git
  cd spdlog && mkdir build && cd build
  cmake .. && make -j
  sudo make install
  ```

  By the way, install a tool for database visualization. 

  ```shell
  sudo apt install sqlitebrowser
  ```

- Now you can install **add_on** project.  Enter the folder you cloned first.

  ```shell
  mkdir build && cd build
  cmake ..
  make
  ```

   By default, the program will be built in the way of "**RelWithDebInfo**" . If you want to debug this program,  Replace the second cmake instruction with `cmake -DCMAKE_BUILD_TYPE=Debug ..` 

# USAGE

-  If you come to this step,  I believe you are already using "ROKIT" and the device has connected the code reader and radar. 

- We receive the QR code ID and relative position (from the lens center to the QR code center) sent from the code reader through port **9080**.

  In addition, you need to send the position and posture of the lens in the **QR code coordinate** system.

  ```c++
  struct CustomerPackage {
    uint32_t dmId;
    double deviationX = 0;
    double deviationY = 0;
    double deviationA = 0;
  };
  ```

-  We send some diagnostic messages through port **9081** (binary interface, currently under improvement). 

-  We wrote a **sample code** about client based on our hardware. If necessary, contact us to obtain. Contact us for instruction manual for more details.

-   The use of the program itself is very simple. It only needs to run the executable file generated before. After that, no matter what happens, you **don't need to pay attention to this program** (no need to restart, no need to care about the startup sequence of ROKIT, etc.).

-    The transformation between the device coordinate system and the code reader coordinate system can be simply configured directly in the **config.yaml** 

   ####  tips:  All the above connections are based on TCP protocol.

