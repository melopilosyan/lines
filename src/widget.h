#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QRadialGradient>

class QPushButton;
class QComboBox;
class QString;
class QDialog;
class QLabel;
class QPoint;
class QColor;


struct ball
{
    QColor color;
    QPointF point;
    bool flag;
    ball();
    void erase();
    ball operator=(ball&);
};


class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void getName(QString);
    void drawingPath();
    void changeSize();
    void animation();
    void newGame();

private:
    void drawGameField(QPainter*, int, int, int);
    bool ballStep(int, int, QVector<int>&);
    void mousePressEvent(QMouseEvent*);
    bool recurcia(QVector<int>&, int);
    void updateBallsCoordinates();
    void closeEvent(QCloseEvent*);
    void paintEvent(QPaintEvent*);
    //void drawNextBalls(QPainter*);
    void animateBall(QPainter*);
    void drawBalls(QPainter*);
    void getPreviousRecord();
    void drawPath(QPainter*);
    void calculateScore();
    bool pathFinder(int);
    void eraseBallsPos();
    void getPlayerName();
    void updateRecord();
    void saveRecord();
    void nextLevel();
    void addBalls();
    void gameOver();
    QRadialGradient radialGradient(ball&, unsigned);

    int column_num;
    QPushButton* buttonNewGame;
    QPushButton* buttonClose;
    QLabel* recordNameLabel;
    QPoint gameField0Point;
    QLabel* gameOverLabel;
    QComboBox* combobox;
    QString  playerName;
    QLabel* recordLabel;
    QPixmap* background;
    QLabel* comboLabel;
    QLabel* scoreLabel;
    ball selected_ball;
    QLabel* stepLabel;
    QVector<int> path;
    QColor pathColor;
    ball* nextBalls;
    QColor* colors;
    QTimer* timer;
    int* ballsPos;
    ball* balls;
    int score;
    int cellSz;
    int  record;
    int colorNum;
    int levelScore;
    int complexity;
    int stepNumber;
    bool pathFinded;
    bool isFieldFull;
    int animateState;
    bool isFieldEmpthy;
    int pathDrawedState;
};


#endif // WIDGET_H
