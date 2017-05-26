#ifndef CAMERA_H
#define CAMERA_H

#include "transform3d.h"

class Camera {
public:
	Camera() noexcept : m_dirty(true) {}

	// Transform By (Add/Scale)
	void translate(const QVector3D& pos);
	void translate(const float x, const float y, const float z) { translate(QVector3D(x, y, z)); }

	void rotate(const QQuaternion& rotate);
	void rotate(const float angle, const QVector3D& axis) { rotate(QQuaternion::fromAxisAndAngle(axis, angle)); }
	void rotate(const float angle, const float x, const float y, const float z) { rotate(QQuaternion::fromAxisAndAngle(QVector3D(x, y, z), angle)); }

	// Transform To (Setters)
	void setTranslation(const QVector3D& pos);
	void setTranslation(const float x, const float y, const float z) { setTranslation(QVector3D(x, y, z)); }

	void setRotation(const QQuaternion& rotate);
	void setRotation(const float angle, const QVector3D& axis) { setRotation(QQuaternion::fromAxisAndAngle(axis, angle)); }
	void setRotation(const float angle, const float x, const float y, const float z) { setRotation(QQuaternion::fromAxisAndAngle(QVector3D(x, y, z), angle)); }

	constexpr auto translation() noexcept { return m_translation; }
	auto rotation() noexcept { return m_rotation; }
	QMatrix4x4 toMatrix();

	auto forward() const { return m_rotation.rotatedVector(Forward); }
	auto right() const { return m_rotation.rotatedVector(Right); }
	auto up() const { return m_rotation.rotatedVector(Up); }

	const QVector3D Forward = QVector3D(0.0f, 0.0f, -1.0f);
	const QVector3D Up = QVector3D(0.0f, 1.0f, 0.0f);
	const QVector3D Right = QVector3D(1.0f, 0.0f, 0.0f);

private:
	bool m_dirty;
	QVector3D m_translation;
	QQuaternion m_rotation;
	QMatrix4x4 m_world;
};

Q_DECLARE_TYPEINFO(Camera, Q_MOVABLE_TYPE);

#endif // CAMERA_H
