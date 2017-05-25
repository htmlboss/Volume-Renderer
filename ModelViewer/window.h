#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "vertex.h"

// Use STL unique_ptr since QScopedPtr doesn't support moving.
#include <memory>
#include <array>

class Window : public QOpenGLWindow, protected QOpenGLFunctions {
	Q_OBJECT
public:
	Window() Q_DECL_NOEXCEPT;
	~Window();

	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;

	// Clean up OpenGL
	void teardownGL();

private:
	// Print OpenGL context info
	void printContextInfo();

	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo;
	QOpenGLShaderProgram* m_program;

	const std::array<Vertex, 6> m_triangle {
		Vertex( QVector3D( 0.00f,  0.75f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f) ),
		Vertex( QVector3D( 0.75f, -0.75f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f) ),
		Vertex( QVector3D(-0.75f, -0.75f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f) )
	};
};

#endif // WINDOW_H
