#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

#include <QOpenGLShaderProgram>

class VolumeRenderer {
public:
	VolumeRenderer();

	void Update();
	void Render(const QMatrix4x4& viewMatrx, const QMatrix4x4& cameraMatrix, const QMatrix4x4& projMatrix);

private:
	QOpenGLShaderProgram m_volumeShader;
};

Q_DECLARE_TYPEINFO(VolumeRenderer, Q_MOVABLE_TYPE);

#endif // VOLUMERENDERER_H
