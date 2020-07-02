#include "MatrixStack.h"

MatrixStack::MatrixStack()
{
	// Initialize the matrix stack with the identity matrix.
	m_matrices.push_back(Matrix4f::identity());
}

void MatrixStack::clear()
{
	// Revert to just containing the identity matrix.
	m_matrices.clear();
	m_matrices.push_back(Matrix4f::identity());
}

Matrix4f MatrixStack::top()
{
	// Return the top of the stack
	return m_matrices.back();
}

void MatrixStack::push( const Matrix4f& m )
{
	m_matrices.push_back(top() * m);
}

void MatrixStack::pop()
{
	// Remove the top element from the stack
	m_matrices.pop_back();
}
