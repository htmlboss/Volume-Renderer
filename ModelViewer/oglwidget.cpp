#include "oglwidget.h"

#include "input.h"

#include <QDebug>
#include <QMouseEvent>

/***********************************************************************************/
OGLWidget::OGLWidget(QWidget* parent) : m_axisInversion(1), m_rotationSpeed(0.25f) {
	m_transform.translate(0.0f, 0.0f, -5.0f);

	QSurfaceFormat fmt;
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	fmt.setVersion(4, 5);
	fmt.setSamples(8);
	setFormat(fmt);
}

/***********************************************************************************/
OGLWidget::~OGLWidget() {
	makeCurrent();
	teardownGL();
}

/***********************************************************************************/
void OGLWidget::initializeGL() {
	initializeOpenGLFunctions();
	connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
	printContextInfo();

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_program.reset(new QOpenGLShaderProgram());
	m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic.vert");
	m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic.frag");
	if (!m_program->link()) {
		qDebug() << m_program->log();
	}
	m_program->bind();

	u_modelToWorld = m_program->uniformLocation("modelToWorld");
	u_worldToCamera = m_program->uniformLocation("worldToCamera");
	u_cameraToView = m_program->uniformLocation("cameraToView");

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
void OGLWidget::resizeGL(int width, int height) {
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, width / float(height), 1.0f, 100.0f);
}

/***********************************************************************************/
void OGLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program->bind();
	m_program->setUniformValue(u_worldToCamera, m_camera.toMatrix());
	m_program->setUniformValue(u_cameraToView, m_projection);

	{
		m_vao.bind();
		m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
		glDrawArrays(GL_TRIANGLES, 0, sizeof(cube) / sizeof(cube[0]));
	}
}

/***********************************************************************************/
void OGLWidget::teardownGL() {
	m_vbo.destroy();
	m_vao.destroy();
}

/***********************************************************************************/
void OGLWidget::SetInvertedAxis(const bool val) {
	m_axisInversion = val ? -1 : 1;
}

/***********************************************************************************/
void OGLWidget::SetMouseSensitivity(const double val) {
	m_rotationSpeed = val;
}

/***********************************************************************************/
void OGLWidget::update() {
	Input::update();

	if (Input::buttonPressed(Qt::RightButton)) {
		m_transform.rotate(m_axisInversion * m_rotationSpeed * Input::mouseDelta().x(), m_camera.Up);
		m_transform.rotate(m_axisInversion * m_rotationSpeed * Input::mouseDelta().y(), m_camera.right());
	}

	// Force update
	QOpenGLWidget::update();
}

/***********************************************************************************/
void OGLWidget::mousePressEvent(QMouseEvent* event) {
	Input::registerMousePress(event->button());
}

/***********************************************************************************/
void OGLWidget::mouseReleaseEvent(QMouseEvent* event) {
	Input::registerMouseRelease(event->button());
}

/***********************************************************************************/
void OGLWidget::printContextInfo() {
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
