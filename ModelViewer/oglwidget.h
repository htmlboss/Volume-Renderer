#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QScopedPointer>

#include "vertex.h"
#include "camera.h"

#include <array>

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core {
	Q_OBJECT
public:
	OGLWidget(QWidget* parent = nullptr);
	~OGLWidget();

	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;

	// Clean up OpenGL
	void teardownGL();

	void SetInvertedAxis(const bool val);
	void SetMouseSensitivity(const double val);

protected slots:
	void update();

protected:
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
	// Print OpenGL context info
	void printContextInfo();

	void render(const GLenum cullFace);

	Camera m_camera;
	GLuint u_modelToWorld, u_worldToCamera, u_cameraToView;

	int_fast8_t m_axisInversion;
	double m_rotationSpeed;

	QMatrix4x4 m_projection;
	Transform3D m_transform;

	// Volume rendering
	GLuint m_cubeVAO, m_cubeVBO, m_cubeEBO, m_transferFuncTex, m_backfaceTex, m_volumeDataTex, m_volumeFBO;
	QScopedPointer<QOpenGLShaderProgram> m_backfaceShader, m_raycastShader;
	GLuint m_bfMVPloc, m_rcMVPloc, m_rcScreenSizeloc, m_rcStepSizeloc, m_rcTransFuncloc, m_rcVolumeloc, m_rcExitPointsloc;

	const std::array<GLfloat, 24> m_cubeVertices {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f
	};
	const std::array<GLuint, 36> m_cubeIndices {
		1,5,7,
		7,3,1,
		0,2,6,
		6,4,0,
		0,1,3,
		3,2,0,
		7,5,4,
		4,6,7,
		2,3,7,
		7,6,2,
		1,0,4,
		4,5,1
	};
};

#endif // OGLWIDGET_H
