#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#include <vector>
#include <vecmath.h>
#ifdef WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

class MatrixStack
{
public:
	MatrixStack();
	void clear();
	Matrix4f top();
	void push( const Matrix4f& m );
	void pop();
	//Matrix4f get(int index) { return m_matrices[index]; }

private:
	std::vector< Matrix4f > m_matrices;

};

#endif // MATRIX_STACK_H
