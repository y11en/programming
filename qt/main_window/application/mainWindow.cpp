#include "mainWindow.h"

MainWindow::MainWindow()
{
	textEdit = new QPlainTextEdit;
	setCentralWidget(textEdit);

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	
	readSettings();
	connect(textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

	setCurrentFile("");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (maybeSave()) {
		event->accept();
	}
	else {
		event->ignore();
	}
	
	writeSettings();
}

void MainWindow::newFile()
{
	if (maybeSave()) {
		textEdit->clear();
		setCurrentFile("");
	}
}

void MainWindow::open()
{
	if (maybeSave()) {
		QString fileName = QFileDialog::getOpenFileName(this);
		
		if (!fileName.isEmpty())
			loadFile(fileName);
	}
}

bool MainWindow::save()
{
	if (curFile.isEmpty()) {
		return saveAs();
	}
	else {
		return saveFile(curFile);
	}
}

bool MainWindow::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	
	if (fileName.isEmpty()) {
		return false;
	}

	return saveFile(fileName);
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Application"), tr("The <b>Application</b> example demonstration how to write doc using qt"));
}

void MainWindow::documentWasModified()
{
	setWindowModified(textEdit->document()->isModified());
}

void MainWindow::createActions()
{
	newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
	newAct->setShortcut(tr("Ctrl+N"));
	newAct->setStatusTip(tr("create a new file"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open an existing file"));
	
	saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAct = new QAction(tr("Save &As..."), this);
	saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
	cutAct->setShortcut(tr("Ctrl+X"));
	cutAct->setStatusTip(tr("Cut the current selection's contents to the "
							"clipboard"));
	connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

	copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
	copyAct->setShortcut(tr("Ctrl+C"));
	copyAct->setStatusTip(tr("Copy the current selection's contents to the "
							"clipboard"));
	connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

	pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
	pasteAct->setShortcut(tr("Ctrl+V"));
	pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
							"selection"));
	connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));	

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	cutAct->setEnabled(false);
	copyAct->setEnabled(false);
	connect(textEdit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool))); 
	connect(textEdit, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	
	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(cutAct);
	editMenu->addAction(copyAct);
	editMenu->addAction(pasteAct);
	
	menuBar()->addSeparator();
	
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("&File"));
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAct);

	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(cutAct);
	editToolBar->addAction(copyAct);
	editToolBar->addAction(pasteAct);	
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
	QSettings settings(QSettings::SystemScope, "Trolltech", "Application Example");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
qDebug() << pos;
	QSize size = settings.value("size", QSize(400, 400)).toSize();
qDebug() << size;
	resize(size);
	move(pos);
}

void MainWindow::writeSettings()
{
	QSettings settings(QSettings::SystemScope, "Trolltech", "Application Example");
	settings.setValue("pos", pos());
qDebug() << pos();
	settings.setValue("size", size());
qDebug() << size();
}

bool MainWindow::maybeSave()
{
	if (textEdit->document()->isModified()) {
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, tr("Application"), tr("The document has been modified"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

		if (ret ==  QMessageBox::Save) {
			return save();
		} else if (ret == QMessageBox::Cancel) {
			return false;
		}
	}

	return true;
}

void MainWindow::loadFile(const QString &fileName)
{
	QFile file(fileName);
	
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Application"), tr("Cannot read file %1:\n%2.")
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

bool MainWindow::saveFile(const QString &fileName)
{
	QFile file(fileName);
	
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Application"),
									tr("Cannot write file %1:\n%2.")
									.arg(fileName)	
									.arg(file.errorString()));

		return false;
	}

	QTextStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << textEdit->toPlainText();
	QApplication::restoreOverrideCursor();

	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
		
	return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	textEdit->document()->setModified(false);
	setWindowModified(false);
	
	QString shownName;
	
	if (curFile.isEmpty()) {
		shownName = "untitled.txt";
	} else {
		shownName = strippedName(curFile);
	}

	setWindowTitle(tr("%1[*]-%2").arg(shownName).arg(tr("Application")));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}
