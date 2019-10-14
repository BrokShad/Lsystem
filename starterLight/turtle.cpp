#include "turtle.h"



void Turtle::translateString(QString mot)
{

}

void Turtle::translateChar(QChar c)
{
    switch (c.unicode()) {
    case 'A':
        create3leafs();
        break;
    case 'F':
        moveForward();
        break;
    case 'L':
        createLeaf();
        break;
    case 'D':
        roll('D');
        break;
    case 'G' :
        roll('G');
        break;
    case '+':
        turn('+');
        break;
    case '-':
        turn('-');
        break;
    case '&':
        point('&');
        break;
    case '^':
        point('^');
        break;
    case '|':
        turn180();
        break;
    case 'J':
        addPoint();
        break;
    case '"':
        multDist(dL);
        break;
    case '!':
        addWeigth(w);
        break;
    case '[':
    {
        pile.push(*this);
        break;
    }
    case ']':
        pile.pop();
        this->operator =(pile.top());
        break;
    default:
        break;
    }
}

void Turtle::create3leafs()
{

}

void Turtle::moveForward()
{

}

void Turtle::createLeaf()
{

}

void Turtle::roll(QChar c)
{

}

void Turtle::turn(QChar c)
{

}

void Turtle::point(QChar c)
{

}

void Turtle::turn180()
{

}

void Turtle::addPoint()
{

}

void Turtle::multDist(float dL)
{

}

void Turtle::addWeigth(float w)
{

}
