#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

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
    QString caseA = "[&FL!A]DDDDD’[&FL!A]DDDDD’[&FL!A]";
    QString caseF = "S DDDDD F";
    QString caseS = "FL";
    QString caseL = "[’’’∧∧{-f+f+f-|-f+f+f}]";
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void generer_mot();
    void displayMesh(MyMesh *_mesh, bool isTemperatureMap = false, float mapRange = -1);
    void resetAllColorsAndThickness(MyMesh* _mesh);
private slots:
    void on_pushButton_chargement_clicked();

    void on_pushButton_generer_clicked();

    void on_pushButton_clicked();

    void on_lineEditA_textEdited(const QString &arg1);

    void on_lineEditF_textEdited(const QString &arg1);

    void on_lineEditS_textEdited(const QString &arg1);

    void on_lineEditL_textEdited(const QString &arg1);

    void on_pushButton_Reset_clicked();

    void on_pushButton_2_clicked();

private:

    MyMesh mesh;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
