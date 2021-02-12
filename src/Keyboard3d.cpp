#include "Keyboard3d.h"
#include <stdio.h>

Keyboard3D::Keyboard3D(View *view, BRect rect)
    :
    Keyboard(view),
    BGLView(rect, "Keyboard", B_FOLLOW_ALL_SIDES, 0, BGL_RGB | BGL_DOUBLE) {
  pntA[0] = -160.0;
  pntA[1] = pntA[2] = 0.0;
  pntB[0] = -130.0;
  pntB[1] = pntB[2] = 0.0;
  width = rect.right - rect.left;
  height = rect.bottom - rect.top;
}

void Keyboard3D::AttachedToWindow(void) {
  LockGL();
  BGLView::AttachedToWindow();
  gInit();
  gReshape(width, height);
  UnlockGL();
}

void Keyboard3D::FrameResized(float newWidth, float newHeight) {
  LockGL();
  BGLView::FrameResized(width, height);
  width = newWidth;
  height = newHeight;

  gReshape(width, height);

  UnlockGL();
  Render();
}

void Keyboard3D::gDraw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBegin(GL_QUADS);

  glNormal3f(0.0, 0.0, 1.0);
  glVertex3f(-0.5, -0.5, 1.0);
  glVertex3f(-0.5, 0.5, 1.0);
  glVertex3f(0.5, 0.5, 1.0);
  glVertex3f(0.5, -0.5, 1.0);

  glNormal3f(0.0, 0.0, -1.0);
  glVertex3f(-0.5, -0.5, 0.0);
  glVertex3f(-0.5, 0.5, 0.0);
  glVertex3f(0.5, 0.5, 0.0);
  glVertex3f(0.5, -0.5, 0.0);

  glNormal3f(1.0, 0.0, 0.0);
  glVertex3f(0.5, -0.5, 0);
  glVertex3f(0.5, -0.5, 1);
  glVertex3f(0.5, 0.5, 1);
  glVertex3f(0.5, 0.5, 0);

  glNormal3f(-1.0, 0.0, 0.0);
  glVertex3f(-0.5, -0.5, 0.0);
  glVertex3f(-0.5, -0.5, 1.0);
  glVertex3f(-0.5, 0.5, 1.0);
  glVertex3f(-0.5, 0.5, 0.0);

  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-0.5, 0.5, 0.0);
  glVertex3f(-0.5, 0.5, 1.0);
  glVertex3f(0.5, 0.5, 1.0);
  glVertex3f(0.5, 0.5, 0.0);

  glNormal3f(0.0, -1.0, 0.0);
  glVertex3f(-0.5, -0.5, 0.0);
  glVertex3f(-0.5, -0.5, 1.0);
  glVertex3f(0.5, -0.5, 1.0);
  glVertex3f(0.5, -0.5, 0.0);

  glEnd();

  glFlush();

  /*	GLint i;

          glClear(GL_COLOR_BUFFER_BIT);
          glLineWidth(linesize);

          if (use_stipple_mode) {
                  glEnable(GL_LINE_STIPPLE);
          } else {
                  glDisable(GL_LINE_STIPPLE);
          }

          if (use_smooth_mode) {
                  glEnable(GL_LINE_SMOOTH);
                  glEnable(GL_BLEND);
          } else {
                  glDisable(GL_LINE_SMOOTH);
                  glDisable(GL_BLEND);
          }

          glPushMatrix();

                  glColor3f(0.0, 0.0, 0.0);
                  glBegin(GL_LINE_STRIP);
                          glVertex3f(1.0, 2.0, -1.0);
                          glVertex3f(1.0, 2.0, 1.0);
                          glVertex3f(-1.0, 2.0, -1.0);
                          glVertex3f(-1.0, 2.0, 1.0);
                  glEnd();
          for (i = 0; i < 360; i += 5) {
                  glRotatef(5.0, 0,0,1);		// Rotate right 5
     degrees glColor3f(1.0, 1.0, 0.0);	// Set color for line
                  glBegin(GL_LINE_STRIP);		// And create the line
                          glVertex3fv(pntA);
                          glVertex3fv(pntB);
                  glEnd();

                  glPointSize(pointsize);		// Set size for point
                  glColor3f(0.0, 1.0, 0.0);	// Set color for point
                  glBegin(GL_POINTS);
                          glVertex3fv(pntA);		// Draw point at one end
                          glVertex3fv(pntB);		// Draw point at other
     end glEnd();
          }

          glPopMatrix();					// Done with
     matrix
  */
}

void Keyboard3D::gReshape(int width, int height) {
  /*
          glViewport(0, 0, width, height);
          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          //gluOrtho2D(-175, 175, -175, 175);
          gluPerspective(60.0, 1.0, -10.0, 10.0);
          glMatrixMode(GL_MODELVIEW);
  */
#define RADIUS 7
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 5 * RADIUS);
  gluLookAt(0.0, 2.5 * RADIUS, RADIUS, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, width, height);
}

void Keyboard3D::ErrorCallback(GLenum whichError) {
  fprintf(stderr, "Unexpected error occured (%d):\\n", whichError);
  fprintf(stderr, "   %s\\n", gluErrorString(whichError));
}

void Keyboard3D::Render(void) {
  LockGL();
  gDraw();
  SwapBuffers();
  UnlockGL();
}

void Keyboard3D::gInit(void) {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glLineStipple(1, 0xF0E0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  use_stipple_mode = GL_FALSE;
  use_smooth_mode = GL_TRUE;
  linesize = 2;
  pointsize = 4;

  GLfloat LightPosition[] = {-0.5 * RADIUS, 2.5 * RADIUS, 0.8 * RADIUS, 1.0};
  /* enables lighting and light source 0 */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  /* sets background color */
  glClearColor(0.8, 0.8, 0.8, 1.0);
  /* sets light source NO 0 */
  glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
}

void Keyboard3D::NoteOn(uchar note) {}

void Keyboard3D::NoteOff(uchar note) {}