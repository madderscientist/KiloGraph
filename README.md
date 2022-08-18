# KiloGraph
【SEU C++大作业】以 知识点+题目 构成的可视化知识图谱 KnowledgeGraph<br>
我们的目标是：<br>
老师使用它，可视化建立学科知识点网络和题库，导出文件给学生；学生做题并生成个性化知识掌握图谱。

# 更新日志
## 2022.8.18
利用Qt实现可视化。主要完成三个系统：视图系统、节点样式系统、物理受力系统。

### 视图系统
使用QGraphics Framework的思路：所有对象在Scene世界中，而程序窗口则是在Scene中的摄像机View，拥有两个属性：在Scene中的坐标和缩放倍率。视图的移动/放缩是先计算操作后的摄像机的位置，再根据放大倍率确定Scene中其他对象在View中的相对位置。Point之间的画线运用广度优先遍历。Point继承自QPushButton。

### 样式系统
每个Point拥有以下状态：

- 是否被选中
- 是否作为父节点（被选中）
- 是否隐藏子节点
- 是否隐藏

涉及选中高亮、子节点高亮、收折子节点等鼠标操作

### 物理受力系统
每个显示的节点都会受到其他节点对其的斥力，和显示的父节点对它的拉力。斥力选用电磁模型，即斥力和距离的平方成反比；拉力选择弹簧模型，即拉力和距离成正比。运动选择了 “加速度-速度-位移” 模型，并加入阻尼使运动丝滑。每秒全部重新计算40次，每次利用相互作用力的特点将复杂度降低一半。

### 我写了个bug，但是它刚好能运行
![image](https://github.com/madderscientist/KiloGraph/blob/main/READMEsources/PHYfailed.gif)<br>
做物理模型时曾将加速度错写成累加，导致能量越动越多，阻尼能让速度反向......查了半天才发现，期间差点被这个神奇的 “物理现象” 笑死。物理模型还需要和样式系统配合，但是文件多了长了，有特殊需求就加个判断，懒得去管、也没有能力管这里改了对其他地方的影响。编译，运行，哦，能跑，那挺好；哦，好奇怪的表现，那就当特性吧。。。<br>
![image](https://github.com/madderscientist/KiloGraph/blob/main/READMEsources/PHYsucceess.gif)

---
下一阶段目标是完成文件系统


## 2022.8.14
刚刚搭建好数据结构，由十字链表构成图，同时用单链表组织节点。
