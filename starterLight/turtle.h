#ifndef TURTLE_H
#define TURTLE_H
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "mainwindow.h"
#include <QStack>
#include <list>
#include <QVector>

class Turtle
{
public:
    Turtle(float x, float y, float z, float angleX_, float angleY_, float angleZ_, float dist_, float valAngle_);
    void translateString(QString mot, MyMesh *mesh, QVector<QString> *VertIdList);
    void translateChar(QChar c, MyMesh *mesh, QVector<QString> *VertIdList);

    void create3leafs();
    void moveForward();
    void createLeaf();
    void roll(QChar c);
    void turn(QChar c);
    void point(QChar c);
    void turn180();
    void addPoint(MyMesh *mesh, QVector<QString> *VertIdList);
    void multDist(float dL);
    void addWeigth(float w);
    std::list<Turtle> pile;
    Turtle equal(Turtle t);

private:
    QVector<MyMesh::VertexHandle> vertList;
    float x;
    float y;
    float z;
    MyMesh::Point coord = MyMesh::Point(x,y,z);
    float valAngle;

    float angleX;
    float angleY;
    float angleZ;
    float dist;
    float dL = 1;
    float w = 1;
};

#endif // TURTLE_H
