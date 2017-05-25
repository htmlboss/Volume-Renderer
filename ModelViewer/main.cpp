#include <QApplication>
#include <QMainWindow>

#include "mainwindow.h"
#include "oglwidget.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	QSurfaceFormat fmt;
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	fmt.setVersion(4, 5);

	OGLWidget* widget = new OGLWidget;
	widget->setFormat(fmt);

	MainWindow window;
	window.setCentralWidget(widget);
	window.resize(QSize(800, 600));
	window.show();

	return a.exec();
}
