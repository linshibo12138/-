#ifndef FIVEINLINE_H
#define FIVEINLINE_H
#include <QDebug>
#include <QWidget>
#include <vector>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
namespace Ui {
class FiveInLine;
}

//界面是580*580固定

//外边距
#define FIL_MARGIN_HEIGHT   (50)//50
#define FIL_MARGIN_WIDTH    (50)
//行列数  五子棋规范为15*15
#define FIL_COLS            (15)
#define FIL_ROWS            (15)
//边和边之间的边距
#define FIL_SPACE           (28)
//棋子的大小
#define FIL_PIECE_SIZE      (24)
//棋盘边缘缩进的距离
#define FIL_DISTANCE        (10)






class FiveInLine : public QWidget
{
    Q_OBJECT

signals:
    //落子信号：鼠标释放捕捉落子位置
    void SIG_pieceDown(int BlackOrWhite, int x, int y);//什么颜色，落在什么位置
public:
    explicit FiveInLine(QWidget *parent = 0);
    ~FiveInLine();
    int getBlackOrWhite()const;
    void changeBlackAndWhite();
    //判断是否出界
    bool isCrossLine(int x, int y);
    //判断是否胜利
    bool isWin(int x, int y);
    //清空棋盘
    void clear();

    //1、首先绘制棋盘  网格线  棋子
    // 重绘事件，绘图背景  棋盘  棋子等
    void paintEvent(QPaintEvent *event);
    //2、鼠标点击，出现棋子，然后鼠标移动，棋子跟着移动，鼠标释放，棋子落子
    //需要涉及到鼠标点击、鼠标移动、鼠标释放事件
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


public slots:
     void slot_pieceDown(int BlackOrWhite, int x, int y);//什么颜色，落在什么位置
     void slot_startGame();
private:
    Ui::FiveInLine *ui;
    //方向，在一条线上的一组两个方向相邻， 方便写为循环
    enum ENUM_DIRECTION {d_left, d_right, d_up, d_down, d_leftup, d_rightdown, d_leftdown, d_rightup};
    enum ENUM_BLACK_OR_WHITE {None, Black, White};
    //存棋盘上棋子的二维数组
    std::vector<std::vector<int>> m_board;

    //绘制棋子的画刷
    QBrush m_pieceColor[3];
    //鼠标移动中鼠标所在位置
    QPoint m_movePoint;
    //移动标志
    bool m_moveFlag;

    //记录当前是黑子还是白子的回合
    int m_BlackOrWhite;
    //重绘定时器
    QTimer m_Timer;
    //结束标志
    bool m_isOver;
    //根据方向对坐标的偏移，每次是一个单位
    int dx[8] = {-1, 1, 0, 0, -1, 1, -1, 1};
    int dy[8] = {0, 0, -1, 1, -1, 1, 1, -1};
};

#endif // FIVEINLINE_H
