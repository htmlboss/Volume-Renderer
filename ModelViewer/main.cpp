#include <QGuiApplication>

#include "window.h"

#include <iostream>

int main(int argc, char *argv[]) {
	QGuiApplication a(argc, argv);

	QSurfaceFormat fmt;
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	fmt.setVersion(4, 5);

	Window window;
	window.setFormat(fmt);
	window.resize(1280, 720);

	try {
		window.show();
		a.exec();
	} catch(const std::runtime_error& e) {
		std::cerr << "Error: " << e.what();
	}

	return 0;
}
