#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "Snorlax.hpp"


using namespace Snorlax;

float deform(float y) {
    float normalizedY = (y + 1.0f) / 2.0f;
    return 0.9f + 0.25f * sinf(normalizedY * M_PI);
}


MyVirtualWorld::MyVirtualWorld() {
    bodyRadius = 6.0f;
    body = gluNewQuadric();
    slices = 32;
    stacks = 32;

    // Initialize HP values
    currentHP = 100.0f;
    maxHP = 100.0f;
}

void MyVirtualWorld::init() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // IMPROVED LIGHTING SETUP - Balanced with nice reflections

    // Main light (key light) - Enhanced for better reflections
    GLfloat light0_pos[] = { 5.0f, 15.0f, 10.0f, 1.0f };
    GLfloat light0_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };      // Slightly increased
    GLfloat light0_diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };      // Increased for better lighting
    GLfloat light0_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };     // Good specular for reflections

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // Fill light - Enhanced to reduce harsh shadows
    GLfloat light1_pos[] = { -10.0f, 8.0f, 5.0f, 1.0f };
    GLfloat light1_ambient[] = { 0.15f, 0.15f, 0.15f, 1.0f };   // Slightly increased
    GLfloat light1_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };      // Good fill light
    GLfloat light1_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };     // Moderate specular

    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

    // Back light - Enhanced rim lighting
    GLfloat light2_pos[] = { 0.0f, 10.0f, -10.0f, 1.0f };
    GLfloat light2_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat light2_diffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat light2_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };     // Enhanced for rim light

    glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);

    // Global ambient - Balanced
    GLfloat global_ambient[] = { 0.15f, 0.15f, 0.15f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // IMPROVED MATERIAL PROPERTIES - Nice shiny but not too shiny
    GLfloat mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };         // Good ambient response
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };         // Good diffuse response
    GLfloat mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };        // Moderate specular - not too shiny
    GLfloat mat_shininess[] = { 40.0f };                         // Perfect shininess level

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void MyVirtualWorld::tickTime() {

    if (mouthAnimating) {
        float speed = 0.04f; // Increased from 0.02f - faster mouth opening/closing

        if (mouthIsOpen) {
            if (mouthOpenProgress < 1.0f)
                mouthOpenProgress += speed;
            else {
                mouthOpenProgress = 1.0f;
                mouthAnimating = false;
            }
        } else {
            if (mouthOpenProgress > 0.0f)
                mouthOpenProgress -= speed;
            else {
                mouthOpenProgress = 0.0f;
                mouthAnimating = false;
            }
        }
    }

    if (showBeam) {
        if (!beamFlashActive) {
            // Start the beam flash if it's not already active
            beamFlashActive = true;
            beamFlashRadius = 0.0f;
            beamFlashTimer = 0.0f;  // Reset the timer
            beamFlashVisible = true;  // Make the flash visible
        }

        // Beam Flash Timer - If flash has been visible for duration, deactivate it
        if (beamFlashVisible) {
            beamFlashTimer += 0.05f;  // Increased from 0.035f - faster timer

            if (beamFlashTimer >= beamFlashDuration) {
                // Stop the beam flash after duration
                beamFlashVisible = false;
                beamFlashRadius = 0.0f;  // Reset the flash radius
            } else {
                // Increase the flash radius as long as the flash is visible
                if (beamFlashRadius < maxBeamFlashRadius) {
                    beamFlashRadius += beamFlashSpeed;  // Grow the flash radius
                }
            }
        }

        // After the flash disappears, the hyper beam starts growing
        if (!beamFlashVisible) {
            if (beamLength < maxBeamLength) {
                beamLength += beamSpeed;  // Increase the beam length over time
            }
        }
    } else {
        // Reset everything if showBeam is false
        beamLength = 0.0f;
        beamFlashActive = false;
        beamFlashVisible = false;
    }
}

void MyVirtualWorld::draw() {
    drawBody();
    drawArms();
    drawLegs();
    drawHead();
    drawMouth();
    drawHyperBeam();
    drawBeamFlash();
}

void MyVirtualWorld::drawBody() {
    // Main Body
    const int stacks = 60;
    const int slices = 60;
    const float radius = 1.0f;
    const float yStrecth = 2.0f;

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(6.0f, 6.0f * yStrecth, 5.0f);
    glColor3f(0.1608f, 0.4510f, 0.5569f);

    for (int i = 0; i < stacks; ++i) {
        float phi1 = M_PI * i / stacks;
        float phi2 = M_PI * (i + 1) / stacks;

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2 * M_PI * j / slices;

            for (int k = 0; k < 2; ++k) {
                float phi = (k == 0) ? phi1 : phi2;

                float y = radius * cos(phi);
                float deformFactor = deform(y);

                float x = radius * sin(phi) * cos(theta) * deformFactor;
                float z = radius * sin(phi) * sin(theta) * deformFactor;

                float yNormalized = (y + 0.5f) / 2.0f;
                float squashFactor = 1.0f;
                if (y < 0.0f) {
                    squashFactor = 0.4f + 0.3f * sinf((yNormalized) * M_PI_2); // Smooth squash for lower half
                } else {
                    squashFactor = 0.9f;
                }

                float ySmoothed = y * squashFactor;
                float nx = sin(phi) * cos(theta);
                float ny = cos(phi);
                float nz = sin(phi) * sin(theta);
                glNormal3f(nx, ny, nz);

                glVertex3f(x, ySmoothed, z);
            }
        }
        glEnd();
    }
    glPopMatrix();

    // Inner Body - SUPER BRIGHT, LIGHT, AND SHINY BELLY
    const float inner_radius = 0.9f;

    // ENHANCED SUPER BRIGHT MATERIAL for the belly area
    GLfloat belly_ambient[] = { 0.6f, 0.6f, 0.55f, 1.0f };      // Much brighter ambient
    GLfloat belly_diffuse[] = { 1.0f, 0.98f, 0.95f, 1.0f };     // MUCH LIGHTER cream color
    GLfloat belly_specular[] = { 0.95f, 0.95f, 0.9f, 1.0f };    // VERY HIGH specular for maximum shine
    GLfloat belly_shininess[] = { 100.0f };                      // SUPER shiny

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, belly_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, belly_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, belly_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, belly_shininess);

    glPushMatrix();
    glRotatef(-13, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 2.3f, 3.5f);
    glScalef(5.0f, 4.5f * yStrecth, 3.0f);
    glColor3f(1.0f, 0.98f, 0.95f);  // MUCH LIGHTER and BRIGHTER cream color

    for (int i = 0; i < stacks; ++i) {
        float phi1 = M_PI * i / stacks;
        float phi2 = M_PI * (i + 1) / stacks;

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2 * M_PI * j / slices;

            for (int k = 0; k < 2; ++k) {
                float phi = (k == 0) ? phi1 : phi2;

                float y = inner_radius * cos(phi);
                float deformFactor = deform(y);
                float x = inner_radius * sin(phi) * cos(theta) * deformFactor;
                float z = inner_radius * sin(phi) * sin(theta) * deformFactor;

                float yNormalized = (y + 0.5f) / 2.0f;
                float squashFactor = 1.0f;
                if (y < 0.0f) {
                    squashFactor = 0.75f + 0.3f * sinf((yNormalized) * M_PI_2);
                } else {
                    squashFactor = 0.9f;
                }

                float ySmoothed = y * squashFactor;
                float nx = sin(phi) * cos(theta);
                float ny = cos(phi);
                float nz = sin(phi) * sin(theta);
                glNormal3f(nx, ny, nz);

                glVertex3f(x, ySmoothed, z);
            }
        }
        glEnd();
    }
    glPopMatrix();

    // RESTORE default material properties after shiny belly
    GLfloat default_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat default_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat default_shininess[] = { 40.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, default_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, default_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, default_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, default_shininess);
}

void MyVirtualWorld::drawArms() {

    // right Arm with pendulum animation
    glPushMatrix();
    glTranslatef(bodyRadius * 0.9f, 6.0f, bodyRadius * 0.0f);
    glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);        // Base rotation
    glScalef(6.0f, 2.0f, 2.5f);
    glColor3f(0.1608f, 0.4510f, 0.5569f);
    glutSolidSphere(bodyRadius / 8.0f, 25, 25);
    glPopMatrix();

    // left Arm with pendulum animation
    glPushMatrix();
    glTranslatef(-bodyRadius * 0.9f, 6.0f, bodyRadius * 0.0f);
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);          // Base rotation
    glScalef(6.0f, 2.0f, 2.5f);
    glColor3f(0.1608f, 0.4510f, 0.5569f);
    glutSolidSphere(bodyRadius / 8.0f, 25, 25);
    glPopMatrix();

    // Left Claws
    glPushMatrix();
    glTranslatef(-8.0f, 3.5f, 1.0f);
    glRotatef(-45, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.5f, 2.7f, 2.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-8.5f, 3.2f, 0.5f);
    glRotatef(-70, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.5f, 2.7f, 2.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-8.6f, 3.0f, -0.0f);
    glRotatef(-90, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.2f, 2.4f, 2.2f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-8.5f, 3.1f, -0.5f);
    glRotatef(-105, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.5f, 2.7f, 2.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-8.2f, 3.4f, -0.8f);
    glRotatef(-150, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.5f, 2.7f, 2.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();

    // Right Claws
    glPushMatrix();
    glTranslatef(8.0f, 3.5f, 1.0f);
    glRotatef(45, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.5f, 2.7f, 2.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.5f, 3.2f, 0.5f);
    glRotatef(70, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.5f, 2.7f, 2.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.6f, 3.0f, -0.0f);
    glRotatef(90, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.2f, 2.4f, 2.2f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.5f, 3.1f, -0.5f);
    glRotatef(105, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.5f, 2.7f, 2.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.2f, 3.4f, -0.8f);
    glRotatef(150, 0.0f, 1.0f, 0.0f);
    glRotatef(45, 1.0f, 0.0f, 0.0f);
    glScalef(2.5f, 2.7f, 2.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.2f, 10, 10);
    glPopMatrix();
}

void MyVirtualWorld::drawLegs() {
    // Legs
    const int stacks = 30;
    const int slices = 30;
    const float flattenFactor = 0.5f;
    const float legSize = bodyRadius * 0.34f;
    const float zForward = 1.0f;

    for (int i = 0; i < 2; ++i) {
        float xOffset = (i == 0) ? -legSize * 1.9f : legSize * 1.9f;
        float rotationAngle = (i == 0) ? -15.0f : 15.0f;

        glPushMatrix();
        glTranslatef(xOffset, -legSize * 1.6f, legSize * 0.0f + zForward);
        glRotatef(90, 0.0f, 1.0f, 0.0f);

        // SAME MATERIAL as head and belly for legs
        GLfloat leg_ambient[] = { 0.4f, 0.4f, 0.35f, 1.0f };
        GLfloat leg_diffuse[] = { 0.9451f, 0.8980f, 0.8431f, 1.0f }; // Same cream color
        GLfloat leg_specular[] = { 0.8f, 0.8f, 0.7f, 1.0f };
        GLfloat leg_shininess[] = { 70.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, leg_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, leg_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, leg_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, leg_shininess);

        glScalef(1.3f, flattenFactor, 1.0f);
        glColor3f(0.9451f, 0.8980f, 0.8431f);  // SAME cream color as head and belly
        glutSolidSphere(legSize, slices, stacks);

        glPopMatrix();
    }

    // RESTORE default material properties for other parts (like claws)
    GLfloat default_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat default_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat default_shininess[] = { 40.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, default_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, default_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, default_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, default_shininess);

    // Left Claws
    glPushMatrix();
    glTranslatef(-2.9f, -3.4f, 3.1f);
    glRotatef(30, 0.0f, 1.0f, 0.0f);
    glRotatef(0, 1.0f, 0.0f, 0.0f);
    glScalef(3.5f, 3.7f, 3.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.3f, 15, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4.0f, -3.3f, 3.5);
    glRotatef(0, 0.0f, 1.0f, 0.0f);
    glRotatef(0, 1.0f, 0.0f, 0.0f);
    glScalef(3.5f, 3.7f, 3.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.3f, 15, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-5.0f, -3.3f, 3.1f);
    glRotatef(-30, 0.0f, 1.0f, 0.0f);
    glRotatef(0, 1.0f, 0.0f, 0.0f);
    glScalef(3.5f, 3.7f, 3.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.3f, 15, 10);
    glPopMatrix();

    // Right claws
    glPushMatrix();
    glTranslatef(2.9f, -3.4f, 3.1f);
    glRotatef(-20, 0.0f, 1.0f, 0.0f);
    glRotatef(0, 1.0f, 0.0f, 0.0f);
    glScalef(3.5f, 3.7f, 3.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.3f, 15, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.0f, -3.3f, 3.5f);
    glRotatef(0, 0.0f, 1.0f, 0.0f);
    glRotatef(0, 1.0f, 0.0f, 0.0f);
    glScalef(3.5f, 3.7f, 3.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.3f, 15, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.0f, -3.3f, 3.1f);
    glRotatef(30, 0.0f, 1.0f, 0.0f);
    glRotatef(0, 1.0f, 0.0f, 0.0f);
    glScalef(3.5f, 3.7f, 3.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCone(0.1f, 0.3f, 15, 10);
    glPopMatrix();

    // Left inner paw
    glPushMatrix();
    glTranslatef(-4.0f, -4.2f, 0.8f);
    glRotatef(-20, 0, 1, 0);
    glRotatef(2, 1, 0, 0);
    glScalef(1.0f, 0.2f, 1.0f);
    glColor3f(0.4980f, 0.4353f, 0.3843f);
    glutSolidSphere(1.3f, 20, 20);
    glPopMatrix();

    // Right paw
    glPushMatrix();
    glTranslatef(4.0f, -4.2f, 0.8f);
    glRotatef(-20, 0, 1, 0);
    glRotatef(2, 1, 0, 0);
    glScalef(1.0f, 0.2f, 1.0f);
    glColor3f(0.4980f, 0.4353f, 0.3843f);
    glutSolidSphere(1.3f, 20, 20);
    glPopMatrix();
}

void MyVirtualWorld::drawHead() {
    // Main head
    glPushMatrix();
    glTranslatef(0.0f, 10.8f, 0.0f);
    glScalef(2.0f, 2.4f, 2.0f);
    glColor3f(0.1608f, 0.4510f, 0.5569f);
    glutSolidSphere(1.5f, 32, 32);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glPushMatrix();
    glTranslatef(0.0f, 0.8f, 0.8f);
    glRotatef(180, 1, 0, 0);
    glRotatef(-135, 0, 0, 1);

    float thickness = 0.5f;

    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.6f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.6f, 0.0f);
    glEnd();

    // Cutout Prism
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.0f, -thickness);
    glVertex3f(0.6f, 0.0f, -thickness);
    glVertex3f(0.0f, 0.6f, -thickness);
    glEnd();

    glBegin(GL_QUADS);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.6f, 0.0f, 0.0f);
    glVertex3f(0.6f, 0.0f, -thickness);
    glVertex3f(0.0f, 0.0f, -thickness);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.6f, 0.0f);
    glVertex3f(0.0f, 0.6f, -thickness);
    glVertex3f(0.0f, 0.0f, -thickness);

    glVertex3f(0.6f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.6f, 0.0f);
    glVertex3f(0.0f, 0.6f, -thickness);
    glVertex3f(0.6f, 0.0f, -thickness);

    glEnd();

    glPopMatrix();

    glDisable(GL_CULL_FACE);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    // SAME SHINY MATERIAL for head face as belly
    GLfloat face_ambient[] = { 0.4f, 0.4f, 0.35f, 1.0f };       // Same as belly
    GLfloat face_diffuse[] = { 0.9451f, 0.8980f, 0.8431f, 1.0f }; // Original cream color
    GLfloat face_specular[] = { 0.8f, 0.8f, 0.7f, 1.0f };       // Same shininess as belly
    GLfloat face_shininess[] = { 70.0f };                        // Same shininess level

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, face_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, face_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, face_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, face_shininess);

    glPushMatrix();
    glTranslatef(-0.0f, 0.0f, 0.65f);
    glScalef(0.85f, 0.8f, 0.6f);
    glColor3f(0.9451f, 0.8980f, 0.8431f);  // Original cream color
    glutSolidSphere(1.5f, 32, 32);
    glPopMatrix();

    glDisable(GL_STENCIL_TEST);

    // Left Eye
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5f);
    glTranslatef(-0.7f, 0.5f, 1.3f);
    glRotatef(-22, 0, 1, 0);
    glRotatef(5, 0, 0, 1);

    float scale = 0.5f;
    float curveFlattening = 0.3f;

    glBegin(GL_LINE_STRIP);
    for (float t = 0.0f; t <= 1.0f; t += 0.01f) {
        float x = (1 - t) * (1 - t) * 0.0f + 2 * (1 - t) * t * 0.5f + t * t * 1.0f;
        float y = (1 - t) * (1 - t) * 0.0f + 2 * (1 - t) * t * (0.2f * curveFlattening) + t * t * 0.0f;
        glVertex3f(x * scale, y * scale, 0.0f);
    }
    glEnd();

    // Right Eye
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5f);
    glTranslatef(0.9f, -0.05f, -0.1f);
    glRotatef(30, 0, 1, 0);
    glRotatef(-2, 0, 0, 1);

    glBegin(GL_LINE_STRIP);
    for (float t = 0.0f; t <= 1.0f; t += 0.01f) {
        float x = (1 - t) * (1 - t) * 0.0f + 2 * (1 - t) * t * 0.5f + t * t * 1.0f;
        float y = (1 - t) * (1 - t) * 0.0f + 2 * (1 - t) * t * (0.2f * curveFlattening) + t * t * 0.0f;
        glVertex3f(x * scale, y * scale, 0.0f);
    }
    glEnd();

    glPopMatrix();

    // Left Ear
    glPushMatrix();
    glTranslatef(-1.5f, 13.5f, -0.1f);
    glRotatef(-100, 0.0f, 1.0f, 0.0f);
    glRotatef(-60, 1.0f, 0.0f, 0.0f);
    glScalef(3.5f, 3.7f, 3.5f);
    glColor3f(0.1608f, 0.4510f, 0.5569f);
    glutSolidCone(0.25f, 0.4f, 12, 10);
    glPopMatrix();

    // Right ear
    glPushMatrix();
    glTranslatef(1.5f, 13.5f, -0.1f);
    glRotatef(100, 0.0f, 1.0f, 0.0f);
    glRotatef(-60, 1.0f, 0.0f, 0.0f);
    glScalef(3.5f, 3.7f, 3.5f);
    glColor3f(0.1608f, 0.4510f, 0.5569f);
    glutSolidCone(0.25f, 0.4f, 12, 10);
    glPopMatrix();
}

void MyVirtualWorld::drawMouth() {
    // Define animation progress for the mouth (from 0 = closed to 1 = open)
    float mouthOpenProgress = this->mouthOpenProgress; // Controlled by tickTime() and keyboard input

    // Mouth Shape: When closed, it's a smile curve; when open, it becomes an oval.
    float radiusX = 0.28f + 0.0f * mouthOpenProgress;  // Horizontal radius grows as the mouth opens
    float radiusY = 0.0f + 0.28f * mouthOpenProgress;  // Vertical radius increases as the mouth opens

    // Position the mouth
    glPushMatrix();
    glTranslatef(0.0f, 11.3f, -0.04f);  // Translate to Snorlax's head position
    glScalef(2.0f, 2.0f, 2.0f);  // Scale appropriately

    // Create the mouth's curve or circle
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.6f);  // Position the mouth further out
    glRotatef(180, 0, 1, 0);  // Rotate to face the viewer
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.5f);

    // Draw mouth as a flattened circle (ellipse) or smile curve
    glBegin(GL_LINE_STRIP);
    int segments = 100;  // Number of segments for smooth circle
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radiusX * cos(angle);
        float y = radiusY * sin(angle);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();

    glPopMatrix();
    glPopMatrix();

    // TEETH - Slightly lighter grey for perfect balance
    // Teeth animation: Move the teeth downward as the mouth opens
    float teethOffset = -0.4f * mouthOpenProgress;
    float teethRotation = -30.0f * mouthOpenProgress;

    // SLIGHTLY LIGHTER GREY material for perfect contrast
    GLfloat teeth_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };       // Slightly lighter grey ambient
    GLfloat teeth_diffuse[] = { 0.55f, 0.55f, 0.55f, 1.0f };    // Slightly lighter grey diffuse
    GLfloat teeth_specular[] = { 0.75f, 0.75f, 0.75f, 1.0f };   // Good specular for shine
    GLfloat teeth_emission[] = { 0.03f, 0.03f, 0.03f, 1.0f };   // Minimal glow
    GLfloat teeth_shininess[] = { 65.0f };                       // Good shininess

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, teeth_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, teeth_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, teeth_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, teeth_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, teeth_shininess);

    // Set slightly lighter grey color
    glColor3f(0.55f, 0.55f, 0.55f);  // Slightly lighter grey - perfect balance

    // Left Tooth - EXACT ORIGINAL SIZE AND POSITION
    glPushMatrix();
    glTranslatef(-0.41f, 11.3f + teethOffset, 3.0f);  // Original position
    glRotatef(-100, 0.0f, 1.0f, 0.0f);
    glRotatef(-95, 1.0f, 0.0f, 0.0f);
    glRotatef(teethRotation, 1.0f, 0.0f, 0.0f);
    glScalef(3.0f, 3.2f, 3.0f);  // Original scale
    glutSolidCone(0.05f, 0.13f, 12, 10);  // Original size
    glPopMatrix();

    // Right Tooth - EXACT ORIGINAL SIZE AND POSITION
    glPushMatrix();
    glTranslatef(0.41f, 11.3f + teethOffset, 3.0f);  // Original position
    glRotatef(-100, 0.0f, 1.0f, 0.0f);
    glRotatef(-95, 1.0f, 0.0f, 0.0f);
    glRotatef(-teethRotation + 10, 1.0f, 0.0f, 1.0f);
    glScalef(3.0f, 3.2f, 3.0f);  // Original scale
    glutSolidCone(0.05f, 0.13f, 12, 10);  // Original size
    glPopMatrix();

    // Restore default material properties (clean up emission)
    GLfloat default_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat default_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat default_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };    // Remove emission
    GLfloat default_shininess[] = { 40.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, default_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, default_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, default_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, default_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, default_shininess);
}

void MyVirtualWorld::drawHyperBeam() {
    if (!showBeam || beamFlashVisible) return;  // Don't show the beam while flash is active

    // Start the Hyper Beam at Snorlax's mouth position
    glPushMatrix();
    glTranslatef(0.0f, 11.2f, 3.3f); // Position at Snorlax's mouth
    glRotatef(20, 1.0f, 0.0f, 0.0f);
    // Enable blending for aura
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Core beam (bright orange)
    glColor4f(1.0f, 0.6f, 0.0f, 0.15f); // Orange
    GLUquadric* quad = gluNewQuadric();
    float baseRadius = 0.4f;
    float flareFactor = 0.1f;
    float auraScale = 2.0f + 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.02f);  // Pulse effect
    float topRadius = baseRadius + beamLength * flareFactor;

    // Drawing the core of the beam (grows as the beam length increases)
    gluCylinder(quad, baseRadius, topRadius, beamLength, 20, 1);

    // Aura layers (semi-transparent)
    glColor4f(1.0f, 1.0f, 0.2f, 0.3f); // Yellow with alpha
    gluCylinder(quad, baseRadius * 1.5f, topRadius * 1.5f, beamLength, 20, 1);

    glColor4f(1.0f, 1.0f, 0.5f, 0.15f); // Fainter outer aura
    gluCylinder(quad, baseRadius * 2.0f, topRadius * 2.0f, beamLength, 20, 1);

    glColor4f(1.0f, 1.0f, 0.3f, 0.25f);  // Fading glow aura
    gluCylinder(quad, baseRadius * auraScale, topRadius * auraScale, beamLength, 20, 1);

    gluDeleteQuadric(quad);
    glDisable(GL_BLEND);

    glPopMatrix();

}


void MyVirtualWorld::drawBeamFlash() {
    if (!beamFlashVisible) return;  // Only draw the flash if it is visible

    glPushMatrix();
    glTranslatef(0.0f, 11.3f, 4.0f);  // Slightly ahead of Snorlax's mouth to simulate flash after firing

    // Enable blending for aura effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLUquadric* quad = gluNewQuadric();

    // Core glowing flash
    glColor4f(1.0f, 0.9f, 0.2f, 0.7f * (1.0f - beamFlashRadius / maxBeamFlashRadius));  // Fade out
    gluSphere(quad, beamFlashRadius, 20, 20);

    // Aura layer 1 - bright yellow
    glColor4f(1.0f, 1.0f, 0.2f, 0.3f);
    gluSphere(quad, beamFlashRadius * 1.5f, 20, 20);

    // Aura layer 2 - softer white/yellow
    glColor4f(1.0f, 1.0f, 0.5f, 0.15f);
    gluSphere(quad, beamFlashRadius * 2.0f, 20, 20);

    // Aura layer 3 - animated pulsating glow
    float auraScale = 2.5f + 0.3f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f);
    glColor4f(1.0f, 1.0f, 0.3f, 0.25f);
    gluSphere(quad, beamFlashRadius * auraScale, 20, 20);

    gluDeleteQuadric(quad);
    glDisable(GL_BLEND);

    glPopMatrix();
}
