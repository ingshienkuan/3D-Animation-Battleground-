/*
 TCG6223 Computer Graphics
 FIST, Multimedia University

 CGLabmain.hpp (Enhanced Version)

 Objective: Enhanced Header File for Main Program with improved lighting support

 Copyright (C)Ya-Ping Wong <ypwong@mmu.edu.my>

 SPECIAL NOTES
 =============
 * Enhanced version with better lighting control and F3 toggle support
 * Added smooth shading flag and enhanced lighting controls

 ENHANCEMENTS
 ============
 * Added smoothShading flag to MySetting struct
 * Enhanced lighting control variables
 * Better material property support
*/

#ifndef YP_CGLABMAIN_HPP
#define YP_CGLABMAIN_HPP

#include <string>
#include <cmath>
#include <iostream>
#include <GL/glut.h>

using namespace std;

void myDisplayFunc(void);
void myReshapeFunc(int width, int height);
void myKeyboardFunc(unsigned char key, int x, int y);
void mySpecialFunc(int key, int x, int y);
void myMouseFunc(int button, int state, int x, int y);
void myMotionFunc(int x, int y);
void myTimerFunc(int value);

void myDataInit();
void myViewingInit();
void myInit();
void myWelcome();
void myLightingInit();

// Enhanced lighting setup function
void setupEnhancedLighting();

static GLfloat myblack[]   = { 0.0f, 0.0f, 0.0f, 1.0f };
static GLfloat mywhite[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat myred[]     = { 1.0f, 0.0f, 0.0f, 1.0f };
static GLfloat mygreen[]   = { 0.0f, 1.0f, 0.0f, 1.0f };
static GLfloat myblue[]    = { 0.0f, 0.0f, 1.0f, 1.0f };
static GLfloat mycyan[]    = { 0.0f, 1.0f, 1.0f, 1.0f };
static GLfloat mymagenta[] = { 1.0f, 0.0f, 1.0f, 1.0f };
static GLfloat myyellow[]  = { 1.0f, 1.0f, 0.0f, 1.0f };

static GLfloat mygray10Percent[] = { 0.1f, 0.1f, 0.1f, 1.0f };
static GLfloat mygray20Percent[] = { 0.2f, 0.2f, 0.2f, 1.0f };
static GLfloat mygray30Percent[] = { 0.3f, 0.3f, 0.3f, 1.0f };

// Enhanced color palette for better material properties
static GLfloat myWoodBrown[]     = { 0.6f, 0.4f, 0.2f, 1.0f };
static GLfloat myGoldenYellow[]  = { 0.8f, 0.7f, 0.3f, 1.0f };
static GLfloat myWarmWhite[]     = { 0.95f, 0.95f, 0.9f, 1.0f };

//***********************************
struct MyWindow
{
 string title;
 GLint posX;
 GLint posY;
 GLint width;
 GLint height;
};

struct MyWorld
{
 GLdouble rotateX, rotateY, rotateZ;
 GLdouble posX, posY, posZ;
 GLdouble scaleX, scaleY, scaleZ;
 void move(GLfloat xinc, GLfloat yinc, GLfloat zinc)
 {
  posX += xinc;
  posY += yinc;
  posZ += zinc;
 }
 void rotate(GLfloat xinc, GLfloat yinc, GLfloat zinc)
 {
  rotateX += xinc;
  rotateY += yinc;
  rotateZ += zinc;
 }
};

struct MyViewer
{
 GLdouble eyeX, eyeY, eyeZ;
 GLdouble centerX, centerY, centerZ;
 GLdouble upX, upY, upZ;

 GLdouble zNear, zFar;   // The distance to the near/far clipping plane
 GLdouble fieldOfView;   // Angle at Y direction
 GLdouble aspectRatio;   // width / height
};

struct MySetting
{
 GLdouble posInc;
 GLdouble angleInc;
 int mouseX, mouseY;
 bool mouseRightMode;
 bool mouseLeftMode;

 bool shadingMode;
 bool smoothShading;  // Enhanced: Added for better F3 toggle tracking
 bool enhancedLighting; // Enhanced: Added for lighting control
};

class MyAxis
{
 private:
    GLfloat xlen, ylen, zlen;
    GLfloat xred, xgreen, xblue;
    GLfloat yred, ygreen, yblue;
    GLfloat zred, zgreen, zblue;
    GLint    linestipplerepeatfactor;
    GLfloat  linewidth;
    GLushort linestipplepattern;
    bool     shoulddraw;
 public:
    MyAxis()
    {
     setXColor(1.0f, 0.0f, 0.0f);
     setYColor(0.0f, 1.0f, 0.0f);
     setZColor(0.0f, 0.0f, 1.0f);
     setLength(20.0f, 20.0f, 20.0f);
     setLineStipple(1, 0xffff);
     setLineWidth(1);
     shoulddraw = true;
    }

    void setLength(GLfloat xLen, GLfloat yLen, GLfloat zLen)
       { xlen = xLen; ylen = yLen; zlen = zLen; }
    void setXColor(GLfloat r, GLfloat g, GLfloat b)
       { xred = r; xgreen = g; xblue = b; }
    void setYColor(GLfloat r, GLfloat g, GLfloat b)
       { yred = r; ygreen = g; yblue = b; }
    void setZColor(GLfloat r, GLfloat g, GLfloat b)
       { zred = r; zgreen = g; zblue = b; }
    void setLineWidth(GLfloat lineWidth)
       { linewidth = lineWidth; }
    void setLineStipple(GLint repeatfactor, GLushort pattern)
       { linestipplerepeatfactor = repeatfactor, linestipplepattern = pattern; }

    void draw()
    {
     if (!shoulddraw) return;

     GLboolean lightingIsOn;
     glGetBooleanv(GL_LIGHTING, &lightingIsOn);
     glDisable(GL_LIGHTING);

     GLint oldlinestipplerepeatfactor;
     GLint oldlinestipplepattern;
     GLfloat  oldlinewidth;
     GLboolean stippleIsOn;
     glGetIntegerv(GL_LINE_STIPPLE_REPEAT, &oldlinestipplerepeatfactor);
     glGetIntegerv(GL_LINE_STIPPLE_PATTERN, &oldlinestipplepattern);
     glGetFloatv(GL_LINE_WIDTH, &oldlinewidth);
     glGetBooleanv(GL_LINE_STIPPLE,&stippleIsOn);

     glEnable(GL_LINE_STIPPLE);
     glLineWidth(linewidth);
     glLineStipple(linestipplerepeatfactor, linestipplepattern);
     glBegin(GL_LINES);
        //x-axis
        glColor3f(xred, xgreen, xblue);

        glVertex3f( 0.0, 0.0, 0.0);
        glVertex3f(xlen, 0.0, 0.0);
        //x-axis arrow
        glVertex3f(    xlen, 0.0,  0.0);
        glVertex3f(0.9*xlen, 0.0,  0.05*xlen);
        glVertex3f(    xlen, 0.0,  0.0);
        glVertex3f(0.9*xlen, 0.0, -0.05*xlen);

        //y-axis
        glColor3f(yred, ygreen, yblue);
        glVertex3f(0.0,  0.0, 0.0);
        glVertex3f(0.0, ylen, 0.0);
        //y-axis arrow
        glVertex3f(       0.0,     ylen, 0.0);
        glVertex3f( 0.05*ylen, 0.9*ylen, 0.0);
        glVertex3f(       0.0,     ylen, 0.0);
        glVertex3f(-0.05*ylen, 0.9*ylen, 0.0);

        //z-axis
        glColor3f(zred, zgreen, zblue);
        glVertex3f(0.0, 0.0,  0.0);
        glVertex3f(0.0, 0.0, zlen);
        //z-axis arrow
        glVertex3f(0.0, 0.0, zlen);
        glVertex3f( 0.05*zlen, 0.0, 0.9*zlen);
        glVertex3f(       0.0, 0.0,     zlen);
        glVertex3f(-0.05*zlen, 0.0, 0.9*zlen);
     glEnd();

     glLineWidth(oldlinewidth);
     glLineStipple(oldlinestipplerepeatfactor, oldlinestipplepattern);

     if (stippleIsOn==GL_FALSE)
        glDisable(GL_LINE_STIPPLE);

     if (lightingIsOn==GL_TRUE)
        glEnable(GL_LIGHTING);
    } //void draw()

    void toggle()
    {
     shoulddraw = !shoulddraw;
    }
    void enable()
    {
     shoulddraw = true;
    }
    void disable()
    {
     shoulddraw = false;
    }
};

// Enhanced material property helper functions
namespace EnhancedMaterials {

    // Wood material properties
    inline void setWoodMaterial() {
        GLfloat mat_ambient[]  = { 0.3f, 0.2f, 0.1f, 1.0f };
        GLfloat mat_diffuse[]  = { 0.6f, 0.4f, 0.2f, 1.0f };
        GLfloat mat_specular[] = { 0.1f, 0.1f, 0.05f, 1.0f };
        GLfloat mat_shininess[] = { 10.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    }

    // Golden material properties
    inline void setGoldenMaterial() {
        GLfloat mat_ambient[]  = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
        GLfloat mat_diffuse[]  = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
        GLfloat mat_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
        GLfloat mat_shininess[] = { 51.2f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    }

    // Stone material properties
    inline void setStoneMaterial() {
        GLfloat mat_ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
        GLfloat mat_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        GLfloat mat_shininess[] = { 20.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    }

    // Default material properties (soft)
    inline void setDefaultMaterial() {
        GLfloat mat_ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat mat_shininess[] = { 32.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    }
}

// Enhanced lighting helper functions
namespace EnhancedLighting {

    // Setup atmospheric lighting (dimmer, warmer)
    inline void setupAtmosphericLighting() {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);

        // Main atmospheric light
        GLfloat light0_ambient[]  = { 0.15f, 0.2f, 0.15f, 1.0f };
        GLfloat light0_diffuse[]  = { 0.4f, 0.4f, 0.35f, 1.0f };
        GLfloat light0_specular[] = { 0.3f, 0.3f, 0.25f, 1.0f };
        GLfloat light0_position[] = { 1.0f, 2.0f, 1.0f, 0.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

        // Soft fill light
        GLfloat light1_ambient[]  = { 0.1f, 0.12f, 0.1f, 1.0f };
        GLfloat light1_diffuse[]  = { 0.2f, 0.25f, 0.2f, 1.0f };
        GLfloat light1_specular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
        GLfloat light1_position[] = { -0.5f, 1.0f, 0.5f, 0.0f };

        glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
        glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

        // Dimmer global ambient
        GLfloat global_ambient[] = { 0.1f, 0.15f, 0.2f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    }

    // Setup bright lighting (original style)
    inline void setupBrightLighting() {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        GLfloat light0_ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat light0_diffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat light0_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

        GLfloat global_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    }
}

#endif //YP_CGLABMAIN_HPP
