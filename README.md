# KiloGraph
【SEU C++大作业】以 知识点+题目 构成的可视化知识图谱 KnowledgeGraph<br>
我们的目标是：<br>
老师使用它，可视化建立学科知识点网络和题库，导出文件给学生；学生做题并生成个性化知识掌握图谱。

# 更新日志
## 2022.8.20
主要改变了窗口的嵌套方式，使之适应未来的多文件打开。<br>
原本是MainWindow里面直接套Graph，现在加入了中间层Page。Page中装Graph和其他悬浮窗口（未做），统一管理快捷键，协同多窗口通信。而在MainWindow中，用QTabWidget组织Page，以支持多文件。<br>
中途遇到诸多crash，主要是QTabWidget的使用。第一次是addTab好为人父，会更改Widget的parent，导致Page通过parent()调用的不是MainWindow（当时的情况是构造函数中可以用，但之后就用不了，百思不得其解）；第二次是QTimer::singleShot执行顺序：在addTab时，由于Page的Graph有QTimer::singleShot(0)，因此addTab必须要在Graph的QTimer::singleShot之后，所以也得QTimer::singleShot。详见MainWindow.cpp的新建按钮操作的注释；第三次是removeTab时crash，因为忘了改&QTabWidget::currentChanged槽函数，导致如果关闭时显示的是最后一个tab，关闭后原本存的index就越界了。详见该函数的注释。

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
