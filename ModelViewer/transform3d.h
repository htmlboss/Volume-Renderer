#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>

class Transform3D {
public:
	Transform3D() noexcept : m_dirty(true), m_scale(1.0f, 1.0f, 1.0f) {}

	// Transform By (Add/Scale)
	void translate(const QVector3D& pos) noexcept;
	void translate(const float x, const float y, const float z) noexcept { translate(QVector3D(x, y, z)); }

	void scale(const QVector3D& scale) noexcept;
	void scale(const float x, const float y, const float z) noexcept { scale(QVector3D(x, y, z)); }
	void scale(const float factor) noexcept { scale(QVector3D(factor, factor, factor)); }

	void rotate(const QQuaternion& quat) noexcept;
	void rotate(const float angle, const QVector3D& axis) noexcept { rotate(QQuaternion::fromAxisAndAngle(axis, angle)); }
	void rotate(const float angle, const float x, const float y, const float z) noexcept { rotate(QQuaternion::fromAxisAndAngle(x, y, z, angle)); }

	void grow(const QVector3D& factor) noexcept;
	void grow(const float x, const float y, const float z) noexcept { grow(QVector3D(x, y, z));}
	void grow(const float factor) noexcept { grow(QVector3D(factor, factor, factor)); }

	// Transform To (Setters)
	void setTranslation(const QVector3D& pos) noexcept;
	void setTranslation(const float x, const float y, const float z) noexcept { setTranslation(QVector3D(x, y, z)); }

	void setScale(const QVector3D& scale) noexcept;
	void setScale(const float x, const float y, const float z) noexcept { setScale(QVector3D(x, y, z)); }
	void setScale(const float factor) noexcept { setScale(QVector3D(factor, factor, factor)); }

	void setRotation(const QQuaternion& quat) noexcept;
	void setRotation(const float angle, const QVector3D& axis) noexcept { setRotation(QQuaternion::fromAxisAndAngle(axis, angle)); }
	void setRotation(const float angle, const float x, const float y, const float z) noexcept { setRotation(QQuaternion::fromAxisAndAngle(x, y, z, angle)); }

	// Getters
	auto getTranslation() const noexcept { return m_translation; }
	auto getScale() const noexcept { return m_scale;}
	auto getRotation() const noexcept { return m_rotation; }
	QMatrix4x4 toMatrix() noexcept;

	auto forward() const { return m_rotation.rotatedVector(m_forward); }
	auto right() const { return m_rotation.rotatedVector(m_right); }
	auto up() const { return m_rotation.rotatedVector(m_up); }

private:
	bool m_dirty;
	QVector3D m_translation, m_scale;
	QQuaternion m_rotation;
	QMatrix4x4 m_world;

	const QVector3D m_forward = QVector3D(0.0f, 0.0f, -1.0f);
	const QVector3D m_up = QVector3D(0.0f, 1.0f, 0.0f);
	const QVector3D m_right = QVector3D(1.0f, 0.0f, 0.0f);
};

Q_DECLARE_TYPEINFO(Transform3D, Q_MOVABLE_TYPE);

#endif // TRANSFORM3D_H
