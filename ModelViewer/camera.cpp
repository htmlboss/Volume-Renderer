#include "camera.h"

/***********************************************************************************/
void Camera::translate(const QVector3D& pos) {
	m_dirty = true;
	m_translation += pos;
}

/***********************************************************************************/
void Camera::rotate(const QQuaternion& rotate) {
	m_dirty = true;
	m_rotation = rotate * m_rotation;
}

/***********************************************************************************/
void Camera::setTranslation(const QVector3D& pos) {
	m_dirty = true;
	m_translation = pos;
}

/***********************************************************************************/
void Camera::setRotation(const QQuaternion& rotate) {
	m_dirty = true;
	m_rotation = rotate;
}

/***********************************************************************************/
QMatrix4x4 Camera::toMatrix() {
	if (m_dirty) {
		m_dirty = false;
		m_world.setToIdentity();
		m_world.rotate(m_rotation.conjugate());
		m_world.translate(-m_translation);
	}
	return m_world;
}
