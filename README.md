<h1 align="center" style="margin: 30px 0 30px; font-weight: bold;">RT-THREAD NIMBLE</h1>
<h4 align="center">nimble BLE STACK for RT-THREAD</h4>
<p align="center">
	<a href="https://github.com/RT-Thread-packages/nimble/stargazers"><img src="https://img.shields.io/github/stars/RT-Thread-packages/nimble?style=flat-square&logo=GitHub"></a>
	<a href="https://github.com/RT-Thread-packages/nimble/network/members"><img src="https://img.shields.io/github/forks/RT-Thread-packages/nimble?style=flat-square&logo=GitHub"></a>
	<a href="https://github.com/RT-Thread-packages/nimble/watchers"><img src="https://img.shields.io/github/watchers/RT-Thread-packages/nimble?style=flat-square&logo=GitHub"></a>
	<a href="https://github.com/RT-Thread-packages/nimble/issues"><img src="https://img.shields.io/github/issues/RT-Thread-packages/nimble.svg?style=flat-square&logo=GitHub"></a>
</p>

## 1 介绍

NimBLE 软件包是 RT-Thread 基于 [Apache NimBLE](https://github.com/apache/mynewt-nimble) 开源蓝牙 5.0 协议栈的移植实现，该协议栈提供完整的 Host 层和 Controller 层支持，1.0 版本支持 Nordic nRF51 和 nRF52 系列芯片完整部署；最新版本可实现 RT-Thread 下支持 Host 层搭配串口连接外部 Controller 芯片使用。

### 1.1 主要特性

- 扩展广播(LE Advertising Extensions)
- 2Mbit/s比特率的物理层
- 长距离编码(Coded PHY for LE Long Range)
- 高速不可连接广播(High Duty Cycle Non-Connectable Advertising)
- 新的跳频算法(Channel Selection Algorithm #2)
- 隐私1.2(LE Privacy 1.2)
- 安全管理(SM),支持传统配对(LE Legacy Pairing),安全连接(LE Secure Connections),特定秘钥分发(Transport Specific Key Distribution)
- 链路层PDU数据长度扩展(LE Data Length Extension)
- 多角色并发(主机(central)/从机(peripheral), server/client)
- 同时广播和扫描
- 低速定向广播(Low Duty Cycle Directed Advertising)
- 连接参数请求(Connection parameters request procedure)
- LE Ping
- 完整的GATT客户端，服务端，以及子功能
- 抽象HCI接口层

### 1.2 Profile和Service支持

- 警报通知服务(ANS)
- 即时报警服务(IAS)
- 链路丢失服务(LLS)
- 电池服务(BAS)
- 设备信息服务(DIS)
- 心率服务(HRS)
- 自行车速度及步调(CSC)
- 射频功率(TPS)

### 1.3 Mesh 特性

- 广播和GATT承载(Advertising and GATT bearers)
- PB-GATT 和 PB-ADV provisioning
- 模型层(Foundation Models (server role))
- 支持中继(Relay support)
- 支持GATT代理(GATT Proxy)

更多关于 NimBLE Stack 的介绍请参考 ``http://mynewt.apache.org/latest/network/docs/index.html``。

### 1.4  目录结构

```txt
    📦NimBLE
    ┣ 📂apps 
    ┣ 📂docs
    ┣ 📂ext
    ┣ 📂nimble
    ┣ 📂porting
    ┗ 📂targets      
```

```
NimBLE
   ├───apps                   /* Bluetooth 示例应用程序 */
   │   ├───advertiser
   │   ├───blecent
   │   ├───blecsc
   │   ├───blehci
   │   ├───blehr
   │   ├───blemesh
   │   ├───blemesh_light
   │   ├───blemesh_shell
   │   ├───bleprph
   │   ├───bleuart
   │   ├───btshell
   │   ├───ext_advertiser
   │   └───ibeacon
   ├───docs                   /* 官方文档及 API 说明 */
   ├───ext
   │   └───tinycrypt          /* Tinycrypt 加密库 */
   ├───nimble
   │   ├───controller         /* Controller 实现 */
   │   │   ├───include
   │   │   └───src
   │   ├───drivers            /* Nordic 系列 Phy 驱动 */
   │   │   ├───nrf51
   │   │   └───nrf52
   │   ├───host               /* Host Stack(主机控制器)实现 */
   │   │   ├───include
   │   │   ├───mesh           /* Mesh 组网功能 */
   │   │   ├───pts            /* PTS 测试相关 */
   │   │   ├───services       /* 通用的 Profile */
   │   │   │   ├───ans
   │   │   │   ├───bas
   │   │   │   ├───bleuart
   │   │   │   ├───dis
   │   │   │   ├───gap
   │   │   │   ├───gatt
   │   │   │   ├───ias
   │   │   │   ├───lls
   │   │   │   └───tps
   │   │   ├───src
   │   │   ├───store
   │   │   ├───tools
   │   │   └───util
   │   ├───include
   │   │   └───nimble
   │   ├───src
   │   └───transport          /* HCI 传输抽象层 */
   │       ├───common         /* h4 协议工具 */
   │       ├───include
   │       └───rtthread       /* RT-Thread UART 对接 HCI 层代码 */
   └───porting                /* OS 抽象层及系统配置 */
       ├───nimble
       │   ├───include
       │   └───src
       └───npl
           └───rtthread       /* RT-Thread OS 接口实现 */
               ├───include
               │   ├───config /* NimBLE 协议栈配置选项 */
               │   ├───console
               │   ├───logcfg /* RT-Thread rtdbg 日志对接 */
               │   └───nimble
               └───src
```

### 1.5 许可证

NimBLE 软件包遵循 Apache-2.0 许可，详见 LICENSE 文件。

### 1.6 依赖

- RT_Thread 3.0+

## 2 获取软件包

使用 NimBLE 软件包需要在 RT-Thread 的包管理中选中它，具体路径如下：

```
RT-Thread online packages
    IoT - internet of things  --->
--- NimBLE:An open-source Bluetooth 5.0 stack porting on RT-Thread
      Bluetooth Role support  --->      
      Host Stack Configuration  --->
      Controller Configuration  --->
      Bluetooth Mesh support  --->
      HCI Transport support  ---->
      Device Driver support  ----
      Log level (INFO)  --->
      Bluetooth Samples (Not enable sample)  --->
(1)   Maximum number of concurrent connections
[*]   Device Whitelist Support
(0)   The number of multi-advertising instances
[ ]   Extended Advertising Feature Support
      Version (latest)  --->

```

**Bluetooth Role support**  ：  配置 BLE角色支持(Central/Peripheral/Broadcaster/Observer) ；   
**Host Stack Configuration**  ：  配置 Host 相关功能；   
**Controller Configuration**  ：  配置 Controller 相关功能；   
**Bluetooth Mesh support**  ：  Mesh 特性支持及配置；   
**HCI Transport support** ： 配置HCI层传输方式 ;  
**Device Driver support** ： 底层 SOC Phy 支持 ;  
**Log level (INFO)**  ：  配置协议栈日志等级；   
**Bluetooth Samples**  ：  配置示例应用；   
**Version**  ：  软件包版本选择；   

配置完成后让 RT-Thread 的包管理器自动更新，或者使用 pkgs --update 命令更新包到 BSP 中。

## 3 使用 NimBLE 软件包

1.0 版本配合独立的 nrf52832-nimble bsp 使用，参考 https://github.com/EvalZero/nrf52832-nimble 。

最新版本特性支持 RT-Thread 搭配 UART 外接蓝牙Controller卡片使用，参考以下文档：

-    [QEMU + 蓝牙Controller卡片使用 NimBLE](https://club.rt-thread.org/ask/article/47e1aad061e7a53c.html)
-   [如何在 ART-Pi 的 bsp 工程中使用 NimBLE 蓝牙协议栈](https://club.rt-thread.org/ask/article/2a90783d5ac51641.html)
-   [如何在 ART-Pi 的 Studio 工程中使用 NimBLE 蓝牙协议栈](https://club.rt-thread.org/ask/article/ed1e170fb2a30f0a.html)

外部蓝牙 Controller 选择及固件可参考 [蓝牙控制器固件](https://github.com/RT-Thread-packages/nimble/tree/master/docs/firmwares)

## 4 注意事项
- 如需使用 NimBLE Host层 和 Controller层完整部署，则需选择 1.0 版本运行。
- 1.0 版本暂时只支持 Nodic nRF52832 nRF52840 MCU，参见 [nrf52832-bsp](https://github.com/EvalZero/nrf52832-nimble)
- 最新版本目前仅能在 RT-Thread 上部署 Host 层，需要使用串口搭配外部蓝牙 Controller 芯片使用。 
- 文档中心相关资料：RT-Thread 标准版 -> 软件包 -> NimBLE 用户手册

## 5 联系方式 & 感谢

- 维护：RT-Thread 开发团队
- 主页：https://github.com/RT-Thread-packages/nimble
