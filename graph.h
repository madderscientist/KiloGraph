#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include "point.h"

class Page;
// 画图区
class Graph : public QWidget
{
	Q_OBJECT
public:
	KG* kg;
	linklist<Point*> plist;
	Point* selected = nullptr;
	
    explicit Graph(Page *parent = nullptr);
	~Graph();
	// 获取Graph数据, 重绘界面
    void createByKG();
    Point* VtoP(V*,QPoint);
	
	// Point的创建和删除
	Point* addPointAtView(QPoint);
	Point* addPointAtScene(QPoint);
	void removeP(Point*);
	
	// 窗口大小改变事件
	void resizeEvent(QResizeEvent*);
	
	// 鼠标事件
	void mousePressEvent(QMouseEvent*);     //鼠标按下事件
	void mouseReleaseEvent(QMouseEvent*);   //鼠标松开事件
	void mouseMoveEvent(QMouseEvent*);      //鼠标按下并移动事件，point全体移动
	void wheelEvent(QWheelEvent*);          //滚轮，point全体缩放(大小和位置）
	
	// 键盘事件
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);
	
    void setSelected(Point*, Point* =nullptr);//改变selected变量及样式
	void backtoCenter();                    //回到中心点
	
    void moveView(QPoint,QPoint);           //集体按向量移动
	void refreshLocation();                 //根据original修改组件位置
    void zoom(int, QPoint);                 //缩放
	QPoint mapToScene(QPoint);              //View映射到Scene坐标
    QPoint mapToView(QPoint);               //Scene映射到View坐标
	
	QPoint clickpos;                        // 在View上的点击位置
	double zoomTime = 1;                    //倍率，由zoomTime确定
    QPoint origin;                          //View原点在Scene的坐标
	
	// 画图事件
	void paintEvent(QPaintEvent*);
	void drawArrow(Point*, Point*);         //在Point之间画箭头
	void BFS_Draw(V*, bool*);               //从某节点开始广度遍历画箭头
	
	void autoMove();                        //物理模型
    float DAMPING = 1.2;                    // 阻尼
    void timerSwitch(bool);
    QTimer* timer;

private:
	int zoomNum = 0;                        //放大倍数
	bool ifdrag = false;
    QPixmap* canvas = nullptr;              //总图 【2022/23:57:艹tmd，就是因为这个没有赋初值，debug了一晚上
signals:
		
};

#endif // GRAPH_H
