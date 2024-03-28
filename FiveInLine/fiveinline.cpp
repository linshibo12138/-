#include "fiveinline.h"
#include "ui_fiveinline.h"

FiveInLine::FiveInLine(QWidget *parent) :
    QWidget(parent), m_board(FIL_COLS, std::vector<int>(FIL_ROWS, 0)),
    ui(new Ui::FiveInLine),m_isOver(false), m_BlackOrWhite(Black), m_movePoint(0, 0), m_moveFlag(true)
{
    ui->setupUi(this);
    m_pieceColor[None] = QBrush(QColor(0, 0, 0, 0));
    m_pieceColor[Black] = QBrush(QColor(0, 0, 0));
    m_pieceColor[White] = QBrush(QColor(255, 255, 255));

    connect(&m_Timer, SIGNAL(timeout())
            , this, SLOT(repaint()));
    m_Timer.start(1000 / 25);//1秒25次

    slot_startGame();
}

FiveInLine::~FiveInLine()
{
    m_Timer.stop();
    delete ui;
}

int FiveInLine::getBlackOrWhite() const
{
    return m_BlackOrWhite;
}

void FiveInLine::changeBlackAndWhite()
{
    if(m_BlackOrWhite == Black) {//若为黑，变白
        m_BlackOrWhite = White;
        ui->lb_color->setText("白子回合");
    }
    else {//若为白，变黑
        m_BlackOrWhite = Black;
        ui->lb_color->setText("黑子回合");
    }
}

bool FiveInLine::isCrossLine(int x, int y)
{
    if(x >= 15 || x < 0 || y < 0 || y >= 15) {
        return true;
    }
    return false;
}

bool FiveInLine::isWin(int x, int y)
{
    //遍历四条直线八个方向的同色棋子个数，只要有一个个数到5就结束了
    int count = 1;
    //循环遍历四条直线
    for(int dr = d_left; dr < 8; ++dr) {
        for(int j = 0; j < 2; ++j) {
            for(int i = 1; i <= 4; ++i) {
                //获取偏移后的棋子坐标
                int xi = dx[dr + j] * i + x;
                int yi = dy[dr + j] * i + y;
                //判断是否出界
                if(isCrossLine(xi,yi)) break;
                //判断是否同色
                if(m_board[xi][yi] == m_board[x][y]) {
                    count++;
                }
                else break;
            }
        }
        if(count >= 5) break;
        else {
            count = 1;
        }
    }
    if(count >= 5) {
        //结束
        m_isOver = true;
        return true;
    }
    return false;

}

void FiveInLine::clear()
{
    //状态位
    m_isOver = true;
    m_BlackOrWhite = Black;
    m_moveFlag = false;

    //界面
    for(int i = 0; i < FIL_COLS; ++i) {
        for(int j = 0; j < FIL_ROWS; ++j) {
            m_board[i][j] = None;
        }
    }
    ui->lb_winner->setText("");
    ui->lb_color->setText("黑子回合");
}

void FiveInLine::paintEvent(QPaintEvent *event)
{

    //绘制棋盘
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(255, 160, 0)));
    painter.drawRect(FIL_MARGIN_WIDTH - FIL_DISTANCE,
                     FIL_MARGIN_HEIGHT - FIL_DISTANCE,
                     (FIL_COLS + 1) * FIL_SPACE + 2 * FIL_DISTANCE,
                     (FIL_ROWS + 1) * FIL_SPACE + 2 * FIL_DISTANCE);

    //绘制网格线
    for(int i = 1; i <= FIL_COLS; ++i) {
        painter.setBrush(QBrush(QColor(0, 0, 0)));//通过画刷设置颜色
        QPoint p1(FIL_MARGIN_WIDTH + FIL_SPACE * i, FIL_MARGIN_HEIGHT + FIL_SPACE);
        QPoint p2(FIL_MARGIN_WIDTH + FIL_SPACE * i, FIL_MARGIN_HEIGHT + FIL_SPACE * (FIL_ROWS));
        painter.drawLine(p1, p2);
    }
    for(int i = 1; i <= FIL_ROWS; ++i) {
        painter.setBrush(QBrush(QColor(0, 0, 0)));//通过画刷设置颜色
        QPoint p1(FIL_MARGIN_WIDTH + FIL_SPACE, FIL_MARGIN_HEIGHT + FIL_SPACE * i);
        QPoint p2(FIL_MARGIN_WIDTH + FIL_SPACE * (FIL_COLS), FIL_MARGIN_HEIGHT + FIL_SPACE * i);
        painter.drawLine(p1, p2);
    }

    //绘制棋子
    //x,y点对应的颜色--->m_pieceColor[m_board[x][y]]
    for(int x = 0; x < FIL_COLS; ++x) {
        for(int y = 0; y < FIL_ROWS; ++y) {
            if(m_board[x][y] != None) {
                painter.setBrush(m_pieceColor[m_board[x][y]]);
                painter.drawEllipse(
                QPoint(FIL_MARGIN_HEIGHT + FIL_SPACE + FIL_SPACE * x,
                       FIL_MARGIN_WIDTH + FIL_SPACE + FIL_SPACE * y),
                            FIL_PIECE_SIZE / 2, FIL_PIECE_SIZE / 2
                            );
                //painter.drawEllipse(中心点坐标， 半径x, 半径y)
            }
        }
    }

    //显示移动的棋子,当前回合是哪个棋子，就走哪个颜色的棋子
    if(m_moveFlag) {
        painter.setBrush(m_pieceColor[getBlackOrWhite()]);//通过画刷设置颜色
        painter.drawEllipse(
        QPoint(m_movePoint.x(), m_movePoint.y()),
                    FIL_PIECE_SIZE / 2, FIL_PIECE_SIZE / 2
                    );
    }

    event->accept();
}

//鼠标点击，出现棋子
void FiveInLine::mousePressEvent(QMouseEvent *event)
{
    //捕捉点——相对坐标
    m_movePoint = event->pos();
    //位置坐标：
    //相对坐标——相对于窗口的坐标：QMouseEvent::pos()
    //绝对坐标——相对于桌面的坐标:QCurson::pos(), QMouseEvent::globalPos()

    //加入结束判断
    if(!m_isOver) {
        //点击状态
        m_moveFlag = true;
    }
    event->accept();
}

//鼠标移动，棋子跟着移动
void FiveInLine::mouseMoveEvent(QMouseEvent *event)
{
    if(m_moveFlag) {
        //捕捉点——相对坐标
        m_movePoint = event->pos();
    }

    event->accept();
}

//鼠标释放，棋子落子
void FiveInLine::mouseReleaseEvent(QMouseEvent *event)
{
    //加入结束判断
    if(!m_isOver) {
        m_moveFlag = false;
        //将相对坐标 换算为 棋盘坐标
        int x = 0, y = 0;
        float fx = (float)event->pos().x();
        float fy = (float)event->pos().y();
        fx = (fx - FIL_MARGIN_WIDTH - FIL_SPACE) / FIL_SPACE;
        fy = (fy - FIL_MARGIN_HEIGHT - FIL_SPACE) / FIL_SPACE;
        //四舍五入
        x = (fx - (int)fx) > 0.5 ? ((int)fx + 1) : (int)fx;
        y = (fy - (int)fy) > 0.5 ? ((int)fy + 1) : (int)fy;

        //是否越界
        if(isCrossLine(x, y)) return;
        //落子
        //Q_EMIT SIG_pieceDown(m_BlackOrWhite, m_movePoint.x(), m_movePoint.y());
        slot_pieceDown(getBlackOrWhite(), x, y);
    }
    event->accept();
}

void FiveInLine::slot_pieceDown(int BlackOrWhite, int x, int y)
{
    qDebug() <<"FiveInLine::slot_pieceDown";
    if(m_BlackOrWhite != getBlackOrWhite()) return;
    //落子  没有子才能放
    if(m_board[x][y] == None)  {
        m_board[x][y] = m_BlackOrWhite;

        //更新该点棋子颜色后，判断输赢
        if(isWin(x, y)) {
            QString str = getBlackOrWhite() == Black ? "黑方胜" : "白方胜";
            ui->lb_winner->setText(str);

        }
        else {
            changeBlackAndWhite();
        }

    }
}

void FiveInLine::slot_startGame()
{
    clear();
    m_isOver = false;
}
