#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void on_actionAboutQt_triggered();

	void on_selectFolderPushButton_clicked();

	void on_snapshotPushButton_clicked();

	void on_screenshotFolder_textChanged(const QString &arg1);

private:
	Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
