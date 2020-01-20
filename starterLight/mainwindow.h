#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>

#include <QFileDialog>
#include <QMainWindow>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <QVector>
#include <QVector3D>
#include <QDebug>
#include <cmath>
#include <QTransform>

namespace Ui {
class MainWindow;
}

using namespace OpenMesh;
using namespace OpenMesh::Attributes;

struct MyTraits : public OpenMesh::DefaultTraits
{
    // use vertex normals and vertex colors
    VertexAttributes( OpenMesh::Attributes::Normal | OpenMesh::Attributes::Color | OpenMesh::Attributes::Status);
    // store the previous halfedge
    HalfedgeAttributes( OpenMesh::Attributes::PrevHalfedge );
    // use face normals face colors
    FaceAttributes( OpenMesh::Attributes::Normal | OpenMesh::Attributes::Color | OpenMesh::Attributes::Status);
    EdgeAttributes( OpenMesh::Attributes::Color | OpenMesh::Attributes::Status );
    // vertex thickness
    VertexTraits{float thickness; float value;};
    // edge thickness
    EdgeTraits{float thickness;};
};
typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> MyMesh;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString mot = "A";
    QString caseA = "F-++F&F^";
    QString caseF = "A[F&&F]";
    QString caseS = "";
    QString caseL = "";
    int iteration_number = 1;
    int angleX = 0;
    int angleY = 0;
    int angleZ = 0;
    int valAngle = 20;
    float dist = 1.0;
    int dL = 1;
    int w = 5;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString generer_mot(QString source);
    void displayMesh(MyMesh *_mesh, bool isTemperatureMap = false, float mapRange = -1);
    void resetAllColorsAndThickness(MyMesh* _mesh);

    QVector<QString> VertIdList;
    QVector<int>* toMesh();

    // VertIdList est le Qvector des id de chaque points du mesh.
    // les fils (comme dans la string de generation) sont séparés par des "[]"
    // Exemple : 0 [ 1 [ 2 ] ] 3 [ 4 ]
    // ce QVector signifie que :
    /*  0 est le pere de 1
     * 1 est le pere de 2
     * 3 est le frere (fait partie de la branche principale) de 0
     * 3 est aussi le pere de 4
     * */
    // Pour trouver quels couples utiliser on partira donc de l'ID courant, puis en allant vers le début du tableau :
    // si un "]" est croisé on attendra un "[" avant de pouvoir définir le couple
    // si un chiffre est croisé et qu'on est pas dans un "[]" alors on sait que le pere du vecteur courant est le chiffre que l'on
    //vient de croiser

    // Finalement il suffira de donner les deux points trouver a la fonciton de création de cylindre pour créé un tronc

    MyMesh* frustum_into_mesh(float xA, float yA, float zA,
                           float xB, float yB, float zB,
                           float radius, float coef_radius,
                           float step_r, float step_s, float step_t);

    void frustum_into_mesh(MyMesh* _mesh, float xA, float yA, float zA,
                           float xB, float yB, float zB,
                           float radius, float coef_radius,
                           float step_r, float step_s, float step_t);

private slots:
    void generer();

    void on_pushButton_generer_clicked();

    void on_lineEditA_textEdited(const QString &arg1);

    void on_lineEditF_textEdited(const QString &arg1);

    void on_lineEditS_textEdited(const QString &arg1);

    void on_lineEditL_textEdited(const QString &arg1);

    void on_iterationsSlider_valueChanged(int value);

    void on_angleXSlider_valueChanged(int value);

    void on_angleYSlider_valueChanged(int value);

    void on_angleZSlider_valueChanged(int value);

    void on_valAngleSlider_valueChanged(int value);

    void on_distSlider_valueChanged(int value);

    void on_dlSlider_valueChanged(int value);

    void on_wSlider_valueChanged(int value);

private:

    MyMesh mesh;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
