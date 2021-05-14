* [更新记录](#更新记录)
    + [2021-3-20](#2021-3-20)
    + [2021-3-22](#2021-3-22)
    + [2021-4-7](#2021-4-7)
    + [2021-5-17](#2021-5-17)
## 【智能车竞赛】2021单车组硬件开源
> 本人参与硬件部分，不过也是新手。把自己的硬件部分开源，希望和大家一起交流进步。
> 
> 好多资料网络上都可以查得到的，可以认为这里做的是硬件方案的分享。
### 更新记录
#### 2021-3-20
先上传前期的一套方案的原理图，目前遇到的问题是主板模块下载程序成功率较低。
可能是MCU供电和USB转TTL电压因为分开供电，电压不稳造成的。
    
[MCU.~(118).SchDoc](https://github.com/gone1724/Smartcar_K/blob/main/MCU.~(118).SchDoc)
    
[MOTOR V1.1.SchDoc](https://github.com/gone1724/Smartcar_K/blob/main/MOTOR%20V1.1.SchDoc)
    
[OPA4377四路 V2.0.SchDoc](https://github.com/gone1724/Smartcar_K/blob/main/OPA4377%E5%9B%9B%E8%B7%AF%20V2.0.SchDoc)

#### 2021-3-22
究极一体板方案！（bushi）这是最近尝试的一体板方案，包括全桥驱动芯片和四电感运放部分，希望不要出问题。

[MCU V3.1.SchDoc](https://github.com/gone1724/Smartcar_K/blob/main/MCU%20V3.1.SchDoc)

#### 2021-4-7
MCU V3.1的驱动部分，8V稳压好像只能到7.1V。

[MCU V3.1.SchDoc](https://github.com/gone1724/Smartcar_K/blob/main/MCU%20V3.1.SchDoc )

#### 2021-5-17
把主板上稳压去掉了，用的之前的驱动板驱动，几个月了，稳的一批。
不过我现在的2350运放又出问题了，之前还能用，现在咋用不了了，气！

[MCU V3.1.SchDoc](https://github.com/gone1724/Smartcar_K/blob/main/MCU%20V3.1.SchDoc)




***
![My Car‘s Photos](https://raw.githubusercontent.com/gone1724/Smartcar_K/main/My%20Car%E2%80%98s%20Photos/IMG_20210408_161804.jpg "没加电磁的K车")
