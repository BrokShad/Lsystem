#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "turtle.h"
#include <stdlib.h>

/* **** début de la partie boutons et IHM **** */


void verif_mesh_vertices(MyMesh *_mesh)
{
    for(int i = 0 ; i < _mesh->n_vertices(); i++)
    {
        qDebug() << "sommet " << i << " " << _mesh->point(_mesh->vertex_handle(i))[0] << ", " << _mesh->point(_mesh->vertex_handle(i))[1] << ", " << _mesh->point(_mesh->vertex_handle(i))[2] << endl;
    }
}


// exemple pour construire un mesh face par face
void MainWindow::on_pushButton_generer_clicked()
{
    mot = "A";
    Turtle t = Turtle(0,0,0,angleX*M_PI/180,angleY*M_PI/180,angleZ*M_PI/180,dist,valAngle*M_PI/180);
    t.translateString(mot,&mesh,&VertIdList);

    MyMesh *new_mesh = new MyMesh();

    // on construit une liste de paire de sommets -1 étant l'origine du repère

    QVector<int> *pairs = toMesh();

    qDebug() << *pairs << endl;
    //qDebug() << pairs->count() << endl;

    for(int i = 1; i < pairs->count(); i +=2)
    {
        MyMesh::Point from, to;

        int index_from = pairs->data()[i-1];
        int index_to = pairs->data()[i];

        if(index_from < 0)
        {
            from = MyMesh::Point(0, 0, 0);
        }
        else
        {
            from = mesh.point(VertexHandle(index_from));
        }

        if(index_to < 0)
        {
            to = MyMesh::Point(0, 0, 0);
        }

        else
        {
            to = mesh.point(VertexHandle(index_to));
        }

        frustum_into_mesh(new_mesh,
                          from[0] , from[1], from[2] ,
                          to[0]  ,  to[1] ,  to[2]  ,
                          0.05f, 0,
                          0.05f, 0.07f, 0.04f);

        //qDebug() << "From " << from[0] << ", " << from[1] << ", " << from[2] << endl;
        //qDebug() << "to " << to[0] << ", " << to[1] << ", " << to[2] << endl;
        //qDebug() << "frustum_into_mesh(new_mesh," << from[0]*10 << "," << from[1]*10 << "," << from[2]*10 << "," <<
                                            //to[0]*10 << "," << to[1]*10 <<"," << to[2]*10 << "," <<
                                           // "0.01f, 0,0.05f, 0.07f, 0.04f);" << endl;
    }

    //verif_mesh_vertices(new_mesh);

    new_mesh->update_normals();
    resetAllColorsAndThickness(new_mesh);
    displayMesh(new_mesh);

    /*
    try
      {
        if ( !OpenMesh::IO::write_mesh(*new_mesh, "output.obj") )
        {
          qDebug()<< "Cannot write mesh to file 'output.obj'" << endl;
        }
      }
      catch( std::exception& x )
      {
        qDebug() << x.what() << endl;
      }
      */

}

/* **** fin de la partie boutons et IHM **** */



/* **** fonctions supplémentaires **** */
// permet d'initialiser les couleurs et les épaisseurs des élements du maillage
void MainWindow::resetAllColorsAndThickness(MyMesh* _mesh)
{
    for (MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++)
    {
        _mesh->data(*curVert).thickness = 1;
        _mesh->set_color(*curVert, MyMesh::Color(0, 0, 0));
    }

    for (MyMesh::FaceIter curFace = _mesh->faces_begin(); curFace != _mesh->faces_end(); curFace++)
    {
        _mesh->set_color(*curFace, MyMesh::Color(150, 150, 150));
    }

    for (MyMesh::EdgeIter curEdge = _mesh->edges_begin(); curEdge != _mesh->edges_end(); curEdge++)
    {
        _mesh->data(*curEdge).thickness = 1;
        _mesh->set_color(*curEdge, MyMesh::Color(0, 0, 0));
    }
}

int find_parent(QStack<int> *parents)
{
    if(parents->empty())
    {
        return -1;
    }

    return parents->top();
}

QVector<int>* MainWindow::toMesh()
{
    static int count = 0;
    static int parent;
    QVector<int>* pairs = new QVector<int>();
    qDebug()<< "count " << VertIdList << endl;

    QStack<int> parents;

    int k = 0;
    for(QVector<QString>::iterator it = VertIdList.begin(); it != VertIdList.end(); it++)
    {
        if(it->compare("[")!=0 && it->compare("]") != 0)
        {
            parent = find_parent(&parents);
            //qDebug() << "pere et fils " << parent << " et " << it->toInt() << endl;
            pairs->append(parent);
            pairs->append(it->toInt());

            if(VertIdList.at(k-1).compare("[")==0)
            {
                parents.push(VertIdList.at(k).toInt());
                //qDebug() << "crochet ouvrant " <<parents << endl;
            }

            else if(VertIdList.at(k-1).compare("]")==0)
            {
                parents.push(VertIdList.at(k).toInt());
            }

            else if(VertIdList.at(k-1).compare("]")!=0 && VertIdList.at(k-1).compare("[")!=0)
            {
                if(!parents.empty())
                    parents.pop();
                parents.push(VertIdList.at(k).toInt());
                //qDebug() << "nombre " << parents << endl;
            }

        }

        else if(it->compare("]") == 0)
        {
            count--;
            if(!parents.empty() && VertIdList.at(k-1).compare("]")!=0 && VertIdList.at(k-1).compare("[")!=0)
                parents.pop();
        }

        else
        {
            count++;
        }

        k++;
    }
    return pairs;
}

// charge un objet MyMesh dans l'environnement OpenGL
void MainWindow::displayMesh(MyMesh* _mesh, bool isTemperatureMap, float mapRange)
{
    GLuint* triIndiceArray = new GLuint[_mesh->n_faces() * 3];
    GLfloat* triCols = new GLfloat[_mesh->n_faces() * 3 * 3];
    GLfloat* triVerts = new GLfloat[_mesh->n_faces() * 3 * 3];

    int i = 0;

    if(isTemperatureMap)
    {
        QVector<float> values;

        if(mapRange == -1)
        {
            for (MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++)
                values.append(fabs(_mesh->data(*curVert).value));
            qSort(values);
            mapRange = values.at(values.size()*0.8);
            qDebug() << "mapRange" << mapRange;
        }

        float range = mapRange;
        MyMesh::ConstFaceIter fIt(_mesh->faces_begin()), fEnd(_mesh->faces_end());
        MyMesh::ConstFaceVertexIter fvIt;

        for (; fIt!=fEnd; ++fIt)
        {
            fvIt = _mesh->cfv_iter(*fIt);
            if(_mesh->data(*fvIt).value > 0){triCols[3*i+0] = 255; triCols[3*i+1] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+2] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            else{triCols[3*i+2] = 255; triCols[3*i+1] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+0] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            if(_mesh->data(*fvIt).value > 0){triCols[3*i+0] = 255; triCols[3*i+1] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+2] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            else{triCols[3*i+2] = 255; triCols[3*i+1] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+0] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            if(_mesh->data(*fvIt).value > 0){triCols[3*i+0] = 255; triCols[3*i+1] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+2] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            else{triCols[3*i+2] = 255; triCols[3*i+1] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+0] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++;
        }
    }
    else
    {
        MyMesh::ConstFaceIter fIt(_mesh->faces_begin()), fEnd(_mesh->faces_end());
        MyMesh::ConstFaceVertexIter fvIt;
        for (; fIt!=fEnd; ++fIt)
        {
            fvIt = _mesh->cfv_iter(*fIt);
            triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++;
        }
    }


    ui->displayWidget->loadMesh(triVerts, triCols, _mesh->n_faces() * 3 * 3, triIndiceArray, _mesh->n_faces() * 3);

    delete[] triIndiceArray;
    delete[] triCols;
    delete[] triVerts;

    GLuint* linesIndiceArray = new GLuint[_mesh->n_edges() * 2];
    GLfloat* linesCols = new GLfloat[_mesh->n_edges() * 2 * 3];
    GLfloat* linesVerts = new GLfloat[_mesh->n_edges() * 2 * 3];

    i = 0;
    QHash<float, QList<int> > edgesIDbyThickness;
    for (MyMesh::EdgeIter eit = _mesh->edges_begin(); eit != _mesh->edges_end(); ++eit)
    {
        float t = _mesh->data(*eit).thickness;
        if(t > 0)
        {
            if(!edgesIDbyThickness.contains(t))
                edgesIDbyThickness[t] = QList<int>();
            edgesIDbyThickness[t].append((*eit).idx());
        }
    }
    QHashIterator<float, QList<int> > it(edgesIDbyThickness);
    QList<QPair<float, int> > edgeSizes;
    while (it.hasNext())
    {
        it.next();

        for(int e = 0; e < it.value().size(); e++)
        {
            int eidx = it.value().at(e);

            MyMesh::VertexHandle vh1 = _mesh->to_vertex_handle(_mesh->halfedge_handle(_mesh->edge_handle(eidx), 0));
            linesVerts[3*i+0] = _mesh->point(vh1)[0];
            linesVerts[3*i+1] = _mesh->point(vh1)[1];
            linesVerts[3*i+2] = _mesh->point(vh1)[2];
            linesCols[3*i+0] = _mesh->color(_mesh->edge_handle(eidx))[0];
            linesCols[3*i+1] = _mesh->color(_mesh->edge_handle(eidx))[1];
            linesCols[3*i+2] = _mesh->color(_mesh->edge_handle(eidx))[2];
            linesIndiceArray[i] = i;
            i++;

            MyMesh::VertexHandle vh2 = _mesh->from_vertex_handle(_mesh->halfedge_handle(_mesh->edge_handle(eidx), 0));
            linesVerts[3*i+0] = _mesh->point(vh2)[0];
            linesVerts[3*i+1] = _mesh->point(vh2)[1];
            linesVerts[3*i+2] = _mesh->point(vh2)[2];
            linesCols[3*i+0] = _mesh->color(_mesh->edge_handle(eidx))[0];
            linesCols[3*i+1] = _mesh->color(_mesh->edge_handle(eidx))[1];
            linesCols[3*i+2] = _mesh->color(_mesh->edge_handle(eidx))[2];
            linesIndiceArray[i] = i;
            i++;
        }
        edgeSizes.append(qMakePair(it.key(), it.value().size()));
    }

    ui->displayWidget->loadLines(linesVerts, linesCols, i * 3, linesIndiceArray, i, edgeSizes);

    delete[] linesIndiceArray;
    delete[] linesCols;
    delete[] linesVerts;

    GLuint* pointsIndiceArray = new GLuint[_mesh->n_vertices()];
    GLfloat* pointsCols = new GLfloat[_mesh->n_vertices() * 3];
    GLfloat* pointsVerts = new GLfloat[_mesh->n_vertices() * 3];

    i = 0;
    QHash<float, QList<int> > vertsIDbyThickness;
    for (MyMesh::VertexIter vit = _mesh->vertices_begin(); vit != _mesh->vertices_end(); ++vit)
    {
        float t = _mesh->data(*vit).thickness;
        if(t > 0)
        {
            if(!vertsIDbyThickness.contains(t))
                vertsIDbyThickness[t] = QList<int>();
            vertsIDbyThickness[t].append((*vit).idx());
        }
    }
    QHashIterator<float, QList<int> > vitt(vertsIDbyThickness);
    QList<QPair<float, int> > vertsSizes;

    while (vitt.hasNext())
    {
        vitt.next();

        for(int v = 0; v < vitt.value().size(); v++)
        {
            int vidx = vitt.value().at(v);

            pointsVerts[3*i+0] = _mesh->point(_mesh->vertex_handle(vidx))[0];
            pointsVerts[3*i+1] = _mesh->point(_mesh->vertex_handle(vidx))[1];
            pointsVerts[3*i+2] = _mesh->point(_mesh->vertex_handle(vidx))[2];
            pointsCols[3*i+0] = _mesh->color(_mesh->vertex_handle(vidx))[0];
            pointsCols[3*i+1] = _mesh->color(_mesh->vertex_handle(vidx))[1];
            pointsCols[3*i+2] = _mesh->color(_mesh->vertex_handle(vidx))[2];
            pointsIndiceArray[i] = i;
            i++;
        }
        vertsSizes.append(qMakePair(vitt.key(), vitt.value().size()));
    }

    ui->displayWidget->loadPoints(pointsVerts, pointsCols, i * 3, pointsIndiceArray, i, vertsSizes);

    delete[] pointsIndiceArray;
    delete[] pointsCols;
    delete[] pointsVerts;
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEditA->setText(caseA);
    ui->lineEditF->setText(caseF);
    ui->lineEditL->setText(caseL);
    ui->lineEditS->setText(caseS);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::generer_mot(QString source)
{
    QString mottmp;
    for (int i = 0; i < source.size() ; i++){
        switch(source.at(i).unicode()){
        case 'A' :
            mottmp.append(caseA);
            break ;
        case 'F' :
            mottmp.append(caseF);
            break ;
        case 'S' :
            mottmp.append(caseS);
            break ;
        case 'L' :
            mottmp.append(caseL);
            break ;
        default :
            mottmp.append(mot.at(i));
            break;
        }
    }
//    qDebug() << mottmp ;
    return mottmp;
}

void MainWindow::on_lineEditA_textEdited(const QString &arg1)
{
    caseA = arg1;

    generer();
}

void MainWindow::on_lineEditF_textEdited(const QString &arg1)
{
    caseF = arg1;

    generer();
}

void MainWindow::on_lineEditS_textEdited(const QString &arg1)
{
    caseS = arg1;

    generer();
}

void MainWindow::on_lineEditL_textEdited(const QString &arg1)
{
    caseL = arg1;

    generer();
}

void MainWindow::on_iterationsSlider_valueChanged(int value)
{
    iteration_number = value;
    ui->iterationsLabel->setText("Nombre d'itérations: "+QVariant(iteration_number).toString());

    generer();
}

void MainWindow::on_angleXSlider_valueChanged(int value)
{
    angleX = value;
    ui->angleXLabel->setText("Angle X: "+QVariant(angleX).toString());

    generer();
}

void MainWindow::on_angleYSlider_valueChanged(int value)
{
    angleY = value;
    ui->angleYLabel->setText("Angle Y: "+QVariant(angleY).toString());

    generer();
}

void MainWindow::on_angleZSlider_valueChanged(int value)
{
    angleZ = value;
    ui->angleZLabel->setText("Angle Z: "+QVariant(angleZ).toString());

    generer();
}

void MainWindow::on_valAngleSlider_valueChanged(int value)
{
    valAngle = value;
    ui->valAngleLabel->setText("ValAngle: "+QVariant(valAngle).toString());

    generer();
}

void MainWindow::on_distSlider_valueChanged(int value)
{
    dist = static_cast<float>(value)/50;
    ui->distLabel->setText("Dist: "+QVariant(dist).toString());

    generer();
}

void MainWindow::on_dlSlider_valueChanged(int value)
{
    dL = value;
    ui->dlLabel->setText("dL: "+QVariant(dL).toString());

    generer();
}

void MainWindow::on_wSlider_valueChanged(int value)
{
    w = value;
    ui->wLabel->setText("w: "+QVariant(w).toString());

    generer();
}

void MainWindow::generer()
{
    mot = "A";
    for(int i = 0; i<iteration_number; i++)
        mot = generer_mot(mot);

    mesh.clear();
    Turtle t = Turtle(0,0,0,angleX*M_PI/180,angleY*M_PI/180,angleZ*M_PI/180,dist,valAngle*M_PI/180);
    t.translateString(mot,&mesh,&VertIdList);
    qDebug() << "Generer " << VertIdList;
    mesh.update_normals();

    // initialisation des couleurs et épaisseurs (sommets et arêtes) du mesh
    resetAllColorsAndThickness(&mesh);


    // on affiche le maillage
    displayMesh(&mesh);
}

//Fonctions Frustum - Génération de maillage

MyMesh::VertexHandle point_to_vertex(MyMesh *_mesh, float x, float y, float z)
{
    return _mesh->add_vertex(MyMesh::Point(x, y,  z));
}

QVector<MyMesh::VertexHandle> points_into_mesh(MyMesh *_mesh, QVector<QVector3D> points)
{
    QVector<MyMesh::VertexHandle> vertices_vector;
    for(int i = 0 ; i < points.size() ; i++)
    {
        vertices_vector.append(point_to_vertex(_mesh, points.data()[i].x(), points.data()[i].y(), points.data()[i].z()));
    }

    return vertices_vector;
}


/*
 *
 * http://gilles.dubois10.free.fr/geometrie_affine/espacereglees.html
 *
 */

QVector3D parametric_frustum_point(float h, float r, float s, float t)
{
    QVector3D point;
    float x = r * cos(t*2*M_PI);
    float y = r * sin(t*2*M_PI);
    float z = h*s; //hauteur * paramètre s (s = 0, z = 0, s = 1, z =0)
    //qDebug() << " coord (" << x << ", " << y << ", " << z << ")" << endl;
    point.setX(x);
    point.setY(y);
    point.setZ(z);

    return point;
}


//Si coef_radius == 1 alors on dessine un cylindre,
//sinon si coef_radius == 0 alors on dessine un cône
//sinon entre ]0, 1[ un frustum (tronc de cone)

QVector<QVector3D> parametric_frustum(float x, float y, float z, float high, float radius, float coef_radius, float step_r, float step_s, float step_t)
{
    QVector<QVector3D> frustum_points;
    QVector3D start(x, y, z);

    float r = radius;
    float min_radius = radius*coef_radius;


    for(float s = 0; s <= 1 ; s += step_s) //hauteur
    {
        if( r > min_radius)
        {
            r = radius-radius*s;
        }


        for(float t = 0; t <= 1; t+= step_t) //position sur le cercle
        {
            frustum_points.append(start+parametric_frustum_point(high, r, s, t));
        }

    }

    return frustum_points;
}


float angleEE(QVector3D from, QVector3D to)
{
    QVector3D horizontal(0, 1, 0);
    QVector3D u(horizontal-from);
    QVector3D v(to-from);

    u.normalize();
    v.normalize();
    //MyMesh::Point p0 = _mesh->point(vex0);

    //MyMesh::Point u = points[0] - p0;
    //MyMesh::Point v = points[1] - p0;
    //v.normalize();
   // u.normalize();

    return acos(QVector3D::dotProduct(u,v));
}


MyMesh* MainWindow::frustum_into_mesh(float xA, float yA, float zA,
                                      float xB, float yB, float zB,
                                      float radius, float coef_radius,
                                      float step_r, float step_s, float step_t)
{
    MyMesh *_mesh = new MyMesh() ;
    float x = xB - xA;
    float y = yB - yA;
    float z = zB - zA;

    float high_AB = sqrt(x*x + y*y + z*z);
    QVector<QVector3D> frustum_points = parametric_frustum(xA, yA, zA, high_AB, radius, coef_radius, step_r, step_s, step_t);

    QVector<MyMesh::VertexHandle> vertices_vec = points_into_mesh(_mesh, frustum_points);

    std::vector<MyMesh::VertexHandle> newFace;

    float nS = 1.f/step_s;
    float mT = 1.f/step_t;

    for(int h = 1 ; h < nS; h++)
    {
        for(int p = 1 ; p <= mT; p++)
        {
            if(p >= mT)
            {
                newFace.clear();
                newFace.push_back(vertices_vec[h*mT+0]);
                newFace.push_back(vertices_vec[h*mT+(mT-1)]);
                newFace.push_back(vertices_vec[(h-1)*mT+(mT-1)]);
                _mesh->add_face(newFace);

                newFace.clear();
                newFace.push_back(vertices_vec[h*mT+0]);
                newFace.push_back(vertices_vec[(h-1)*mT+(mT-1)]);
                newFace.push_back(vertices_vec[(h-1)*mT+0]);
                _mesh->add_face(newFace);
            }

            else {
                //1er demi-carré
                newFace.clear();
                newFace.push_back(vertices_vec[h*mT+p]);
                newFace.push_back(vertices_vec[h*mT+(p-1)]);
                newFace.push_back(vertices_vec[(h-1)*mT+(p-1)]);
                _mesh->add_face(newFace);

                //2ème demi-carré
                newFace.clear();
                newFace.push_back(vertices_vec[h*mT+p]);
                newFace.push_back(vertices_vec[(h-1)*mT+(p-1)]);
                newFace.push_back(vertices_vec[(h-1)*mT+p]);
                _mesh->add_face(newFace);
            }
        }
    }

    return _mesh;
}


void MainWindow::frustum_into_mesh(MyMesh* _mesh, float xA, float yA, float zA,
                                      float xB, float yB, float zB,
                                      float radius, float coef_radius,
                                      float step_r, float step_s, float step_t)
{
    float x = xB - xA;
    float y = yB - yA;
    float z = zB - zA;

    float high_AB = sqrt(x*x + y*y + z*z);
    QVector<QVector3D> frustum_points = parametric_frustum(x, y, z, high_AB, radius, coef_radius, step_r, step_s, step_t);

    QVector<MyMesh::VertexHandle> vertices_vec = points_into_mesh(_mesh, frustum_points);

    std::vector<MyMesh::VertexHandle> newFace;

    float nS = 1.f/step_s;
    float mT = 1.f/step_t;

    for(int h = 1 ; h < nS; h++)
    {
        for(int p = 1 ; p <= mT; p++)
        {
            if(p >= mT)
            {
                newFace.clear();
                newFace.push_back(vertices_vec[h*mT+0]);
                newFace.push_back(vertices_vec[h*mT+(mT-1)]);
                newFace.push_back(vertices_vec[(h-1)*mT+(mT-1)]);
                _mesh->add_face(newFace);

                newFace.clear();
                newFace.push_back(vertices_vec[h*mT+0]);
                newFace.push_back(vertices_vec[(h-1)*mT+(mT-1)]);
                newFace.push_back(vertices_vec[(h-1)*mT+0]);
                _mesh->add_face(newFace);
            }

            else {
                //1er demi-carré
                newFace.clear();
                newFace.push_back(vertices_vec[h*mT+p]);
                newFace.push_back(vertices_vec[h*mT+(p-1)]);
                newFace.push_back(vertices_vec[(h-1)*mT+(p-1)]);
                _mesh->add_face(newFace);

                //2ème demi-carré
                newFace.clear();
                newFace.push_back(vertices_vec[h*mT+p]);
                newFace.push_back(vertices_vec[(h-1)*mT+(p-1)]);
                newFace.push_back(vertices_vec[(h-1)*mT+p]);
                _mesh->add_face(newFace);
            }
        }
    }
}



//Fonctions

void motToMesh();

//



