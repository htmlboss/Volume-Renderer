#include "oglwidget.h"

#include "input.h"

#include <QDebug>
#include <QMouseEvent>
#include <QImage>
#include <fstream>
#include <QDir>

/***********************************************************************************/
OGLWidget::OGLWidget(QWidget* parent) : m_axisInversion(1), m_rotationSpeed(0.25f) {
	m_transform.translate(0.0f, 0.0f, -4.0f);

	QSurfaceFormat fmt;
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
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
	using namespace std;

	initializeOpenGLFunctions();
	connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
	printContextInfo();

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glGenVertexArrays(1, &m_cubeVAO);
	glBindVertexArray(m_cubeVAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_cubeVertices), m_cubeVertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_cubeIndices), m_cubeIndices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Read in transfer function
	ifstream inFile("res/tff.dat", ifstream::in);
	if (!inFile) {
		qDebug() << "Error opening file tff.dat. " << QDir::currentPath();
	}
	constexpr auto MAX_CNT = 100000000;
	GLubyte* tff = (GLubyte *) calloc(MAX_CNT, sizeof(GLubyte));
	inFile.read(reinterpret_cast<char *>(tff), MAX_CNT);

	if (inFile.eof()) {
		size_t bytecnt = inFile.gcount();
		*(tff + bytecnt) = '\0';
		qDebug() << "bytecnt " << bytecnt;
	}
	else if(inFile.fail()) {
		qDebug() << "tff.dat" << "read failed.";
	}
	else {
		qDebug() << "tff.dat" << "is too large." << *tff;
	}

	glGenTextures(1, &m_transferFuncTex);
	glBindTexture(GL_TEXTURE_1D, m_transferFuncTex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tff);

	inFile.close();
	free(tff);

	glGenTextures(1, &m_backfaceTex);
	glBindTexture(GL_TEXTURE_2D, m_backfaceTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->width(), this->height(), 0, GL_RGBA, GL_FLOAT, nullptr);

	FILE* fp;
	constexpr size_t size = 256 * 256 * 225;
	std::vector<GLubyte> data(size); // 8bit
	if (!(fp = fopen("res/head256.raw", "rb"))) {
		qDebug() << "Error: opening .raw file failed.";
	}
	else {
		qDebug() << "OK: open .raw file successed";
	}

	const auto dat = fread(&data[0], sizeof(char), size, fp);
	if (dat != size) {
		qDebug() << "Error: read .raw file failed" << dat << " " << ferror(fp);
	}
	else {
		qDebug() << "OK: read .raw file successed";
	}
	fclose(fp);

	glGenTextures(1, &m_volumeDataTex);
	glBindTexture(GL_TEXTURE_3D, m_volumeDataTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, 256, 256, 225, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data.data());

	m_backfaceShader.reset(new QOpenGLShaderProgram());
	if (!m_backfaceShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/backface.vert")) {
		qDebug() << m_backfaceShader->log();
	}
	if (!m_backfaceShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/backface.frag")) {
		qDebug() << m_backfaceShader->log();
	}
	m_backfaceShader->enableAttributeArray(0);
	m_backfaceShader->enableAttributeArray(1);
	m_backfaceShader->bindAttributeLocation("VerPos", 0);
	m_backfaceShader->bindAttributeLocation("VerClr", 1);
	if (!m_backfaceShader->link()) {
		qDebug() << m_backfaceShader->log();
	}
	m_backfaceShader->bind();
	m_bfMVPloc = m_backfaceShader->uniformLocation("MVP");

	m_raycastShader.reset(new QOpenGLShaderProgram());
	if (!m_raycastShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/raycast.vert")) {
		qDebug() << m_raycastShader->log();
	}
	if (!m_raycastShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/raycast.frag")) {
		qDebug() << m_raycastShader->log();
	}
	m_raycastShader->enableAttributeArray(0);
	m_raycastShader->enableAttributeArray(1);
	m_raycastShader->bindAttributeLocation("VerPos", 0);
	m_raycastShader->bindAttributeLocation("VerClr", 1);
	if (!m_raycastShader->link()) {
		qDebug() << m_raycastShader->log();
	}
	m_raycastShader->bind();
	m_rcMVPloc = m_raycastShader->uniformLocation("MVP");
	m_rcScreenSizeloc = m_raycastShader->uniformLocation("ScreenSize");
	m_rcStepSizeloc = m_raycastShader->uniformLocation("StepSize");
	m_rcTransFuncloc = m_raycastShader->uniformLocation("TransferFunc");
	m_rcVolumeloc = m_raycastShader->uniformLocation("VolumeTex");
	m_rcExitPointsloc = m_raycastShader->uniformLocation("ExitPoints");

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width(), height());

	glGenFramebuffers(1, &m_volumeFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_volumeFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_backfaceTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	const auto complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (complete != GL_FRAMEBUFFER_COMPLETE) {
		qDebug() << "framebuffer is not complete";
	}
}

/***********************************************************************************/
void OGLWidget::resizeGL(int width, int height) {
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, width / float(height), 1.0f, 100.0f);

	glBindTexture(GL_TEXTURE_2D, m_backfaceTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

	glDeleteFramebuffers(1, &m_volumeFBO);
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	glGenFramebuffers(1, &m_volumeFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_volumeFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_backfaceTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	const auto complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (complete != GL_FRAMEBUFFER_COMPLETE) {
		qDebug() << "framebuffer is not complete";
	}
}

/***********************************************************************************/
void OGLWidget::paintGL() {

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_volumeFBO);
	m_backfaceShader->bind();
	const auto MVP = m_projection * m_camera.toMatrix() * m_transform.toMatrix();
	m_backfaceShader->setUniformValue(m_bfMVPloc, MVP);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glBindVertexArray(m_cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
	m_raycastShader->bind();

	m_raycastShader->setUniformValue(m_rcMVPloc, MVP);
	m_raycastShader->setUniformValue(m_rcScreenSizeloc, QVector2D(width(), height()));
	m_raycastShader->setUniformValue(m_rcStepSizeloc, 0.001f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, m_transferFuncTex);
	m_raycastShader->setUniformValue(m_rcTransFuncloc, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_backfaceTex);
	m_raycastShader->setUniformValue(m_rcExitPointsloc, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, m_volumeDataTex);
	m_raycastShader->setUniformValue(m_rcVolumeloc, 2);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glBindVertexArray(m_cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

/***********************************************************************************/
void OGLWidget::teardownGL() {
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
