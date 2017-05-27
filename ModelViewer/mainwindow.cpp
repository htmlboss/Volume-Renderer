#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QTime>

/***********************************************************************************/
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	ui->snapshotPushButton->setEnabled(false);

	setWindowTitle("APFSDS-T");

	QFile f(":qdarkstyle/style.qss");
	if (!f.exists()) {
		qDebug() << "Unable to set stylesheet, file not found";
	}
	else {
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		setStyleSheet(ts.readAll());
	}

	statusBar()->showMessage("Ready");
}

/***********************************************************************************/
MainWindow::~MainWindow() {
	delete ui;
}

/***********************************************************************************/
void MainWindow::on_actionAboutQt_triggered() {
	QApplication::aboutQt();
}

/***********************************************************************************/
void MainWindow::on_selectFolderPushButton_clicked() {
	ui->screenshotFolder->setText(QFileDialog::getExistingDirectory(this, "Select a folder to save snapshots...", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

/***********************************************************************************/
void MainWindow::on_snapshotPushButton_clicked() {
	static uint_fast64_t snapshotNum;

	const auto snapshot = ui->openGLWidget->grabFramebuffer();
	const auto fileName = QString(ui->screenshotFolder->text() + "/snapshot" + QString::number(snapshotNum++) + ".png");

	snapshot.save(fileName);

	statusBar()->showMessage("Saved screenshot to: " + fileName + ".", 3000);
}

/***********************************************************************************/
void MainWindow::on_screenshotFolder_textChanged(const QString& arg1) {
	if (arg1.isEmpty()) {
		ui->snapshotPushButton->setEnabled(false);
		return;
	}
	ui->snapshotPushButton->setEnabled(true);
}

/***********************************************************************************/
void MainWindow::on_invertAxisCheckBox_clicked() {
	ui->openGLWidget->SetInvertedAxis(ui->invertAxisCheckBox->isChecked());
}

/***********************************************************************************/
void MainWindow::on_rotSensitivirySpinBox_editingFinished() {
	ui->openGLWidget->SetMouseSensitivity(ui->rotSensitivirySpinBox->value());
}

/***********************************************************************************/
void MainWindow::on_rotSensitivirySpinBox_valueChanged(double arg1) {
	ui->openGLWidget->SetMouseSensitivity(arg1);
}
