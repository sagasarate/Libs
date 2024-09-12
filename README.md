# EasyGameLibs
2024-9-12：进行目录调整，DistributedObjectServer暂时移除，稍后放置于独立项目中

2024-5-15：多项累积更新，主要是Lua库的大量Bug修正    

2023-6-32：
    增加复合数据类型CVariedValue
    增加消息组播功能，以位标记形式提供最多64个组，以及基于GroupID的组，前者组播效率较高
    LuaWrap支持UNICODE，升级lua5.4，调整完善在c++层和lua层的对象管理
    添加内置WebSocket支持
    增加CEasyTimer64,CTimerQueue改为64位计时
    调整了proxy的消息传递机制
    
2021-11-3：增加了定时器队列，并在DOS框架中增加了定时器功能，DOS的客户端代理缺省实现增加了消息组播的支持

2016-6-11：这次提交增加和修改的部分如下 网络库增加了对IPv6的支持，CIPAddress类有较大变化，以适应IPv6的加入 
    CNetConnection的OnRecvData函数参数有所变化 
    增加了一个DHT网络的实验性组件CDHTService，目前可以完成通过HashInfo下载种子的功能，不过性能还有问题，有待优化 
    工具库Utils中ILogPrinter接口的Log输出相关函数参数发生变化，取消DWORD型Color参数，增加字符串型Tag参数 
    PugXML中SaveToString和SaveToFile由xml_node搬到了xml_parser类，并修改了参数，增加了对带编码签名头的文件的支持 
    全部库中尽量取消了对long类型的使用，例如CSmartValue不再支持long类型的转换，避免Linux和Windows交叉使用的兼容问题 
    Lua库修正了一个可能导致栈溢出和非法访问的bug DistributedObjectServer现在整合了mono，所以可以使用c#来写逻辑对象模块了，不过因为整合过程中发现mono不是很靠谱的样子，实际应用还需要观察 
    DistributedObjectServer现在可以编写客户端代理插件了，从而可以实现与客户端协议的定制化，也可以实现网管定制化 
    IDistributedObject接口的OnObjectReport和OnProxyObjectIPReport函数参数有变化，升级项目注意同步修改，以免无法正常继承

Libs里是我长期积累的一些工具，里面包括操作系统基本操作，文件系统，网络，数据库等等，还有一个简单的3D渲染引擎， 可用于windows和linux平台的跨平台开发，（因为在windows平台下使用的项目比较多，所以windows部分的可靠性相对会高些）这些是过去式了，如今随着linux项目应用增多，如今Linux版本性能已经明显高于windows版，而且可靠性也略高了。

DistributedObjectServer里是一个分布式对象服务器的运行框架，可用于服务器开发。
