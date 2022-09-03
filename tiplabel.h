#ifndef TIPLABEL_H
#define TIPLABEL_H
#include <QLabel>
#include<QTimer>
#include <QDebug>

class TipLabel : public QLabel {
    Q_OBJECT
public:
    // 内容 父窗口 停留时间 是否自动排布
    explicit TipLabel(QString str,QWidget* parent,int time=2000,bool n=true) : QLabel(parent) {
        if (n) {
            int maxcharnum = ((0.33 * parent->width()) / 25);   //最大字符数
            str = multiRowText(str, maxcharnum);
        }
        setText(str);
        QTimer::singleShot(time, this, &QWidget::close);
        setStyleSheet("color:white;border-radius:16px;background-color:rgba(30,30,30,0.7);padding:20px 35px;font-size:25px;");
        setAttribute(Qt::WA_DeleteOnClose);                         // 自动析构
        setAlignment(Qt::AlignCenter);                              // 文字居中
        setAttribute(Qt::WA_TransparentForMouseEvents, true);       // 鼠标穿透
        setWindowFlags(Qt::WindowStaysOnTopHint);                   // 置顶
        setGeometry((parent->width() - sizeHint().width()) / 2,
                    (parent->height() - sizeHint().height()) / 2,
                    sizeHint().width(), sizeHint().height());       // 悬浮窗居中
        show();
    }
    static void showTip(QString str,QObject* parent,int time=2000,bool n=true){
        new TipLabel(str,(QWidget*)parent,time,n);
    }
    static QString multiRowText(QString str, int maxcharnum){
        int len = str.length();                             //字符串长
        int rownum = (len + maxcharnum - 0.5) / maxcharnum; //行数
        int charnum = len / rownum;                         //每行字符数（还没有+1）
        int extra = len - (charnum * rownum);               //需要+1的行数
        int pos = 0;                                        //访问字符串位置
        while (pos < len + rownum - 2) {
            if (extra) {
                pos += (charnum + 1);
                str.insert(pos, '\n');
                extra--;
            } else {
                pos += charnum;
                str.insert(pos, '\n');
            }
            pos++;
        }
        str[pos - 1] = 0;
        return str;
    }
};
#endif // TIPLABEL_H
