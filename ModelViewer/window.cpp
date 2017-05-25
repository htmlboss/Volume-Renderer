#include "window.h"

#include <QDebug>QOpenGLShaderProgram

/***********************************************************************************/
Window::Window() Q_DECL_NOEXCEPT {
}

/***********************************************************************************/
Window::~Window() {
	makeCurrent();
	teardownGL();
}

/***********************************************************************************/
void Window::initializeGL() {
	initializeOpenGLFunctions();
	printContextInfo();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_program = new QOpenGLShaderProgram();
	m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic.vert");
	m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic.frag");
	if (!m_program->link()) {
		qDebug() << m_program->log();
	}
	m_program->bind();

	m_vbo.create();
	m_vbo.bind();
	m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_vbo.allocate(m_triangle.data(), sizeof(m_triangle));

	m_vao.create();
	m_vao.bind();

	m_program->enableAttributeArray(0);
	m_program->enableAttributeArray(1);
	m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), 3, Vertex::sizeOf());
	m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), 3, Vertex::sizeOf());

	m_vbo.release();
	m_vao.release();
	m_program->release();
}

/***********************************************************************************/
void Window::resizeGL(int width, int height) {
	qDebug() << "Window resized to: " << width << " " << height;
}

/***********************************************************************************/
void Window::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();
	{
		m_vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, m_triangle.size());
	}
}

/***********************************************************************************/
void Window::teardownGL() {
	m_vbo.destroy();
	m_vao.destroy();
	delete m_program;
}

/***********************************************************************************/
void Window::printContextInfo() {
	QString glType, glVersion, glProfile;

	// Get Version Information
	glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
	glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

	// Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
	switch (format().profile()) {
	   CASE(NoProfile);
	   CASE(CoreProfile);
	   CASE(CompatibilityProfile);
	 }
#undef CASE

	 // qPrintable() will print our QString w/o quotes around it.
	 qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}
