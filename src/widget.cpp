#include "widget.h"
#include <QtGui>

ball::ball():color(QColor()),
             point(QPointF(-1, -1)),
             flag(false)
{

}

ball ball::operator=(ball& Ball)
{
    color = Ball.color;
    point = Ball.point;
    flag = Ball.flag;
    return *this;
}

void ball::erase()
{
    color = QColor();
    point = QPointF(-1, -1);
    flag = false;
}

Widget::Widget(QWidget *parent):QWidget(parent),
                                column_num(5),
                                buttonNewGame(new QPushButton(this)),
                                buttonClose(new QPushButton(this)),
                                recordNameLabel(new QLabel(this)),
                                gameField0Point(),
                                gameOverLabel(new QLabel(this)),
                                combobox(new QComboBox(this)),
                                playerName(),
                                recordLabel(new QLabel(this)),
                                background(new QPixmap),
                                comboLabel(new QLabel(this)),
                                scoreLabel(new QLabel(this)),
                                selected_ball(),
                                stepLabel(new QLabel(this)),
                                path(),
                                pathColor(),
                                nextBalls(new ball[3]),
                                colors(new QColor[7]),
                                timer(new QTimer(this)),
                                ballsPos(new int[column_num*column_num]),
                                balls(new ball[column_num*column_num]),
                                score(0),
                                cellSz(0),
                                record(0),
                                colorNum(5),
                                levelScore(0),
                                complexity(2),
                                stepNumber(0),
                                pathFinded(false),
                                isFieldFull(false),
                                animateState(-5),
                                isFieldEmpthy(false),
                                pathDrawedState(0)
{
    setMinimumSize(800, 560);
    setWindowTitle("Lines");

    buttonNewGame->setText("New Game");
    buttonClose->setText("Exit");
    comboLabel->setText("Choose Level");
    recordLabel->setText("Record");

    recordLabel->setGeometry(15, 25, 60, 20);
    recordNameLabel->setGeometry(15, 45, 110, 20);
    stepLabel->setGeometry(15, 75, 95, 20);
    scoreLabel->setGeometry(15, 105, 95, 20);

    combobox->insertItem(0, "Level 1");
    combobox->insertItem(1, "Level 2");
    combobox->insertItem(2, "Level 3");
    combobox->insertItem(3, "Level 4");
    combobox->insertItem(4, "Level 5");
    combobox->insertItem(5, "Level 6");

    colors[0] = Qt::yellow;
    colors[1] = Qt::red;
    colors[2] = Qt::green;
    colors[3] = Qt::blue;
    colors[4] = Qt::magenta;
    colors[5] = Qt::cyan;
    colors[6] = QColor("#dd9900");

    connect(timer, SIGNAL(timeout()), this, SLOT(animation()));
    connect(timer, SIGNAL(timeout()), this, SLOT(drawingPath()));
    connect(buttonClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(buttonNewGame, SIGNAL(clicked()), this, SLOT(newGame()));
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSize()));

    getPreviousRecord();
    eraseBallsPos();
    updateRecord();
    addBalls();
}

Widget::~Widget()
{
    delete []colors;
    delete []balls;
    delete []ballsPos;
    delete background;
}

void Widget::paintEvent(QPaintEvent* )
{
     QPainter p(this);


     // measure coordinates
     int min = (width() < height()) ? width() : height();
     int max = (width() > height()) ? width() : height();
     int pW_X = 0, pW_Y = 0;
     if((width() - height()) < 130) {
         pW_X = (max - min)/2+95;
         pW_Y = 95;
     }
     else {
         pW_X = (max - min)/2+30;
         pW_Y = 30;
     }
     gameField0Point = QPoint(pW_X, pW_Y);
     cellSz = (width()-2*pW_X)/column_num;
     int size = cellSz*column_num;                              // coordinates

     // draw buttons and labels
     buttonNewGame->setGeometry(width()-100, pW_Y, 90, 25);
     buttonClose->setGeometry(width()-100, pW_Y+size-30, 80, 25);
     comboLabel->setGeometry(width()-105, pW_Y+45, 95, 25);
     combobox->setGeometry(width()-99, pW_Y+75, 85, 25);
     scoreLabel->setText("Score:  " + QString::number(score));
     stepLabel->setText("Steps:  " + QString::number(stepNumber));
     gameOverLabel->setGeometry(width()/2-83,height()/2-20, 180, 40);

     // no comment
     drawGameField(&p, pW_X, pW_Y, size);
     updateBallsCoordinates();
     drawBalls(&p);

     if(isFieldFull) {
         gameOverLabel->setText("<h1><b><font color=black>Game Over</font></b></h1>");
         if(record < score)
            getPlayerName();
     }

     if(isFieldEmpthy) {
         gameOverLabel->setText("<h1><b><font color=black>You Won</font></b></h1>");
         if(record < score)
            getPlayerName();
     }

     if(pathFinded)
         drawPath(&p);


}

void Widget::drawGameField(QPainter *p, int pW_X, int pW_Y, int size)
{
    p->setPen(Qt::NoPen);
    QColor color(Qt::gray);
    p->setBrush(color);
    p->drawRect(pW_X, pW_Y, size, size);
    for(int i = 0; i <= size; i += cellSz) {
       for(int j = pW_X, count = 0; j < pW_X+size; j += cellSz, count++) {
           if(i != size) {
               p->setPen(color.lighter(145));
               p->drawLine(j+1, pW_Y+i+1, j+cellSz-1, pW_Y+i+1);
           }
           if(i) {
               p->setPen(color.darker(210));
               p->drawLine(j+2, pW_Y+i, j+cellSz, pW_Y+i);
           }
       }
       for(int j = pW_Y, count = 0; j < pW_Y+size; j += cellSz, count++) {
           if(i != size) {
               p->setPen(color.lighter());
               p->drawLine(pW_X+i+1, j+1, pW_X+i+1, j+cellSz-1);
               p->setPen(color.lighter());
               p->drawLine(pW_X+i+2, j+1, pW_X+i+2, j+cellSz-1);
           }
           else if(!i){
               p->setPen(color.lighter(145));
               p->drawLine(pW_X+i, j+1, pW_X+i, j+cellSz);
           }
           if(i) {
               p->setPen(color.darker(210));
               p->drawLine(pW_X+i, j+1, pW_X+i, j+cellSz);
           }
       }
    }
    p->setPen(color.lighter());
    p->drawLine(pW_X, pW_Y, pW_X+size, pW_Y);
    p->setPen(color.lighter(140));
    p->drawLine(pW_X, pW_Y+1, pW_X+size, pW_Y+1);
}

QRadialGradient Widget::radialGradient(ball& aBall, unsigned r)
{
    QRadialGradient radGrad( aBall.point,
                             r,
                             aBall.point + QPoint(-cellSz/2+7, -cellSz/2+7)
                            );
    radGrad.setColorAt(0, aBall.color.lighter(170));
    radGrad.setColorAt(0.5, aBall.color);
    radGrad.setColorAt(1, aBall.color.darker());
    radGrad.setSpread( QGradient::RepeatSpread );
    return radGrad;
}

void Widget::drawBalls(QPainter *p)
{
    for(int i = 0; i < column_num*column_num; i++) {
        if(balls[i].flag) {
            p->setPen(balls[i].color.darker(150));
            if(balls[i].point == selected_ball.point)
                animateBall(p);
             else {
                p->setBrush(radialGradient(balls[i], cellSz/2));
                p->drawEllipse(balls[i].point, cellSz/2-3, cellSz/2-3);
            }
        }
    }
}

void Widget::animateBall(QPainter *p)
{
    int x = selected_ball.point.x();
    int y = selected_ball.point.y();
    int r = cellSz/2-3.3;
    ball aBall = selected_ball;

    switch(animateState) {
        case 11:
        case 10:
            aBall.point = QPointF(x, y+2.5);
            p->setBrush(radialGradient(aBall, cellSz/2));
            p->drawEllipse(aBall.point, r+1, r-2.5);
            break;
        case 12:
        case 9:
            aBall.point = QPointF(x, y+1);
            p->setBrush(radialGradient(aBall, cellSz/2));
            p->drawEllipse(aBall.point, r+0.5, r-1);
            break;
        case 0:
        case 8:
            aBall.point = QPointF(x, y);
            p->setBrush(radialGradient(aBall, cellSz/2));
            p->drawEllipse(aBall.point, r, r);
            break;
        case 1:
        case 7:
            aBall.point = QPointF(x, y-2);
            p->setBrush(radialGradient(aBall, cellSz/2));
            p->drawEllipse(aBall.point, r, r);
            break;
        case 2:
        case 6:
            aBall.point = QPointF(x, y-4);
            p->setBrush(radialGradient(aBall, cellSz/2));
            p->drawEllipse(aBall.point, r, r);
            break;
        case 3:
        case 5:
            aBall.point = QPointF(x, y-6);
            p->setBrush(radialGradient(aBall, cellSz/2));
            p->drawEllipse(aBall.point, r, r);
            break;
        case 4:
            aBall.point = QPointF(x, y-8);
            p->setBrush(radialGradient(aBall, cellSz/2));
            p->drawEllipse(aBall.point, r, r);
            break;
        default:
            break;
    }
}

void Widget::drawPath(QPainter* painter)
{
    QVector<QPoint> pathPoint;
    int traceLenght = path.size();

    // make number of ball to real points in the game field
    for(int i = 0; i < traceLenght; i++) {
        int point_x = gameField0Point.x() + (path[i]%column_num) * cellSz + cellSz/2;
        int point_y = gameField0Point.y() + (path[i]/column_num) * cellSz + cellSz/2;
        pathPoint.push_back(QPoint(point_x+1, point_y+1));
    }

    // transfer selected ball
    if(pathDrawedState == traceLenght) {
        int point_x = gameField0Point.x() + (path[traceLenght-1]%column_num) * cellSz + cellSz/2;
        int point_y = gameField0Point.y() + (path[traceLenght-1]/column_num) * cellSz + cellSz/2;
        balls[path[traceLenght-1]].point = QPoint(point_x+1, point_y+1);
        balls[path[traceLenght-1]].color = balls[path[0]].color;
        balls[path[traceLenght-1]].flag = balls[path[0]].flag;
        balls[path[0]].erase();
    }
    // draw miny balls on path
    int r = (cellSz/2 - 3)/3;
    painter->setPen(pathColor);
    painter->setBrush(pathColor.darker(100));
    if(pathDrawedState < traceLenght)
        for(int i = 1; i < pathDrawedState; i++)
            painter->drawEllipse(pathPoint[i], r, r);
    else
        for(int i = (pathDrawedState - traceLenght+1); i < traceLenght-1; i++)
            painter->drawEllipse(pathPoint[i], r, r);
}

void Widget::changeSize()
{
    nextLevel();
    delete []balls;
    delete []ballsPos;
    isFieldFull = false;
    isFieldEmpthy = false;
    gameOverLabel->setText(NULL);
    int index = combobox->currentIndex();
    switch(index) {
        case 0: colorNum = 5; complexity = 2; column_num = 5; break;
        case 1: colorNum = 5; complexity = 3; column_num = 7; levelScore = 250; break;
        case 2: colorNum = 6; complexity = 4; column_num = 10; levelScore = 600; break;
        case 3: colorNum = 6; complexity = 4; column_num = 12; levelScore = 1000; break;
        case 4: colorNum = 7; complexity = 5; column_num = 14; levelScore = 1500; break;
        case 5: colorNum = 7; complexity = 5; column_num = 16; break;
        default: break;
    }
    balls = new ball[column_num*column_num];
    ballsPos = new int[column_num*column_num];
    eraseBallsPos();
    addBalls();
    update();
}

void Widget::eraseBallsPos()
{
    for(int i = 0; i < column_num*column_num; i++)
        ballsPos[i] = -1;
}

void Widget::updateBallsCoordinates()
{
    int balsSize = column_num*column_num;
    for(register int i = 0; i < balsSize; i++) {
        if(balls[i].flag) {
            int point_x = gameField0Point.x() + (i%column_num) * cellSz + cellSz/2;
            int point_y = gameField0Point.y() + (i/column_num) * cellSz + cellSz/2;
            balls[i].point = QPoint(point_x+1, point_y+1);
        }
    }
}

void Widget::addBalls()
{
    QTime midnight(0, 0, 0);
    qsrand( midnight.secsTo(QTime::currentTime()) );

    int freeCellCount = 0, addBallCount = 0;
    int color_num = qrand()%colorNum;
    for(int i = 0; i < column_num*column_num; i++)
        if(!balls[i].flag)
            freeCellCount++;
    if((freeCellCount >= 1) && (freeCellCount <= 3))
        isFieldFull = true;

    while((addBallCount != 3) && (freeCellCount != addBallCount)) {
        int ball_num = qrand()%(column_num * column_num);
        if(balls[ball_num].flag)
            continue;
        int cell_height = ball_num / column_num;
        int cell_width = ball_num % column_num;
        int point_x = gameField0Point.x() + cell_width * cellSz + cellSz/2;
        int point_y = gameField0Point.y() + cell_height * cellSz + cellSz/2;

        balls[ball_num].color = colors[(color_num++)%colorNum];
        balls[ball_num].point = QPoint(point_x+1, point_y+1);
        balls[ball_num].flag = true;
        addBallCount++;
    }
}

void Widget::mousePressEvent(QMouseEvent* event)
{
    QPoint mouseClickPoint = event->pos();

    // if gamer clicks not on the game field do nothing
    if((mouseClickPoint.x() < gameField0Point.x()) ||
       (mouseClickPoint.x() > (gameField0Point.x() + column_num*cellSz)))
        return;
    if((mouseClickPoint.y() < gameField0Point.y()) ||
       (mouseClickPoint.y() > (gameField0Point.y() + column_num*cellSz)))
        return;
    if(isFieldFull || isFieldEmpthy)
        return;                                         // do nothing

    int width = (mouseClickPoint.x() - gameField0Point.x()) / cellSz;
    int height = (mouseClickPoint.y() - gameField0Point.y()) / cellSz;
    int ball_num = column_num * height + width;
    if(balls[ball_num].flag) {
        if(balls[ball_num].point == selected_ball.point) {
            selected_ball.erase();
            timer->stop();
            animateState = 0;
            update();
        }
        else {
            selected_ball = balls[ball_num];
            animateState = 0;
            timer->start(30);
        }
    }
    else
        if(selected_ball.flag) {
            pathColor = selected_ball.color;
            if(pathFinder(ball_num)) {
                selected_ball.erase();
                pathFinded = true;
                pathDrawedState = 1;
                timer->start(30);
            }
            else {
                selected_ball.erase();
                timer->stop();
            }
        }
}

void Widget::closeEvent(QCloseEvent* ev)
{
    saveRecord();
    ev->accept();
}

void Widget::newGame()
{
    if(record < score)
        record = score;
    recordNameLabel->setText(playerName + " / " + QString::number(record));
    for(int i = 0; i < column_num*column_num; i++)
        balls[i].erase();
    gameOverLabel->setText(NULL);
    isFieldEmpthy = false;
    isFieldFull = false;
    animateState = 0;
    stepNumber = 0;
    saveRecord();
    addBalls();
    score = 0;
    update();
}

void Widget::animation()
{
    if(++animateState == 13)
        animateState = 0;
    update();
}

void Widget::drawingPath()
{
    int count = path.size();
    if(++pathDrawedState == 2*count) {
        pathFinded = false;
        calculateScore();
        updateRecord();
        path.clear();
        stepNumber++;
    }
    update();
}

bool Widget::pathFinder(int transferPos)
{
    QVector<int> nextSteps;
    // get selected balls posission
    int width = (selected_ball.point.x() - gameField0Point.x()) / cellSz;
    int height = (selected_ball.point.y() - gameField0Point.y()) / cellSz;
    int selectedPos = column_num * height + width;                         // get posission

    //remamber the selected ball posission
    ballsPos[selectedPos] = selectedPos;
    nextSteps.push_back(selectedPos);

    // if no way to reach to the transfer posission
    // clean ballsPos and return
    if(!recurcia(nextSteps, transferPos)) {
        eraseBallsPos();
        return false;
    }

    // get path in the vector
    int start = selectedPos, destination = transferPos;
    path.push_back(destination);
    while( true ) {
        if(ballsPos[destination] == start) {
            path.push_front(start);
            break;
        }
        path.push_front(ballsPos[destination]);
        destination = ballsPos[destination];
    }                                                       // path

    // clean ballsPos
    eraseBallsPos();
    return true;
}

bool Widget::recurcia(QVector<int>& selecteds, int destination)
{
    QVector<int> nextSteps;
    int selectedsNum = selecteds.size();
    for(int i = 0; i < selectedsNum; i++) {
        if(ballStep(selecteds.at(i), destination, nextSteps))
            return true;
    }
    if(nextSteps.isEmpty())
        return false;
    selecteds.clear();
    return recurcia(nextSteps, destination);
}

bool Widget::ballStep(int selectedPos, int transferPos, QVector<int>& NextSteps)
{
    // tast top cell
    if(selectedPos-column_num >= 0) {
        if(!balls[selectedPos-column_num].flag &&
                    (ballsPos[selectedPos-column_num] == -1)) {
            ballsPos[selectedPos-column_num] = selectedPos;
            if(transferPos == selectedPos-column_num)
                return true;
            else
                NextSteps.push_back(selectedPos-column_num);
        }
    }
    // tast right cell
    if((selectedPos%column_num + 1) < column_num) {
        if(!balls[selectedPos+1].flag &&
                    (ballsPos[selectedPos+1] == -1)) {
            ballsPos[selectedPos+1] = selectedPos;
            if(transferPos == selectedPos+1)
                return true;
            else
                NextSteps.push_back(selectedPos+1);
        }
    }
    // tast lower cell
    if((selectedPos/column_num + 1) < column_num) {
        if(!balls[selectedPos+column_num].flag &&
                    (ballsPos[selectedPos+column_num] == -1)) {
            ballsPos[selectedPos+column_num] = selectedPos;
            if(transferPos == selectedPos+column_num)
                return true;
            else
                NextSteps.push_back(selectedPos+column_num);
        }
    }
    // tast left cell
    if(selectedPos%column_num != 0) {
        if(!balls[selectedPos-1].flag &&
                    (ballsPos[selectedPos-1] == -1)) {
            ballsPos[selectedPos-1] = selectedPos;
            if(transferPos == selectedPos-1)
                return true;
            else
                NextSteps.push_back(selectedPos-1);
        }
    }
    return false;
}

void Widget::calculateScore()
{
    int horizontalCount = 1, verticalCount = 1,
        LRDiagonalCount = 1, RLDiagonalCount = 1;
    QVector<int> horizontalArr, horizontalArr1, verticalArr, verticalArr1,
                 LRDiagonalArr, LRDiagonalArr1, RLDiagonalArr, RLDiagonalArr1;
    for(int i = 0; i < column_num; i++) {
        for(int j = 0; j < column_num; j++) {
            if(balls[i*column_num + j].flag) {
                if((balls[i*column_num + j].color == balls[i*column_num + j + 1].color) &&
                                                                    (j != column_num-1)) {
                    horizontalArr.push_back((i*column_num + j));
                    horizontalCount++;
                }
                else {
                    if(horizontalCount >= complexity) {
                        score += (horizontalCount - complexity)*2 + complexity+1;
                        horizontalArr.push_back((i*column_num + j));
                        for(int k = 0; k < horizontalCount; k++)
                           horizontalArr1.push_back(horizontalArr[k]);
                    }
                    horizontalArr.clear();
                    horizontalCount = 1;
                }
            }
        }
        for(int j = 0; j < column_num; j++) {
            if(balls[j*column_num + i].flag) {
                if((balls[j*column_num + i].color == balls[j*column_num + i + column_num].color) &&
                                                                            (j != column_num-1)) {
                    verticalArr.push_back((j*column_num + i));
                    verticalCount++;
                }
                else {
                    if(verticalCount >= complexity) {
                        score += (verticalCount - complexity)*2 + complexity+1;
                        verticalArr.push_back((j*column_num + i));
                        for(int k = 0; k < verticalCount; k++)
                            verticalArr1.push_back(verticalArr[k]);
                    }
                    verticalArr.clear();
                    verticalCount = 1;
                }
            }
        }
        if(i <= (column_num-complexity+1)) {
            int ball_num = i;
            while( true ) {
                if(balls[ball_num].flag) {
                    if((balls[ball_num].color == balls[ball_num + 1 + column_num].color) &&
                            ((ball_num%column_num) != (column_num-1))) {
                        LRDiagonalArr.push_back(ball_num);
                        LRDiagonalCount++;
                    }
                    else {
                        if(LRDiagonalCount >= complexity) {
                            score += (LRDiagonalCount - complexity)*2 + complexity+1;
                            LRDiagonalArr.push_back(ball_num);
                            for(int k = 0; k < LRDiagonalCount; k++)
                                LRDiagonalArr1.push_back(LRDiagonalArr[k]);
                        }
                        LRDiagonalArr.clear();
                        LRDiagonalCount = 1;
                    }
                }
                if((ball_num%column_num) == (column_num-1))
                    break;
                ball_num += 1 + column_num;
            }
        }
        if(i && (i < (column_num-complexity+1))) {
            int ball_num = i*column_num;
            while(ball_num < column_num*column_num) {
                if(balls[ball_num].flag) {
                    if((balls[ball_num].color == balls[ball_num + 1 + column_num].color) &&
                                            (ball_num < column_num*column_num - column_num)) {
                        LRDiagonalArr.push_back(ball_num);
                        LRDiagonalCount++;
                    }
                    else {
                        if(LRDiagonalCount >= complexity) {
                            score += (LRDiagonalCount - complexity)*2 + complexity+1;
                            LRDiagonalArr.push_back(ball_num);
                            for(int k = 0; k < LRDiagonalCount; k++)
                                LRDiagonalArr1.push_back(LRDiagonalArr[k]);
                        }
                        LRDiagonalArr.clear();
                        LRDiagonalCount = 1;
                    }
                }
                ball_num += 1 + column_num;
            }
        }
        if(i >= complexity-1) {
            int ball_num = i;
            while( true ) {
                if(balls[ball_num].flag) {
                    if((balls[ball_num].color == balls[ball_num - 1 + column_num].color) &&
                                                                        ball_num%column_num) {
                        RLDiagonalArr.push_back(ball_num);
                        RLDiagonalCount++;
                    }
                    else {
                        if(RLDiagonalCount >= complexity) {
                            score += (RLDiagonalCount - complexity)*2 + complexity+1;
                            RLDiagonalArr.push_back(ball_num);
                            for(int k = 0; k < RLDiagonalCount; k++)
                                RLDiagonalArr1.push_back(RLDiagonalArr[k]);
                        }
                        RLDiagonalArr.clear();
                        RLDiagonalCount = 1;
                    }
                }
                if((ball_num%column_num) == 0)
                    break;
                ball_num += -1 + column_num;
            }
        }
        if((i > 1) && (i <= (column_num-complexity+1))) {
            int ball_num = i * column_num - 1;
            while(ball_num < column_num*column_num) {
                if(balls[ball_num].flag) {
                    if((balls[ball_num].color == balls[ball_num - 1 + column_num].color) &&
                                            (ball_num < column_num*column_num - column_num)) {
                        RLDiagonalArr.push_back(ball_num);
                        RLDiagonalCount++;
                    }
                    else {
                        if(RLDiagonalCount >= complexity) {
                            score += (RLDiagonalCount - complexity)*2 + complexity+1;
                            RLDiagonalArr.push_back(ball_num);
                            for(int k = 0; k < RLDiagonalCount; k++)
                                RLDiagonalArr1.push_back(RLDiagonalArr[k]);
                        }
                        RLDiagonalArr.clear();
                        RLDiagonalCount = 1;
                    }
                }
                ball_num += -1 + column_num;
            }
        }
    }
    // remove balls
    if(verticalArr1.size() || horizontalArr1.size() ||
            LRDiagonalArr1.size() || RLDiagonalArr1.size()) {
        //sleep(1);
        int sz = horizontalArr1.size();
        for(int k = 0; k < sz; k++)
            balls[horizontalArr1[k]].erase();
        sz = verticalArr1.size();
        for(int k = 0; k < sz; k++)
            balls[verticalArr1[k]].erase();
        sz = LRDiagonalArr1.size();
        for(int k = 0; k < sz; k++)
            balls[LRDiagonalArr1[k]].erase();
        sz = RLDiagonalArr1.size();
        for(int k = 0; k < sz; k++)
            balls[RLDiagonalArr1[k]].erase();
        int freeCellCount = 0;
        for(int k = 0; k < column_num*column_num; k++)
            if(!balls[k].flag)
                freeCellCount++;
        if(freeCellCount == column_num*column_num)
            isFieldEmpthy = true;
    }
    else
        addBalls();
}

void Widget::updateRecord()
{
    if(record < score) {
        playerName = "You";
        recordNameLabel->setText(playerName + " / " + QString::number(score));
    }

    if((score > 250) && (score <= 600) && (levelScore < 250))
        combobox->setCurrentIndex(1);
    else if((score > 600) && (score <= 1000) && (levelScore < 600))
        combobox->setCurrentIndex(2);
    else if((score > 1000) && (score <= 1500) && (levelScore < 1000))
        combobox->setCurrentIndex(3);
     else if((score > 1500) && (score <= 2200) && (levelScore < 1500))
        combobox->setCurrentIndex(4);
    else if(score > 2200)
        combobox->setCurrentIndex(5);
}

void Widget::saveRecord()
{
    QFile f("record");
    if (!f.open(QIODevice::WriteOnly))
        return;
    record = (record < score) ? score : record;
    QTextStream stream(&f);
    stream << playerName << "\n" << record;
    f.close();
}

void Widget::getPreviousRecord()
{
    QFile f("record");
    if(!f.open(QIODevice::ReadOnly)) {
        levelScore = 250;
        playerName = "Unknown";
        recordNameLabel->setText(playerName + " / " + QString::number(record));
        return;
    }

    QByteArray data = f.readLine();
    QString name(data);

    if(1 == name.size() || (name == "You\n"))
        playerName = "Unknown";
    else
        playerName = name.remove(name.size()-1, 1);

    data = f.readLine();
    record = data.toInt();
    levelScore = score;
    if((score > 250) && (score <= 600))
        combobox->setCurrentIndex(1);
    else if((score > 600) && (score <= 1000))
        combobox->setCurrentIndex(2);
    else if((score > 1000) && (score <= 1500))
        combobox->setCurrentIndex(3);
     else if((score > 1500) && (score <= 2200))
        combobox->setCurrentIndex(4);
    else if(score > 2200)
        combobox->setCurrentIndex(5);

    recordNameLabel->setText(playerName + " / " + QString::number(record));
}

void Widget::getPlayerName()
{
    QDialog* dialog = new QDialog(this);
    QLineEdit* lineEdit = new QLineEdit(dialog);
    QLabel* label = new QLabel(tr("Tell me your name"), dialog);

    dialog->setFixedSize(280, 130);
    label->setGeometry(73, 15, 170, 35);
    lineEdit->setGeometry(63, 55, 155, 30);

    connect(lineEdit, SIGNAL(returnPressed()), dialog, SLOT(close()));
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(newGame()));
    connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(getName(QString)));

    dialog->show();
    isFieldFull = false;
    isFieldEmpthy = false;
}

void Widget::getName(QString name)
{
    playerName = name;
}

void Widget::nextLevel()
{
    QDialog* showLevel = new QDialog(this);
    QLabel* label = new QLabel(showLevel);
    int level = 0;

    switch(combobox->currentIndex()) {
        case 0: level = 1; break;
        case 1: level = 2; break;
        case 2: level = 3; break;
        case 3: level = 4; break;
        case 4: level = 5; break;
        case 5: level = 6; break;
    }

    showLevel->setFixedSize(160, 60);
    label->setGeometry(48, 10, 80, 30);
    showLevel->setWindowTitle("Levels");
    label->setText("Level  " + QString::number(level));
    showLevel->show();

    timer->start(2000);
    connect(timer, SIGNAL(timeout()), showLevel, SLOT(close()));
}
