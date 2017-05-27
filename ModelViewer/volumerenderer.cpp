#include "volumerenderer.h"

/***********************************************************************************/
VolumeRenderer::VolumeRenderer() {

	m_volumeShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic.vert");
	m_volumeShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic.frag");
	if (!m_volumeShader.link()) {
		qDebug() << m_volumeShader.log();
	}
}

/***********************************************************************************/
void VolumeRenderer::Render(const QMatrix4x4& viewMatrx, const QMatrix4x4& cameraMatrix, const QMatrix4x4& projMatrix) {
	m_volumeShader.bind();
}
