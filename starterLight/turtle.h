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

private:
    MyMesh::Point coord;
    float angleX;
    float angleY;
    float angleZ;
    float dist;

};

#endif // TURTLE_H
