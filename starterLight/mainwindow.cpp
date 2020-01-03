#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "turtle.h"
#include <stdlib.h>

/* **** début de la partie boutons et IHM **** */


// exemple pour charger un fichier .obj
void MainWindow::on_pushButton_chargement_clicked()
{
    // fenêtre de sélection des fichiers
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Mesh"), "", tr("Mesh Files (*.obj)"));

    // chargement du fichier .obj dans la variable globale "mesh"
    OpenMesh::IO::read_mesh(mesh, fileName.toUtf8().constData());

    mesh.update_normals();

    // initialisation des couleurs et épaisseurs (sommets et arêtes) du mesh
    resetAllColorsAndThickness(&mesh);

    // on affiche le maillage
    displayMesh(&mesh);
}

// exemple pour construire un mesh face par face
void MainWindow::on_pushButton_generer_clicked()
{
    Turtle t = Turtle(0,0,0,22,15,20,1);
    t.translateString(mot,&mesh,&VertIdList);

    MyMesh *new_mesh = new MyMesh();

    // on construit une liste de sommets

    QVector<int> *pairs = toMesh();

    qDebug() << "Aloha " << endl;

    qDebug() << pairs->count() << endl;

    for(int i = 1; i < pairs->count(); i +=2)
    {
        MyMesh::Point from, to;
        from = mesh.point(VertexHandle(pairs->data()[i-1]));
        to = mesh.point(VertexHandle(pairs->data()[i]));

        qDebug() << "From " << from[0] << ", " << from[1] << ", " << from[2] << endl;
        qDebug() << "to " << to[0] << ", " << to[1] << ", " << to[2] << endl;


        frustum_into_mesh(new_mesh,
                          from[0], from[1], from[2],
                          to[0], to[1], to[2],
                          0.5f, 0,
                          0.05f, 0.07f, 0.04f);
    }

/* Test des 2 fonctions polymorphes OK

    //règles utilisées :
    //
    //QString mot = "A";
    //QString caseA = "F&F[+F]F";
    //QString caseF = "F^[F-]";
    //QString caseS = "FL";
    //QString caseL = "[’’’∧∧{-f+f+f-|-f+f+f}]";


    new_mesh = frustum_into_mesh(0.672423f ,  -0.57559f ,  -75.9688f,
                                 1.53848f ,  -0.758341f ,  -173.814f,
                                 1, 0,
                                 0.05f, 0.07f, 0.04f);


    frustum_into_mesh(new_mesh,
                      1.53848f ,  -0.758341f ,  -173.814f,
                      49.2062f ,  -10.8169f ,  -271.659f,
                      2, 0,
                      0.05f, 0.07f, 0.04f);
 */
    new_mesh->update_normals();
    resetAllColorsAndThickness(new_mesh);
    displayMesh(new_mesh);
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

int find_parent(QStack<unsigned int> *parents)
{
    return parents->top();
}

QVector<int>* MainWindow::toMesh()
{
    static int parent;
    QVector<int>* pairs = new QVector<int>();
    qDebug()<< "count " << VertIdList.count() << endl;

    QStack<unsigned int> parents;
    parents.push(VertIdList.at(0).toInt());

    int k = 1;
    for(QVector<QString>::iterator it = VertIdList.begin()+1; it != VertIdList.end(); it++)
    {
        if(it->compare("[")!=0 && it->compare("]") != 0)
        {
            parent = find_parent(&parents);

            qDebug() << "pere et fils " << parent << " et " << it->toInt() << endl;
            pairs->append(parent);
            pairs->append(it->toInt());

            if(VertIdList.at(k-1).compare("[")==0)
            {
                parents.push(VertIdList.at(k).toInt());
            }

            else
            {
                parents.push(VertIdList.at(k).toInt());
            }
        }

        else if(it->compare("]") == 0)
        {
            if(!parents.empty())
                parents.pop();
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generer_mot()
{
    QString mottmp;
    for (int i = 0; i < mot.size() ; i++){
        switch(mot.at(i).unicode()){
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
    mot = mottmp;
    qDebug() << mot ;
}



void MainWindow::on_pushButton_clicked()
{
    generer_mot();
}

void MainWindow::on_lineEditA_textEdited(const QString &arg1)
{
    caseA = arg1;
}

void MainWindow::on_lineEditF_textEdited(const QString &arg1)
{
    caseF = arg1;
}

void MainWindow::on_lineEditS_textEdited(const QString &arg1)
{
    caseS = arg1;
}

void MainWindow::on_lineEditL_textEdited(const QString &arg1)
{
    caseL = arg1;
}

void MainWindow::on_pushButton_Reset_clicked()
{
    mot = "A";
}


//Fonctions Frustum - Génération de maillage

MyMesh::VertexHandle point_to_vertex(MyMesh *_mesh, float x, float y, float z)
{
    return _mesh->add_vertex(MyMesh::Point(x, y,  z));
}

QVector<MyMesh::VertexHandle> points_into_mesh(MyMesh *_mesh, QVector<float> points)
{
    QVector<MyMesh::VertexHandle> vertices_vector;
    for(int i = 0 ; i < points.size() ; i+=3)
    {
        vertices_vector.append(point_to_vertex(_mesh, points.data()[i], points.data()[i+1], points.data()[i+2]));
    }

    return vertices_vector;
}


/*
 *
 * http://gilles.dubois10.free.fr/geometrie_affine/espacereglees.html
 *
 */

QVector<float> parametric_frustum_point(float h, float r, float s, float t)
{
    QVector<float> point;
    float x = r * cos(t*2*M_PI);
    float y = r * sin(t*2*M_PI);
    float z = h*s; //hauteur * paramètre s (s = 0, z = 0, s = 1, z =0)

    point.push_back(x);
    point.push_back(y);
    point.push_back(z);

    return point;
}


//Si coef_radius == 1 alors on dessine un cylindre,
//sinon si coef_radius == 0 alors on dessine un cône
//sinon entre ]0, 1[ un frustum (tronc de cone)

QVector<float> parametric_frustum(float high, float radius, float coef_radius, float step_r, float step_s, float step_t)
{
    QVector<float> frustum_points;
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
            frustum_points.append(parametric_frustum_point(high, r, s, t));
        }

    }
    return frustum_points;
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
    QVector<float> frustum_points = parametric_frustum(high_AB, radius, coef_radius, step_r, step_s, step_t);

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
    QVector<float> frustum_points = parametric_frustum(high_AB, radius, coef_radius, step_r, step_s, step_t);

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
void MainWindow::on_pushButton_2_clicked()
{
    Turtle t = Turtle(0,0,0,22,15,20,1);
    t.translateString(mot,&mesh,&VertIdList);
    qDebug() << VertIdList;
    mesh.update_normals();

    // initialisation des couleurs et épaisseurs (sommets et arêtes) du mesh
    resetAllColorsAndThickness(&mesh);


    // on affiche le maillage
    displayMesh(&mesh);
}
