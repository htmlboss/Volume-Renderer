#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

class Vertex {
public:
	Q_DECL_CONSTEXPR Vertex() : Position(0.0f, 0.0f, 0.0f), Color(1.0f, 1.0f, 1.0f) {}
	Q_DECL_CONSTEXPR explicit Vertex(const QVector3D& position) : Position(position), Color(1.0f, 1.0f, 1.0f) {}
	Q_DECL_CONSTEXPR Vertex(const QVector3D& position, const QVector3D& color) : Position(position), Color(color) {}

	static Q_DECL_CONSTEXPR auto positionOffset() { return offsetof(Vertex, Position); }
	static Q_DECL_CONSTEXPR auto colorOffset() { return offsetof(Vertex, Color); }
	static Q_DECL_CONSTEXPR auto sizeOf() { return sizeof(Vertex); }

	QVector3D Position;
	QVector3D Color;
};
// Vertex can be moved (equivalent of std::move() I think)
Q_DECLARE_TYPEINFO(Vertex, Q_MOVABLE_TYPE);

#endif // VERTEX_H
