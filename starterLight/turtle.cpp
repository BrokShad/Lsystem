#include "turtle.h"

Turtle::Turtle(float x_, float y_, float z_, float angleX_, float angleY_, float angleZ_, float dist_)
{
    x = x_;
    y = y_;
    z = z_;
    angleX = angleX_;
    angleY = angleY_;
    angleZ = angleZ_;
    dist = dist_;
}

void Turtle::translateString(QString mot, MyMesh *mesh)
{
    pile.push_back(*this);
    for (int i = 0; i < mot.size() ; i++){
        translateChar(i,mesh);
    }
}

void Turtle::translateChar(QChar c,MyMesh *mesh)
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
        addPoint(mesh);
        break;
    case '"':
        multDist(dL);
        break;
    case '!':
        addWeigth(w);
        break;
    case '[':
    {
        pile.push_back(*this);
        break;
    }
    case ']':
        pile.pop_back();
        this->equal(pile.back());
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
    float newx = dist * sin(angleX) * cos(angleY);
    float newy = dist * sin(angleX)* sin(angleY);
    float newz = dist * cos(angleY);
    MyMesh::Point p = MyMesh::Point(newx,newy,newz);
    coord = p;
}

void Turtle::createLeaf()
{

}

void Turtle::turn(QChar c)
{
    if(c.unicode() == '+'){
        angleX += valAngle;
    } else {
        angleX -= valAngle;
    }
}

void Turtle::point(QChar c)
{
    if(c.unicode() == '&'){
        angleY += valAngle;
    } else {
        angleY -= valAngle;
    }
}

void Turtle::turn180()
{
    angleX *= -1;
    angleY *= -1;
}

void Turtle::addPoint(MyMesh *mesh)
{
    MyMesh::VertexHandle vh;
    vh = mesh->add_vertex(MyMesh::Point(coord));
}

void Turtle::multDist(float dL_)
{
    dist *= dL_;
}

void Turtle::addWeigth(float w_)
{
    this->w += w_;
}

Turtle Turtle::equal(Turtle t)
{
    this->angleX = t.angleX;
    this->angleY = t.angleY;
    this->angleZ = t.angleZ;
    this->dist = t.dist;
    this->dL = t.dL;
    this->w = t.w;
    return *this;
}
