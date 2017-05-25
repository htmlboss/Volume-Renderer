#include "window.h"

#include <QDebug>

/***********************************************************************************/
Window::Window() Q_DECL_NOEXCEPT {
	m_transform.translate(0.0f, 0.0f, -5.0f);
}

/***********************************************************************************/
Window::~Window() {
	makeCurrent();
	teardownGL();
}

/***********************************************************************************/
void Window::initializeGL() {
	initializeOpenGLFunctions();
	connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
	printContextInfo();

	glEnable(GL_CULL_FACE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_program.reset(new QOpenGLShaderProgram());
	m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic.vert");
	m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic.frag");
	if (!m_program->link()) {
		throw std::runtime_error(m_program->log().toStdString());
	}
	m_program->bind();

	u_modelToWorld = m_program->uniformLocation("modelToWorld");
	u_worldToView = m_program->uniformLocation("worldToView");

	m_vbo.create();
	m_vbo.bind();
	m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_vbo.allocate(cube, sizeof(cube));

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

	m_projection.setToIdentity();
	m_projection.perspective(45.0f, width / float(height), 0.0f, 1000.0f);
}

/***********************************************************************************/
void Window::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();
	m_program->setUniformValue(u_worldToView, m_projection);
	{
		m_vao.bind();
		m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
		glDrawArrays(GL_TRIANGLES, 0, sizeof(cube) / sizeof(cube[0]));
	}
}

/***********************************************************************************/
void Window::teardownGL() {
	m_vbo.destroy();
	m_vao.destroy();
}

/***********************************************************************************/
void Window::update() {
	m_transform.rotate(1.0f, QVector3D(0.4f, 0.3f, 0.3f));

	// Force update
	QOpenGLWindow::update();
}

/***********************************************************************************/
void Window::printContextInfo() {
	QString glType, glVersion, glslVersion, glProfile;

	// Get Version Information
	glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
	glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	glslVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
	switch (format().profile()) {
	   CASE(NoProfile);
	   CASE(CoreProfile);
	   CASE(CompatibilityProfile);
	 }
#undef CASE

	 // qPrintable() will print our QString w/o quotes around it.
	 qDebug() << qPrintable(glType) << qPrintable(glVersion) << " " << qPrintable(glslVersion) << " (" << qPrintable(glProfile) << ")";
}
