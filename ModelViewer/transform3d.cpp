#include "transform3d.h"

/***********************************************************************************/
void Transform3D::translate(const QVector3D& pos) noexcept {
	m_dirty = true;
	m_translation += pos;
}

/***********************************************************************************/
void Transform3D::scale(const QVector3D& scale) noexcept {
	m_dirty = true;
	m_scale *= scale;
}

/***********************************************************************************/
void Transform3D::rotate(const QQuaternion& quat) noexcept {
	m_dirty = true;
	m_rotation = quat * m_rotation;
}

/***********************************************************************************/
void Transform3D::grow(const QVector3D& factor) noexcept {
	m_dirty = true;
	m_scale += factor;
}

/***********************************************************************************/
void Transform3D::setTranslation(const QVector3D& pos) noexcept {
	m_dirty = true;
	m_translation = pos;
}

/***********************************************************************************/
void Transform3D::setScale(const QVector3D& scale) noexcept {
	m_dirty = true;
	m_scale = scale;
}

/***********************************************************************************/
void Transform3D::setRotation(const QQuaternion& quat) noexcept {
	m_dirty = true;
	m_rotation = quat;
}

/***********************************************************************************/
QMatrix4x4 Transform3D::toMatrix() noexcept {
	if (m_dirty) {
		m_dirty = false;
		m_world.setToIdentity();
		m_world.translate(m_translation);
		m_world.rotate(m_rotation);
		m_world.scale(m_scale);
	}
	return m_world;
}


