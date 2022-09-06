<h1 align='center'> KiloGraph </h1>
<img align='center' src='https://user-images.githubusercontent.com/51468627/188633244-c45dfc47-89cb-4fa3-916a-8c00e5571467.png'/>

【SEU C++大作业】以 知识点+题目 构成的可视化知识图谱 KnowledgeGraph <br>
我们的目标是：<br>
老师使用它，可视化建立学科知识点网络和题库，导出文件给学生；学生做题并生成个性化知识掌握图谱。


# 下载
[V 1.0](https://github.com/madderscientist/KiloGraph/releases/tag/v1.0)
使用说明也在release里面。一定要调阻尼！阻尼！阻尼！模型发散就调阻尼！

# 更新日志

## 2022/9/3

咕咕了许久，因为准备了此项目的中期答辩，还有选课、选导师等事。<br>
主要完成了最终模块——学生答题<br>
除了嵌入时的适配和新功能使用流程的设计制作，有三点值得一提：<br>

### 文件

新增.skg和.learn文件。.skg为学生只读文件，由.kg导出而成，采用了很简单很简陋的加密，防止学生看二进制文件得到答案；.learn为学习进度文件，只有在.skg中导入，一个.learn对应一个学习进度，一个.skg可以有多个.learn。<br>
和之前的版本相比，文件加入了文件头识别。此后文件格式应该不会改动。<br>
值得一提的是，.learn不绑定.skg文件，而是采用题目id匹配的弱耦合。.learn存储的是每个题目答题的历史情况以及题目的id，每个题目有自己独一无二的id。这么做的目的是，我们可以修改.kg并重新导出.skg，而旧.learn文件不会作废。你甚至可以把多个文件的进度存在一个.learn文件中。也因此带来一个问题：文件会越来越大，所以要注意文件的命名，不要乱导入。

### 推题

算法比较简陋。历史记录存在string中，每一位加权算题目权值。权值以1为“掌握”的分界，1以下为掌握，1以上为未掌握；没做的题权值赋10；每次选取权值最大的和与其权值差2以内的题目，随机一个出。做完更新历史string，并重新计算权值。为了和“历史”区分开，突出本次做题的连贯性和时效性，对的题额外减0.2，错的题额外加1。以上所有参数都没有精心调整过，因为做题不比背单词，答案是很容易记住的。目前的方案可以保证先出完没做到题，再做完错题。

### csv导入导出

在.kg界面题目可以导出/导入为.csv，csv的具体格式是：<br>
```
题干1, 答案1, 关联知识点1.1, 关联知识点1.2, ...
题干2, 答案2, 关联知识点2.1, 关联知识点2.2, ...
```

“关联知识点”指的是节点标题，导入时会自动匹配。<br>
但是导出的csv文件为utf-8格式，excel不能直接打开...

---
接下来就是些小优化了。不太想做下去了。我觉得最核心的功能在8/23日已经完成，而且具有很大的拓展性。这次更新只不过是为了迎合课题要求，路走窄了。在b站看到软体解迷宫问题，我觉得用咱的图就可以模拟这个过程。以后想得起来也许会剥离一个纯净版，只有图的可视化。


## 2022.8.25
完成了知识图谱的保存与读取<br>
完成了面向老师的题目列表<br>
意味着老师界面基本完成。<br>
现在是23:17，累死了，长话短说，就是题目列表真的写死我。呼出入口放在了右击菜单。文件方面是组员主力做的，所以是iostream

## 2022.8.23
重写了节点详情展示窗口tdetail:

- UI升级
- 可以管理出边
- 实现批量加边、批量增加子节点
- 节点标题时时更新

tdetail基于卡片视图Card，之后将以Card为基础写出 题目面板 和 学生detail。<br>
文件系统尚未完成，但已完成根据数据结构创建节点的功能。（之前是数据结构和UI同步维护，现在是先数据结构后UI，为读取文件做准备）<br>
在状态栏增加了 暂停物理模型 和 实时调节阻尼 的功能。之前若一次创建上百个子节点，因位置不当可能导致父节点猛烈震荡。现在可以先暂停，然后调节阻尼至最大，再启动物理效果。阻尼的范围是0.8~20.2，若低于0.8会过于Q弹；若高于20则阻尼会使低速度结点的速度反向。（因为时间不连续）

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
