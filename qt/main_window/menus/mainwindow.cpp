#include "mainwindow.h"

#include <QtGui>

MainWindow::MainWindow()
{
	setAttribute(Qt::WA_DeleteOnClose);
	
	textEdit = new QTextEdit;
	setCentralWidget(textEdit);
	
	createActions();
	createMenus();
	statusBar();

	setWindowTitle(tr("Recent Files"));
	resize(400, 300);
}

void MainWindow::newFile()
{
	MainWindow *other = new MainWindow;
	other->show();
}

void MainWindow::open()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	
	if (!fileName.isEmpty()) {
		loadFile(fileName);
	}
}

void MainWindow::save()
{
	if (curFile.isEmpty()) {
		saveAs();
	} else {
		saveFile(curFile);
	}
}

void MainWindow::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	
	if (fileName.isEmpty()) {
		return;
	}

	saveFile(fileName);
}

void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	
	if (action) {
		loadFile(action->data().toString());
	}
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Recent Files"), tr("The <b>Recent Files</b>  example demonstrations how to provide a recent used file menu in a Qt application."));
}

void MainWindow::createActions()
{
	newAct = new QAction(tr("&New"), this);
	newAct->setShortcut(tr("Ctrl+N"));
	newAct->setStatusTip(tr("Create a new file"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
	
	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAct = new QAction(tr("&Save"), this);
	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAct = new QAction(tr("Save &As..."), this);
	saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	for (int i = 0; i < MaxRecentFiles; ++i) { 
		recentFilesActs[i] = new QAction(this);
		recentFilesActs[i]->setVisible(false);
		connect(recentFilesActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}	

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows())); 

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About Qt"), this);	
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt())); 
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	separatorAct = fileMenu->addSeparator();
	
	for (int i = 0; i < MaxRecentFiles; ++i) {
		fileMenu->addAction(recentFilesActs[i]);
	}

	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	updateRecentFileActions();

	menuBar()->addSeparator();
	
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);	
	helpMenu->addAction(aboutQtAct);	
}

void MainWindow::loadFile(const QString &fileName)
{
	QFile file(fileName);
	
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Recent Files"), 
							tr("Cannot read files %1:\n%2.")
							.arg(fileName)
							.arg(file.errorString()));
		return;
	}
	
	QTextStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	textEdit->setPlainText(in.readAll());
	QApplication::restoreOverrideCursor();

	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::saveFile(const QString &fileName)
{
	QFile file(fileName);
	
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Recent Files"), 
							tr("Cannot write files %1:\n%2.")
							.arg(fileName)
							.arg(file.errorString()));
		return;
	}
	
	QTextStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << textEdit->toPlainText();
	QApplication::restoreOverrideCursor();

	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	if (curFile.isEmpty()) {
		setWindowTitle(tr("Recent Files"));
	} else {
		setWindowTitle(tr("%1 - %2").arg(strippedName(curFile)).arg(tr("Recent Files")));
	}
	
	QSettings settings("Trolltech", "Recent files examples");
	QStringList files = settings.value("recentFileList").toStringList();
	files.removeAll(fileName);
	files.prepend(fileName);
	
	while (files.size() > MaxRecentFiles) {
		files.removeLast();
	}

	settings.setValue("recentFileList", files);
	
	foreach (QWidget *widget, QApplication::topLevelWidgets()) {
		MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
		if (mainWin) {
			mainWin->updateRecentFileActions();
		}
	}
}

void MainWindow::updateRecentFileActions()
{
	QSettings settings("Trolltech", "Recent files examples");
	QStringList files = settings.value("recentFileList").toStringList();
	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
	
	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
		recentFilesActs[i]->setText(text);
		recentFilesActs[i]->setData(files[i]);
		recentFilesActs[i]->setVisible(true);
	}

	for (int j = numRecentFiles; j < MaxRecentFiles; ++j) {
		recentFilesActs[j]->setVisible(false);
	}
	
	separatorAct->setVisible(numRecentFiles > 0);
}

QString MainWindow::strippedName(const QString &fullName)
{
	return QFileInfo(fullName).fileName();	
}
