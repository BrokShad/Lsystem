#ifndef TURTLE_H
#define TURTLE_H
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "mainwindow.h"

class Turtle
{
public:
    Turtle();
    void translateString(QString mot);
    void translateChar(QChar c);

    void create3leafs();
    void moveForward();
    void createLeaf();
    void roll(QChar c);
    void turn(QChar c);
    void point(QChar c);
    void turn180();
    void addPoint();
    void multDist(float dL);
    void addWeigth(float w);

    Turtle operator =(Turtle t){
        return t;
    }

private:
    MyMesh::Point coord;
    float angleX;
    float angleY;
    float angleZ;
    float dist;
    float dL = 1;
    float w = 1;
};

#endif // TURTLE_H
