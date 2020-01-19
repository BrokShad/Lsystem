#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

enum Axe
{
    X_Axis,
    Y_Axis,
    Z_Axis
};

struct Point3D
{
	double x;
	double y;
	double z;

	void display()
	{
		cout << x << ":" << y << ":" << z << endl;
	}
};


struct Matrix
{
	int h;
	int w;
    vector<vector<double>> m;

	Matrix(int columns, int lines)
	{
		h = lines;
		w = columns;
		vector<double> v;
		for(int i=0; i < w; i++)
		{
			m.push_back(v);
			for(int j=0; j < h; j++)
			{
				m[i].push_back(0);
			}
		}
	}

	void operate(Matrix A, Matrix B, int column, int line)
	{
		this->m[column][line] = 0;
		for(int x = 0; x < A.w; x++)
		{
			this->m[column][line] += A.m[x][line]*B.m[column][x];
		}
	}

	void Point3D_to_Matrix(Point3D p)
	{
		m[0][0] = p.x;
		m[0][1] = p.y;
		m[0][2] = p.z;
		m[0][3] = 1;
	}

	void set(int column, int line, double value)
	{
		m[column][line] = value;
	}

	double get(int column, int line)
	{
		return m[column][line];
	}

	void display()
	{
		for(int i = 0; i < h; i++)
		{
			for(int j = 0; j < w; j++)
				cout << m[j][i] << " ";
			cout << endl;
		}
	}
};


Matrix matrix_product(Matrix matrix1, Matrix matrix2)
{
	int i,j;
	Matrix matrix3 = Matrix(matrix2.w,matrix1.h);

	for(i = 0; i < matrix1.h; i++)
	{
		for(j = 0; j < matrix2.w; j++)
		{
			matrix3.operate(matrix1, matrix2, j, i);
		}
	}
	//matrix3.display();
	return matrix3;
}


Matrix vectorize_point(Point3D p)
{
	Matrix matrix = Matrix(1,4);
	matrix.Point3D_to_Matrix(p);
	//matrix.display();
	return matrix;
}

Point3D devectorize_point(Matrix m)
{
	Point3D p;
	p.x = m.get(0,0)/m.get(0,3);
	p.y = m.get(0,1)/m.get(0,3);
	p.z = m.get(0,2)/m.get(0,3);
	return p;
}

Point3D rotate_spherical_point(Point3D p, Axe axe, double angle)
{
	Matrix point_matrix = vectorize_point(p);
	Matrix rotation_matrix = Matrix(4,4);
	switch(axe)
	{
		case X_Axis:
			rotation_matrix.set(0,0,1);	rotation_matrix.set(1,0,0);				rotation_matrix.set(2,0,0);				rotation_matrix.set(3,0,0);	
			rotation_matrix.set(0,1,0);	rotation_matrix.set(1,1,cos(angle));	rotation_matrix.set(2,1,-sin(angle));	rotation_matrix.set(3,1,0);	
			rotation_matrix.set(0,2,0);	rotation_matrix.set(1,2,sin(angle));	rotation_matrix.set(2,2,cos(angle));	rotation_matrix.set(3,2,0);	
			rotation_matrix.set(0,3,0);	rotation_matrix.set(1,3,0);				rotation_matrix.set(2,3,0);				rotation_matrix.set(3,3,1);	
			point_matrix = matrix_product(rotation_matrix,point_matrix);
			break;
		case Y_Axis:
			rotation_matrix.set(0,0,cos(angle));	rotation_matrix.set(1,0,0);	rotation_matrix.set(2,0,sin(angle));	rotation_matrix.set(3,0,0);	
			rotation_matrix.set(0,1,0);				rotation_matrix.set(1,1,1);	rotation_matrix.set(2,1,0);				rotation_matrix.set(3,1,0);	
			rotation_matrix.set(0,2,-sin(angle));	rotation_matrix.set(1,2,0);	rotation_matrix.set(2,2,cos(angle));	rotation_matrix.set(3,2,0);	
			rotation_matrix.set(0,3,0);				rotation_matrix.set(1,3,0);	rotation_matrix.set(2,3,0);				rotation_matrix.set(3,3,1);	
			point_matrix = matrix_product(rotation_matrix,point_matrix);
			break;
		case Z_Axis:
			rotation_matrix.set(0,0,cos(angle));	rotation_matrix.set(1,0,-sin(angle));	rotation_matrix.set(2,0,0);	rotation_matrix.set(3,0,0);	
			rotation_matrix.set(0,1,sin(angle));	rotation_matrix.set(1,1,cos(angle));	rotation_matrix.set(2,1,0);	rotation_matrix.set(3,1,0);	
			rotation_matrix.set(0,2,0);				rotation_matrix.set(1,2,0);				rotation_matrix.set(2,2,1);	rotation_matrix.set(3,2,0);	
			rotation_matrix.set(0,3,0);				rotation_matrix.set(1,3,0);				rotation_matrix.set(2,3,0);	rotation_matrix.set(3,3,1);	
			point_matrix = matrix_product(rotation_matrix,point_matrix);
			break;
	}



	// double x,y,z,w;
	// x = qAxis.x*sin((qD*M_PI/180)/2);
	// y = qAxis.y*sin((qD*M_PI/180)/2);
	// z = qAxis.z*sin((qD*M_PI/180)/2);
	// w = cos((qD*M_PI/180)/2);

	// rotation_matrix.set(0,0,1-2*y*y-2*z*z);	rotation_matrix.set(1,0,2*x*y+2*w*z); 	rotation_matrix.set(2,0,2*x*z-2*w*y);	rotation_matrix.set(3,0,0);
	// rotation_matrix.set(0,1,2*x*y-2*w*z);	rotation_matrix.set(1,1,1-2*x*x-2*z*z);	rotation_matrix.set(2,1,2*y*z+2*w*x);	rotation_matrix.set(3,1,0);
	// rotation_matrix.set(0,2,2*x*z+2*w*y);	rotation_matrix.set(1,2,2*y*z-2*w*x);	rotation_matrix.set(2,2,1-2*x*x-2*y*y); rotation_matrix.set(3,2,0);
	// rotation_matrix.set(0,3,0);				rotation_matrix.set(1,3,0);				rotation_matrix.set(2,3,0);				rotation_matrix.set(3,3,1);	

	// point_matrix = matrix_product(rotation_matrix,point_matrix);

	p = devectorize_point(point_matrix);
	return p;
}

Point3D translate_point(Point3D p, int tx, int ty, int tz)
{
	Matrix point_matrix = vectorize_point(p);
	Matrix translation_matrix = Matrix(4,4);

	translation_matrix.set(0,0,1);	translation_matrix.set(1,0,0);	translation_matrix.set(2,0,0);	translation_matrix.set(3,0,tx);	
	translation_matrix.set(0,1,0);	translation_matrix.set(1,1,1);	translation_matrix.set(2,1,0);	translation_matrix.set(3,1,ty);	
	translation_matrix.set(0,2,0);	translation_matrix.set(1,2,0);	translation_matrix.set(2,2,1);	translation_matrix.set(3,2,tz);	
	translation_matrix.set(0,3,0);	translation_matrix.set(1,3,0);	translation_matrix.set(2,3,0);	translation_matrix.set(3,3,1);	
	point_matrix = matrix_product(translation_matrix,point_matrix);

	p = devectorize_point(point_matrix);
	return p;
}

