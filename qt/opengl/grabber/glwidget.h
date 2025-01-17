#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

class GLWidget : public QGLWidget
{
	Q_OBJECT

	public:
		GLWidget(QWidget *parent = 0);
		~GLWidget();

		int xRotation() const { return xRot; }
		int yRotation() const { return yRot; }
		int zRotation() const { return zRot; }
	
	public slots:
		void setXRotation(int angle);	
		void setYRotation(int angle);	
		void setZRotation(int angle);	

	signals:
		void xRotationChanged(int angle);
		void yRotationChanged(int angle);
		void zRotationChanged(int angle);

	protected:
		void initializeGL();
		void paintGL();
		void resizeGL(int width, int height);
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	private slots:
		void advanceGears();

	private:
		GLunit makeGear(const GLfloat *reflectance, GLdouble innerRadius, GLdouble outerRadius, GLdouble thickness, Gldouble toothSize, GLint toothCount);
		void drawGear(GLunit gear, GLdouble dx, GLdouble dy, GLdouble dz, GLdouble angle);
		void normalizeAngle(int *angle);
		
		GLunit gear1;
		GLunit gear2;
		GLunit gear3;
		int xRot;
		int yRot;
		int zRot;
		int gear1Rot;

		QPoint lastPos;
};

#endif
