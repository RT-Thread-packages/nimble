# 1 介绍

NimBLE 软件包是 RT-Thread 基于 [Apache NimBLE](https://github.com/apache/mynewt-nimble) 开源蓝牙 5.0 协议栈的移植实现，该协议栈提供完整的 Host 层和 Controller 层支持，目前支持 Nordic nRF51 和 nRF52 系列芯片。

## 1.1 主要特性

该协议栈的主要特性如下：

- 符合 5.0 核心规范
   - 1M, 2M 和 Coded PHY
   - Advertising Extensions
- 只支持低功耗蓝牙
- 通用访问配置(GAP)
   - central, peripheral, observer, broadcaster
   - privacy
   - multiple concurrent roles
- 安全管理(SM)
    - Legacy Pairing, Secure Connections
- 通用属性协议(GATT)
- L2CAP Connection Oriented Channels
- Bluetooth Mesh

更多关于 NimBLE Stack 的介绍请参考 ``http://mynewt.apache.org/latest/network/docs/index.html``。

## 1.2 特点与优势

同 nordic 系列常用的 SoftDevice 方案相比，NimBLE 有如下优势和特点：

- SoftDevice 核心软件以二进制形式发布，限制了灵活性或定制可能，NimBLE 则不受此影响
- 不依赖芯片和MCU架构
- 代码方便调试，易于修复和增强功能
- RAM 和 Flash 资源占用少
- 配置灵活，实现更高的数据吞吐

### 1.3 许可证

NimBLE 软件包遵循 Apache-2.0 许可，详见 LICENSE 文件。

### 1.4 依赖

- RT_Thread 3.0+

## 2 获取软件包

使用 NimBLE 软件包需要在 RT-Thread 的包管理中选中它，具体路径如下：

```
RT-Thread online packages
    IoT - internet of things  --->
         NimBLE:An open-source Bluetooth 5.0 stack porting on RT-Thread  --->
          [*]   Peripheral Role support                                   
          [ ]   Central Role support                                     
                Observer  --->                                           
                Common configuration  ----                               
                Host stack configuration  --->                           
                Controller configuretion  --->                           
                Log level (INFO)  --->                                   
                Samples (peripheral heart-rate)  --->         
                Version (latest)  --->
```

**Peripheral Role support**  ：  配置作为 Peripheral 设备；
**Central Role support  **  ：  配置作为 Central 设备；
**Observer**  ：  配置作为 Observer 设备；
**Host stack configuration**  ：  配置 Host Stack；
**Controller configuretion**  ：  配置 Controller；
**Log level (INFO)**  ：  配置协议栈日志等级；
**Samples**  ：  配置示例应用；
**Version**  ：  配置作为 Peripheral 设备；

配置完成后让 RT-Thread 的包管理器自动更新，或者使用 pkgs --update 命令更新包到 BSP 中。

## 3 使用 NimBLE 软件包

配合独立的 nrf52832-nimble bsp 使用，参考 https://github.com/EvalZero/nrf52832-nimble 。

## 4 注意事项
- NimBLE 当前处于开发阶段，暂时只支持 Nodic nRF52832 MCU，参见 [nrf52832-bsp](https://github.com/EvalZero/nrf52832-nimble)

## 5、联系方式 & 感谢

- 维护：RT-Thread 开发团队
- 主页：https://github.com/RT-Thread-packages/nimble
