/*
==============================================================================
TCG6223 COMPUTER GRAPHICS - BLIND BOXES BATTLE COLLECTION
Snorlax vs Kirby: Interactive 3D Battle Arena
==============================================================================
Team Members: [Add your team member names and IDs here]
Lab Section: [Add your lab section]
Submission Date: June 12, 2025

PROJECT OVERVIEW:
This project demonstrates advanced OpenGL programming through an interactive
3D battle system featuring two detailed characters built entirely from primitives.

TECHNICAL FEATURES DEMONSTRATED:
- Complex 3D character modeling using mathematical transformations
- Multi-layered lighting system (ambient, diffuse, specular, rim lighting)
- Real-time particle systems with 3D star geometry
- Dynamic animation using trigonometric functions
- Interactive battle mechanics with HP tracking
- 2D UI overlay system with custom text rendering
- Advanced material properties and normal calculations
- Transparency effects and alpha blending
==============================================================================
*/

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <GL/glut.h>

#include "CGLabmain.hpp"
#include "Snorlax.hpp"
#include "Kirby.hpp"
#include "BattleGround.hpp"

// ===== GLOBAL INSTANCES AND STATE MANAGEMENT =====
Snorlax::MyVirtualWorld snorlaxWorld;
Kirby::MyVirtualWorld kirbyWorld;
BattleGround::MyVirtualWorld battlegroundWorld;

// Display mode enumeration for different viewing options
enum ActiveDisplay {
    SNORLAX_ONLY,
    KIRBY_ONLY,
    BATTLEGROUND_ONLY,
    KIRBY_AND_SNORLAX,
    ALL_THREE
};
ActiveDisplay activeDisplay = ALL_THREE;

// Game state tracking variables
bool gameOver = false;
bool snorlaxWon = false;
bool kirbyWon = false;

// Rendering control variables
bool smoothShading = true;
bool enhancedLighting = true;

// Core application structure variables
using namespace std;
MyWindow   window;
MyWorld    world;
MyViewer   viewer;
MySetting  setting;
MyAxis     worldaxis;

// ===== ADVANCED LIGHTING SYSTEM =====
/**
 * Three-point lighting setup with enhanced material properties
 * Demonstrates professional lighting techniques for 3D character rendering
 */
void setupEnhancedLighting() {
    if (enhancedLighting) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_NORMALIZE);

        // Configure shading model based on user preference
        if (smoothShading) {
            glShadeModel(GL_SMOOTH);
        } else {
            glShadeModel(GL_FLAT);
        }

        // Primary directional light - main scene illumination
        GLfloat light0_ambient[] = { 0.25f, 0.3f, 0.25f, 1.0f };
        GLfloat light0_diffuse[] = { 0.7f, 0.7f, 0.6f, 1.0f };
        GLfloat light0_specular[] = { 0.5f, 0.5f, 0.4f, 1.0f };
        GLfloat light0_position[] = { 2.0f, 3.0f, 1.0f, 0.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

        // Secondary fill light - reduces harsh shadows
        GLfloat light1_ambient[] = { 0.2f, 0.22f, 0.2f, 1.0f };
        GLfloat light1_diffuse[] = { 0.3f, 0.35f, 0.3f, 1.0f };
        GLfloat light1_specular[] = { 0.15f, 0.15f, 0.15f, 1.0f };
        GLfloat light1_position[] = { -1.0f, 2.0f, 0.5f, 0.0f };

        glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
        glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

        // Global ambient lighting for overall scene brightness
        GLfloat global_ambient[] = { 0.2f, 0.25f, 0.3f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        // Enhanced material properties for realistic surface interaction
        GLfloat mat_shininess[] = { 32.0f };
        GLfloat mat_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    } else {
        glDisable(GL_LIGHTING);
    }
}

// ===== MAIN RENDERING PIPELINE =====
/**
 * Primary display function managing the complete rendering pipeline
 * Handles 3D world setup, character positioning, and UI overlay rendering
 */
void myDisplayFunc(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setupEnhancedLighting();

    // Apply world transformation matrix
    glPushMatrix();
    glTranslatef(world.posX, world.posY, world.posZ);
    glRotatef(world.rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(world.rotateY, 0.0f, 1.0f, 0.0f);
    glRotatef(world.rotateZ, 0.0f, 0.0f, 1.0f);
    glScalef(world.scaleX, world.scaleY, world.scaleZ);

    worldaxis.draw();

    // Render appropriate scene based on user selection
    switch(activeDisplay) {
        case SNORLAX_ONLY:
            snorlaxWorld.tickTime();
            snorlaxWorld.draw();
            break;

        case KIRBY_ONLY:
            kirbyWorld.update(0.016f);
            kirbyWorld.draw();
            break;

        case BATTLEGROUND_ONLY:
            battlegroundWorld.tickTime();
            battlegroundWorld.draw();
            break;

        case KIRBY_AND_SNORLAX:
            // Position Snorlax on left side facing right
            glPushMatrix();
            glTranslatef(-10.0f, -2.0f, 0.0f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            snorlaxWorld.tickTime();
            snorlaxWorld.draw();
            glPopMatrix();

            // Position Kirby on right side facing left
            glPushMatrix();
            glTranslatef(10.0f, 0.0f, 0.0f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
            kirbyWorld.update(0.016f);
            kirbyWorld.draw();
            glPopMatrix();
            break;

        case ALL_THREE:
            // Render complete battle scene with environment
            glPushMatrix();
            glTranslatef(0.0f, -6.0f, 0.0f);
            glScalef(1.2f, 1.2f, 1.2f);
            battlegroundWorld.tickTime();
            battlegroundWorld.draw();
            glPopMatrix();

            // Position Snorlax on battle platform (left side)
            glPushMatrix();
            glTranslatef(-15.5f, -1.5f, 0.0f);
            glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            glScalef(1.0f, 1.0f, 1.0f);
            snorlaxWorld.tickTime();
            snorlaxWorld.draw();
            glPopMatrix();

            // Position Kirby on battle platform (right side)
            glPushMatrix();
            glTranslatef(15.5f, -0.5f, 1.4f);
            glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
            glScalef(0.8f, 0.8f, 0.8f);
            kirbyWorld.update(0.016f);
            kirbyWorld.draw();
            glPopMatrix();
            break;
    }

    glPopMatrix();

    // Render game over screen with victory announcements
    if (gameOver) {
        // Switch to 2D orthogonal projection for UI overlay
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        // Main "GAME OVER!" text
        glPushMatrix();
        glTranslatef(290.0f, 520.0f, 0.0f);
        glScalef(22.0f, 30.0f, 8.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(6.0f);
        float spacing = 1.3f;

        battlegroundWorld.drawLetter(0.0f, 0.0f, 1.0f, 'G');
        battlegroundWorld.drawLetter(1.0f * spacing, 0.0f, 1.0f, 'A');
        battlegroundWorld.drawLetter(2.0f * spacing, 0.0f, 1.0f, 'M');
        battlegroundWorld.drawLetter(3.0f * spacing, 0.0f, 1.0f, 'E');

        battlegroundWorld.drawLetter(4.8f * spacing, 0.0f, 1.0f, 'O');
        battlegroundWorld.drawLetter(5.8f * spacing, 0.0f, 1.0f, 'V');
        battlegroundWorld.drawLetter(6.8f * spacing, 0.0f, 1.0f, 'E');
        battlegroundWorld.drawLetter(7.8f * spacing, 0.0f, 1.0f, 'R');

        // Exclamation mark using quads
        glBegin(GL_QUADS);
        glVertex2f(8.8f * spacing - 0.05f, 0.2f);
        glVertex2f(8.8f * spacing + 0.05f, 0.2f);
        glVertex2f(8.8f * spacing + 0.05f, 0.8f);
        glVertex2f(8.8f * spacing - 0.05f, 0.8f);
        glEnd();

        glBegin(GL_QUADS);
        glVertex2f(8.8f * spacing - 0.08f, 0.05f);
        glVertex2f(8.8f * spacing + 0.08f, 0.05f);
        glVertex2f(8.8f * spacing + 0.08f, 0.15f);
        glVertex2f(8.8f * spacing - 0.08f, 0.15f);
        glEnd();

        glPopMatrix();

        // Winner announcement text
        if (snorlaxWon) {
            glPushMatrix();
            glTranslatef(350.0f, 480.0f, 0.0f);
            glScalef(10.0f, 14.0f, 2.0f);
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(5.0f);

            battlegroundWorld.drawLetter(0.0f, 0.0f, 1.0f, 'S');
            battlegroundWorld.drawLetter(1.0f * spacing, 0.0f, 1.0f, 'N');
            battlegroundWorld.drawLetter(2.0f * spacing, 0.0f, 1.0f, 'O');
            battlegroundWorld.drawLetter(3.0f * spacing, 0.0f, 1.0f, 'R');
            battlegroundWorld.drawLetter(4.0f * spacing, 0.0f, 1.0f, 'L');
            battlegroundWorld.drawLetter(5.0f * spacing, 0.0f, 1.0f, 'A');
            battlegroundWorld.drawLetter(6.0f * spacing, 0.0f, 1.0f, 'X');

            battlegroundWorld.drawLetter(7.8f * spacing, 0.0f, 1.0f, 'W');
            battlegroundWorld.drawLetter(8.8f * spacing, 0.0f, 1.0f, 'I');
            battlegroundWorld.drawLetter(9.8f * spacing, 0.0f, 1.0f, 'N');
            battlegroundWorld.drawLetter(10.8f * spacing, 0.0f, 1.0f, 'S');

            glBegin(GL_QUADS);
            glVertex2f(11.8f * spacing - 0.05f, 0.2f);
            glVertex2f(11.8f * spacing + 0.05f, 0.2f);
            glVertex2f(11.8f * spacing + 0.05f, 0.8f);
            glVertex2f(11.8f * spacing - 0.05f, 0.8f);
            glEnd();

            glBegin(GL_QUADS);
            glVertex2f(11.8f * spacing - 0.08f, 0.05f);
            glVertex2f(11.8f * spacing + 0.08f, 0.05f);
            glVertex2f(11.8f * spacing + 0.08f, 0.15f);
            glVertex2f(11.8f * spacing - 0.08f, 0.15f);
            glEnd();

            glPopMatrix();
        } else if (kirbyWon) {
            glPushMatrix();
            glTranslatef(350.0f, 480.0f, 0.0f);
            glScalef(10.0f, 14.0f, 2.0f);
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(5.0f);

            battlegroundWorld.drawLetter(0.0f, 0.0f, 1.0f, 'K');
            battlegroundWorld.drawLetter(1.0f * spacing, 0.0f, 1.0f, 'I');
            battlegroundWorld.drawLetter(2.0f * spacing, 0.0f, 1.0f, 'R');
            battlegroundWorld.drawLetter(3.0f * spacing, 0.0f, 1.0f, 'B');
            battlegroundWorld.drawLetter(4.0f * spacing, 0.0f, 1.0f, 'Y');

            battlegroundWorld.drawLetter(5.8f * spacing, 0.0f, 1.0f, 'W');
            battlegroundWorld.drawLetter(6.8f * spacing, 0.0f, 1.0f, 'I');
            battlegroundWorld.drawLetter(7.8f * spacing, 0.0f, 1.0f, 'N');
            battlegroundWorld.drawLetter(8.8f * spacing, 0.0f, 1.0f, 'S');

            glBegin(GL_QUADS);
            glVertex2f(9.8f * spacing - 0.05f, 0.2f);
            glVertex2f(9.8f * spacing + 0.05f, 0.2f);
            glVertex2f(9.8f * spacing + 0.05f, 0.8f);
            glVertex2f(9.8f * spacing - 0.05f, 0.8f);
            glEnd();

            glBegin(GL_QUADS);
            glVertex2f(9.8f * spacing - 0.08f, 0.05f);
            glVertex2f(9.8f * spacing + 0.08f, 0.05f);
            glVertex2f(9.8f * spacing + 0.08f, 0.15f);
            glVertex2f(9.8f * spacing - 0.08f, 0.15f);
            glEnd();

            glPopMatrix();
        }

        // Restart instruction
        glPushMatrix();
        glTranslatef(340.0f, 440.0f, 0.0f);
        glScalef(7.0f, 9.0f, 2.0f);
        glColor3f(0.3f, 0.3f, 0.3f);
        glLineWidth(3.0f);

        battlegroundWorld.drawLetter(0.0f, 0.0f, 1.0f, 'P');
        battlegroundWorld.drawLetter(1.0f * spacing, 0.0f, 1.0f, 'R');
        battlegroundWorld.drawLetter(2.0f * spacing, 0.0f, 1.0f, 'E');
        battlegroundWorld.drawLetter(3.0f * spacing, 0.0f, 1.0f, 'S');
        battlegroundWorld.drawLetter(4.0f * spacing, 0.0f, 1.0f, 'S');

        battlegroundWorld.drawLetter(5.5f * spacing, 0.0f, 1.0f, 'R');

        battlegroundWorld.drawLetter(7.0f * spacing, 0.0f, 1.0f, 'T');
        battlegroundWorld.drawLetter(8.0f * spacing, 0.0f, 1.0f, 'O');

        battlegroundWorld.drawLetter(9.5f * spacing, 0.0f, 1.0f, 'R');
        battlegroundWorld.drawLetter(10.5f * spacing, 0.0f, 1.0f, 'E');
        battlegroundWorld.drawLetter(11.5f * spacing, 0.0f, 1.0f, 'S');
        battlegroundWorld.drawLetter(12.5f * spacing, 0.0f, 1.0f, 'T');
        battlegroundWorld.drawLetter(13.5f * spacing, 0.0f, 1.0f, 'A');
        battlegroundWorld.drawLetter(14.5f * spacing, 0.0f, 1.0f, 'R');
        battlegroundWorld.drawLetter(15.5f * spacing, 0.0f, 1.0f, 'T');

        glPopMatrix();

        // Restore 3D projection
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}

// ===== GAME LOGIC SYSTEM =====
/**
 * Checks for game over conditions and announces winner
 */
void checkGameOver() {
    if (snorlaxWorld.currentHP <= 0.0f && !gameOver) {
        gameOver = true;
        kirbyWon = true;
        snorlaxWon = false;
        cout << "\n========== GAME OVER! ==========\n";
        cout << "KIRBY WINS!\n";
        cout << "===============================\n";
    } else if (kirbyWorld.currentHP <= 0.0f && !gameOver) {
        gameOver = true;
        snorlaxWon = true;
        kirbyWon = false;
        cout << "\n========== GAME OVER! ==========\n";
        cout << "SNORLAX WINS!\n";
        cout << "===============================\n";
    }
}

// ===== INPUT HANDLING SYSTEM =====
/**
 * Keyboard input handler for character controls and game management
 */
void myKeyboardFunc(unsigned char key, int x, int y)
{
    GLfloat xinc,yinc,zinc;
    xinc = yinc = zinc = 0.0;

    switch (key)
    {
        // World navigation controls
        case 'a': case 'A': xinc = -setting.posInc;  break;
        case 'd': case 'D': xinc =  setting.posInc;  break;
        case 'q': case 'Q': yinc = -setting.posInc;  break;
        case 'e': case 'E': yinc =  setting.posInc;  break;
        case 'w': case 'W': zinc = -setting.posInc;  break;
        case 's': case 'S': zinc =  setting.posInc;  break;

        // Display mode cycling
        case 't': case 'T':
            activeDisplay = (ActiveDisplay)((activeDisplay + 1) % 5);
            cout << "Switched to: ";
            switch(activeDisplay) {
                case SNORLAX_ONLY: cout << "Snorlax Only\n"; break;
                case KIRBY_ONLY: cout << "Kirby Only\n"; break;
                case BATTLEGROUND_ONLY: cout << "BattleGround Only\n"; break;
                case KIRBY_AND_SNORLAX: cout << "Kirby and Snorlax\n"; break;
                case ALL_THREE: cout << "All Three (BattleGround + Characters)\n"; break;
            }
            break;

        // Game reset functionality
        case 'r': case 'R':
            gameOver = false;
            snorlaxWon = false;
            kirbyWon = false;
            snorlaxWorld.currentHP = 100.0f;
            kirbyWorld.currentHP = 100.0f;
            cout << "Game Reset! Both characters back to full HP.\n";
            break;

        // Lighting system toggle
        case 'l': case 'L':
            enhancedLighting = !enhancedLighting;
            cout << "Enhanced lighting " << (enhancedLighting ? "enabled" : "disabled") << endl;
            break;

        // Snorlax attack controls
        case 'h': case 'H':
            if (!gameOver && (activeDisplay == SNORLAX_ONLY || activeDisplay == KIRBY_AND_SNORLAX || activeDisplay == ALL_THREE)) {
                snorlaxWorld.showBeam = !snorlaxWorld.showBeam;
                snorlaxWorld.mouthAnimating = !snorlaxWorld.mouthAnimating;
                snorlaxWorld.mouthIsOpen = !snorlaxWorld.mouthIsOpen;

                // Apply damage to Kirby
                if (snorlaxWorld.showBeam) {
                    kirbyWorld.currentHP -= kirbyWorld.maxHP * 0.25f;
                    if (kirbyWorld.currentHP < 0.0f) {
                        kirbyWorld.currentHP = 0.0f;
                    }
                    cout << "Snorlax attacks! Kirby HP: " << kirbyWorld.currentHP << "/" << kirbyWorld.maxHP << endl;
                    checkGameOver();
                }

                cout << "Snorlax beam/mouth animation toggled\n";
            }
            break;

        // Kirby ability controls
        case 'f': case 'F':
            if (!gameOver && (activeDisplay == KIRBY_ONLY || activeDisplay == KIRBY_AND_SNORLAX || activeDisplay == ALL_THREE)) {
                kirbyWorld.isFloating = !kirbyWorld.isFloating;
                cout << "Kirby floating toggled\n";
            }
            break;

        case 'b': case 'B':
            if (!gameOver && (activeDisplay == KIRBY_ONLY || activeDisplay == KIRBY_AND_SNORLAX || activeDisplay == ALL_THREE)) {
                if (kirbyWorld.isFloating) {
                    cout << "Kirby used Float Burst!\n";
                }
                kirbyWorld.triggerBurst();

                // Apply damage to Snorlax
                snorlaxWorld.currentHP -= snorlaxWorld.maxHP * 0.25f;
                if (snorlaxWorld.currentHP < 0.0f) {
                    snorlaxWorld.currentHP = 0.0f;
                }
                cout << "Kirby attacks! Snorlax HP: " << snorlaxWorld.currentHP << "/" << snorlaxWorld.maxHP << endl;
                checkGameOver();
            }
            break;

        case 27: exit(1); break;
    }

    world.move(xinc, yinc, zinc);
    glutPostRedisplay();
}

/**
 * Special key handler for function keys and arrow controls
 */
void mySpecialFunc(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_DOWN  : world.rotateX -= setting.angleInc;  break;
        case GLUT_KEY_UP    : world.rotateX += setting.angleInc;  break;
        case GLUT_KEY_LEFT  : world.rotateY -= setting.angleInc;  break;
        case GLUT_KEY_RIGHT : world.rotateY += setting.angleInc;  break;
        case GLUT_KEY_HOME  :
            myDataInit();
            gameOver = false;
            snorlaxWon = false;
            kirbyWon = false;
            snorlaxWorld.currentHP = 100.0f;
            kirbyWorld.currentHP = 100.0f;
            break;
        case GLUT_KEY_F1    :
            setting.shadingMode = !setting.shadingMode;
            if (setting.shadingMode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            cout << "Switched to " << (setting.shadingMode ? "FILL" : "WIREFRAME") << " mode" << endl;
            break;
        case GLUT_KEY_F2    :
            worldaxis.toggle();
            break;
        case GLUT_KEY_F3    :
            // Enhanced shading and lighting toggle
            smoothShading = !smoothShading;
            enhancedLighting = smoothShading;

            if (smoothShading) {
                glShadeModel(GL_SMOOTH);
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);
                glEnable(GL_LIGHT1);
                glEnable(GL_LIGHT2);
                cout << "Switched to SMOOTH shading with LIGHTING ENABLED" << endl;
            } else {
                glShadeModel(GL_FLAT);
                glDisable(GL_LIGHTING);
                glDisable(GL_LIGHT0);
                glDisable(GL_LIGHT1);
                glDisable(GL_LIGHT2);
                cout << "Switched to FLAT shading with LIGHTING DISABLED" << endl;
            }

            setting.smoothShading = smoothShading;
            setting.enhancedLighting = enhancedLighting;
            break;
    }
    glutPostRedisplay();
}

// ===== MOUSE INTERACTION SYSTEM =====
void myMouseFunc(int button, int state, int x, int y)
{
    y = window.height - y;
    switch (button)
    {
        case GLUT_RIGHT_BUTTON:
           if (state==GLUT_DOWN && !setting.mouseRightMode)
           {
              setting.mouseX = x;
              setting.mouseY = y;
              setting.mouseRightMode = true;
           }
           if (state==GLUT_UP && setting.mouseRightMode)
           {
              setting.mouseRightMode = false;
           }
           break;
        case GLUT_LEFT_BUTTON:
           if (state==GLUT_DOWN && !setting.mouseLeftMode)
           {
              setting.mouseX = x;
              setting.mouseY = y;
              setting.mouseLeftMode = true;
           }
           if (state==GLUT_UP &&  setting.mouseLeftMode)
           {
              setting.mouseLeftMode = false;
           }
           break;
    }
}

void myMotionFunc(int x, int y)
{
    y = window.height - y;
    GLint xinc = x - setting.mouseX;
    GLint yinc = y - setting.mouseY;

    if(setting.mouseRightMode)
    {
        world.rotate(0.0f, 0.0f, -xinc*0.5);
    }
    if(setting.mouseLeftMode)
    {
        world.rotate(-yinc*0.5, xinc*0.5, 0.0f);
    }

    setting.mouseX = x;
    setting.mouseY = y;
    glutPostRedisplay();
}

// ===== INITIALIZATION FUNCTIONS =====
void myReshapeFunc(int width, int height)
{
    window.width  = width;
    window.height = height;
    glViewport(0, 0, width, height);
}

void myDataInit()
{
    window.title = "TCG6223 Computer Graphics - Enhanced Combined Scene";
    window.posX = 100;
    window.posY = 100;
    window.width  = 800;
    window.height = 500;

    world.rotateX  = 0.0;
    world.rotateY  = 0.0;
    world.rotateZ  = 0.0;
    world.posX     = 0.0;
    world.posY     = 0.0;
    world.posZ     = 0.0;
    world.scaleX   = 1.0;
    world.scaleY   = 1.0;
    world.scaleZ   = 1.0;

    viewer.eyeX    = 0.0;
    viewer.eyeY    = 0.0;
    viewer.eyeZ    = 40.0;
    viewer.centerX = 0.0;
    viewer.centerY = 0.0;
    viewer.centerZ = 0.0;
    viewer.upX     = 0.0;
    viewer.upY     = 1.0;
    viewer.upZ     = 0.0;
    viewer.zNear   = 0.1;
    viewer.zFar    = 500.0;
    viewer.fieldOfView = 60.0;
    viewer.aspectRatio = static_cast<GLdouble> (window.width) / window.height;

    setting.posInc   = 1.0;
    setting.angleInc = 2.0;
    setting.mouseX   = 0;
    setting.mouseY   = 0;

    setting.mouseRightMode = false;
    setting.mouseLeftMode = false;

    setting.shadingMode = true;
    setting.smoothShading = true;
    setting.enhancedLighting = true;
}

void myViewingInit()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(viewer.fieldOfView,
                   viewer.aspectRatio,
                   viewer.zNear,
                   viewer.zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(viewer.eyeX,   viewer.eyeY,   viewer.eyeZ,
              viewer.centerX,viewer.centerY,viewer.centerZ,
              viewer.upX,    viewer.upY,    viewer.upZ );
}

void myLightingInit()
{
    static GLfloat  ambient[] = { 0.2f,  0.25f,  0.2f, 1.0f };
    static GLfloat  diffuse[] = { 0.8f,  0.8f,  0.7f, 1.0f };
    static GLfloat specular[] = { 0.6f,  0.6f,  0.5f, 1.0f };
    static GLfloat  specref[] = { 0.4f,  0.4f,  0.3f, 1.0f };
    static GLfloat position[] = {10.0f, 10.0f, 10.0f, 1.0f };
    short shininess = 64;

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
    glMateriali(GL_FRONT, GL_SHININESS, shininess);

    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
}

void myInit()
{
    myDataInit();

    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowPosition(window.posX, window.posY);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow(window.title.c_str());

    glutDisplayFunc(myDisplayFunc);
    glutReshapeFunc(myReshapeFunc);
    glutKeyboardFunc(myKeyboardFunc);
    glutSpecialFunc(mySpecialFunc);
    glutMotionFunc(myMotionFunc);
    glutMouseFunc(myMouseFunc);

    glPointSize(4.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glFrontFace(GL_CCW);
    glShadeModel (GL_SMOOTH);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    myViewingInit();
    myLightingInit();

    // Initialize all character virtual worlds
    snorlaxWorld.init();
    kirbyWorld.init();
    battlegroundWorld.init();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    myWelcome();
    myInit();
    glutMainLoop();
    return 0;
}

void myWelcome()
{
    cout << "*****************************************************************\n";
    cout << "*                   TCG6223 Computer Graphics                   *\n";
    cout << "*                  FIST, Multimedia University                  *\n";
    cout << "*                ENHANCED COMBINED SCENE VERSION                *\n";
    cout << "*****************************************************************\n";
    cout << "| Press:                                                        |\n";
    cout << "|   <a>,<d>,<w>,<s>,<q>,<e> => move world                       |\n";
    cout << "|   <arrows>                => rotate world                     |\n";
    cout << "|   HOME                    => restore defaults & reset game    |\n";
    cout << "|   ESC                     => exit                             |\n";
    cout << "|                                                               |\n";
    cout << "|   T => toggle display modes:                                  |\n";
    cout << "|       1. Snorlax Only                                         |\n";
    cout << "|       2. Kirby Only                                           |\n";
    cout << "|       3. BattleGround Only                                    |\n";
    cout << "|       4. Kirby and Snorlax                                    |\n";
    cout << "|       5. All Three (BattleGround + Characters)                |\n";
    cout << "|                                                               |\n";
    cout << "|   H => Snorlax beam/mouth animation (damages Kirby)           |\n";
    cout << "|   F => Kirby floating toggle                                  |\n";
    cout << "|   B => Kirby burst effect (damages Snorlax)                   |\n";
    cout << "|   R => Reset game (restore full HP to both characters)        |\n";
    cout << "|   L => Toggle enhanced lighting (dimmer/brighter)             |\n";
    cout << "|                                                               |\n";
    cout << "|   F1 => toggle shading / wire-frame mode                      |\n";
    cout << "|   F2 => toggle rendering of axes                              |\n";
    cout << "|   F3 => toggle smooth / flat shading (ENHANCED)               |\n";
    cout << "|                                                               |\n";
    cout << "| Mouse (Left Drag or Right Drag) => rotate world               |\n";
    cout << "|                                                               |\n";
    cout << "| GAME OVER: When a character's HP reaches 0, press R to        |\n";
    cout << "|            restart the battle!                                |\n";
    cout << "*****************************************************************\n";
    cout << "|        H A V E   F U N   W I T H   E N H A N C E D  !!!       |\n";
    cout << "*****************************************************************\n";
}
