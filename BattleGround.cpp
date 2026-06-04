#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "BattleGround.hpp"
#include "Snorlax.hpp"
#include "Kirby.hpp"

// Add extern declarations to access character instances
extern Snorlax::MyVirtualWorld snorlaxWorld;
extern Kirby::MyVirtualWorld kirbyWorld;

using namespace BattleGround;

// Define static constants
const float MyVirtualWorld::platform_radius = 5.0f;
const float MyVirtualWorld::platform_height = 0.07f;
const int MyVirtualWorld::platform_segments = 120;
const int MyVirtualWorld::star_points = 8;
const float MyVirtualWorld::star_outer_radius = 1.9f;
const float MyVirtualWorld::star_inner_radius = 0.74f;
bool lightingEnabled = true;

MyVirtualWorld::MyVirtualWorld() {
    elapsedTime = 0.0f;
    // Increased from 4 to 8 floating stones
    for (int i = 0; i < 16; ++i) {
        stoneZ[i] = 0.0f;
        stoneAngle[i] = 0.0f;
    }
    // Increased from 5 to 12 clouds
    for (int i = 0; i < 12; ++i) {
        cloudX[i] = 0.0f;
    }
}

void MyVirtualWorld::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Enhanced background color - brighter
    glClearColor(0.2f, 0.4f, 0.6f, 1.0f); // Brighter blue fallback
    glDisable(GL_CULL_FACE);

    // Enhanced lighting setup
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1); // Secondary light
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // Main light - brighter and warmer
    GLfloat light0_ambient[] = { 0.3f, 0.35f, 0.3f, 1.0f };
    GLfloat light0_diffuse[] = { 0.7f, 0.7f, 0.6f, 1.0f }; // Brighter, warmer
    GLfloat light0_specular[] = { 0.5f, 0.5f, 0.4f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // Secondary fill light - brighter
    GLfloat light1_ambient[] = { 0.2f, 0.25f, 0.2f, 1.0f };
    GLfloat light1_diffuse[] = { 0.3f, 0.35f, 0.3f, 1.0f };
    GLfloat light1_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

    // Brighter global ambient light
    GLfloat global_ambient[] = { 0.25f, 0.3f, 0.35f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // Enhanced material properties for better shading
    GLfloat mat_shininess[] = { 40.0f };
    GLfloat mat_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void MyVirtualWorld::drawGradientBackground() {
    // Save current states
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);

    // Save current matrices and disable lighting for background
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Enhanced gradient with brighter, more visible colors
    glBegin(GL_QUADS);
        // Top vertices (brighter blue sky)
        glColor3f(0.2f, 0.5f, 0.8f);  // Brighter blue at top
        glVertex2f(-1.0f, 1.0f);      // Top-left
        glVertex2f(1.0f, 1.0f);       // Top-right
        // Bottom vertices (warmer golden horizon)
        glColor3f(1.0f, 0.8f, 0.3f);  // Brighter golden yellow at bottom
        glVertex2f(1.0f, -1.0f);      // Bottom-right
        glVertex2f(-1.0f, -1.0f);     // Bottom-left
    glEnd();

    // Restore states to their previous values
    if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
    if (lightingEnabled) glEnable(GL_LIGHTING);

    // Restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void MyVirtualWorld::tickTime() {
    elapsedTime += 0.016f; // approx 60 FPS frame time

    // Enhanced floating stones - 8 stones with varied movement patterns
    float baseZ[16] = {2.0f, 2.2f, 0.9f, 1.3f, 2.8f, 1.7f, 2.5f, 1.1f, 2.0f, 2.2f, 0.9f, 1.3f, 2.8f, 1.7f, 2.5f, 1.1f};
    float phase[16] = {0.4f, 0.9f, 1.6f, 2.7f, 3.2f, 1.1f, 2.1f, 3.8f, 0.4f, 0.9f, 1.6f, 2.7f, 3.2f, 1.1f, 2.1f, 3.8f};

    for (int i = 0; i < 16; ++i) {
        // Varied floating speeds and amplitudes for more dynamic movement
        float speed = 1.1f + (i % 3) * 0.2f;
        float amplitude = 0.7f + (i % 4) * 0.3f;
        stoneZ[i] = baseZ[i] + amplitude * sin(elapsedTime * speed + phase[i]);
        stoneAngle[i] = fmod(elapsedTime * (35 + i * 5) + phase[i] * 60, 360.0f);
    }

    // Enhanced clouds - 12 clouds with more varied positions and movements
    float cloudBaseX[12] = {-8.0f, 8.0f, 0.0f, 7.5f, -7.7f, -4.2f, 5.3f, -9.5f, 9.8f, -2.1f, 3.8f, -6.4f};
    float cloudPhase[12] = {0.2f, 0.9f, 2.2f, 1.5f, 2.9f, 0.7f, 1.8f, 3.1f, 0.5f, 2.4f, 1.3f, 2.8f};
    float cloudSpeed[12] = {0.11f, 0.16f, 0.13f, 0.14f, 0.12f, 0.15f, 0.10f, 0.17f, 0.12f, 0.14f, 0.11f, 0.13f};

    for (int i = 0; i < 12; ++i) {
        // Varied cloud movement patterns
        float movement_range = 3.0f + (i % 3) * 1.0f;
        cloudX[i] = cloudBaseX[i] + sin(elapsedTime * cloudSpeed[i] + cloudPhase[i]) * movement_range;
    }
}

void MyVirtualWorld::draw() {
    // Draw gradient background first
    drawGradientBackground();

    // Set enhanced light positions
    GLfloat light0_position[] = { 1.0f, 1.0f, 0.5f, 0.0f }; // Main directional light
    GLfloat light1_position[] = { -0.5f, 0.8f, 0.3f, 0.0f }; // Fill light
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    drawBattlePlatform();

    // Draw enhanced clouds with varied sizes and heights
    drawCloud3D(cloudX[0], 10.0f, 4.5f, 1.4f);     // Original clouds
    drawCloud3D(cloudX[1], 12.0f, 6.2f, 1.16f);
    drawCloud3D(cloudX[2], 13.5f, 4.7f, 1.8f);
    drawCloud3D(cloudX[3], -6.8f, 5.9f, 1.35f);
    drawCloud3D(cloudX[4], -7.7f, 5.6f, 1.05f);

    // New additional clouds at various heights and sizes
    drawCloud3D(cloudX[5], 8.5f, 7.2f, 0.9f);      // Smaller, higher clouds
    drawCloud3D(cloudX[6], -9.2f, 8.1f, 1.2f);
    drawCloud3D(cloudX[7], 11.3f, 3.8f, 1.6f);     // Lower, larger clouds
    drawCloud3D(cloudX[8], -5.1f, 9.5f, 0.8f);     // High small clouds
    drawCloud3D(cloudX[9], 2.7f, 7.8f, 1.1f);      // Mid-level clouds
    drawCloud3D(cloudX[10], -11.2f, 6.4f, 1.3f);   // Far clouds
    drawCloud3D(cloudX[11], 6.8f, 8.7f, 0.95f);    // High clouds

    // Draw enhanced floating stones with more varied positions and VIBRANT COLORS
    drawFloatingStone(-6.0f, 3.0f, stoneZ[0], 0.92f, stoneAngle[0]);     // Original stones
    drawFloatingStone(6.0f, -5.0f, stoneZ[1], 0.74f, stoneAngle[1]);
    drawFloatingStone(3.8f, 6.0f, stoneZ[2], 0.89f, stoneAngle[2]);
    drawFloatingStone(-7.6f, -8.0f, stoneZ[3], 1.07f, stoneAngle[3]);
    drawFloatingStone(8.5f, 2.3f, stoneZ[4], 0.65f, stoneAngle[4]);      // Smaller stones
    drawFloatingStone(-4.2f, -6.7f, stoneZ[5], 1.15f, stoneAngle[5]);    // Larger stone
    drawFloatingStone(1.8f, 8.9f, stoneZ[6], 0.78f, stoneAngle[6]);      // Medium stones
    drawFloatingStone(-9.1f, 4.5f, stoneZ[7], 0.85f, stoneAngle[7]);     // Varied placement

    drawFloatingStone(-1.0f, 10.0f, stoneZ[8], 0.85f, stoneAngle[8]);     // Original stones
    drawFloatingStone(8.0f, -9.0f, stoneZ[9], 0.67f, stoneAngle[9]);
    drawFloatingStone(6.8f, 10.0f, stoneZ[10], 0.99f, stoneAngle[10]);
    drawFloatingStone(-9.6f, -4.0f, stoneZ[11], 1.12f, stoneAngle[11]);
    drawFloatingStone(15.5f, 4.3f, stoneZ[12], 0.78f, stoneAngle[12]);      // Smaller stones
    drawFloatingStone(-6.2f, -9.8f, stoneZ[13], 1.99f, stoneAngle[13]);    // Larger stone
    drawFloatingStone(5.0f, 11.3f, stoneZ[14], 0.63f, stoneAngle[14]);      // Medium stones
    drawFloatingStone(-10.6f, 7.0f, stoneZ[15], 0.74f, stoneAngle[15]);     // Varied placement

    // =====  Set up 2D Orthogonal Projection =====
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600); // Match your window size

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Snorlax HP Bar
    glColor3f(0.8549f, 0.8471f, 0.8588f);
    drawOuterHPBar(115.0f, 150.0f, 195.0f, 30.0f);

    // Use actual Snorlax HP values
    drawHPBar(115.0f, 150.0f, 180.0f, 20.0f, snorlaxWorld.currentHP, snorlaxWorld.maxHP);

    //Snorlax AttackBar
    glColor3f(0.0078f, 0.7569f, 0.7294f);
    drawAttackBar(98.0f, 102.0f, 180.0f, 15.0f);

    //Kirby AttackBar
    glColor3f(0.0078f, 0.7569f, 0.7294f);
    drawAttackBar(702.0f, 102.0f, 180.0f, 15.0f);

    //Snorlax DefenseBar
    glColor3f(1.0f, 0.8667f, 0.3412f);
    drawAttackBar(75.0f, 61.0f, 135.0f, 14.0f);

    //Kirby DefenseBar
    glColor3f(1.0f, 0.8667f, 0.3412f);
    drawAttackBar(724.0f, 61.0f, 135.0f, 14.0f);

    //Snorlax SpeedBar
    glColor3f(1.0f, 0.498f, 0.059f);
    drawAttackBar(38.0f, 21.0f, 60.0f, 14.0f);

    //Kirby SpeedBar
    glColor3f(1.0f, 0.498f, 0.059f);
    drawAttackBar(716.0f, 21.0f, 150.0f, 14.0f);

    //Kirby HPBar
    glColor3f(0.8549f, 0.8471f, 0.8588f);
    drawOuterHPBar(680.0f, 150.0f, 195.0f, 30.0f);
    glPushMatrix();
    glTranslatef(683.0f * 2, 0, 0);  // Mirror position
    glScalef(-1, 1, 1);
    // Use actual Kirby HP values
    drawKirbyHPBar(688.0f, 150.0f, 180.0f, 20.0f, kirbyWorld.currentHP, kirbyWorld.maxHP);
    glPopMatrix();

    //Pokéball
    glPushMatrix();
    glTranslatef(20.0f, 150.0f, 0.1f); // Position in screen coords
    glScalef(50.0f, 50.0f, 1.0f);       // Scale to visible size
    drawPokeball();                    // Modified 2D version
    glPopMatrix();

    //Star
    glPushMatrix();
    glTranslatef(775.0f, 150.0f, 0.1f);
    glScalef(50.0f, 50.0f, 1.0f);
    drawHPStar();
    glPopMatrix();

    //"SNORLAX" text
    glPushMatrix();
    glTranslatef(10.0f, 170.0f, 0.0f); // Centered position
    glScalef(9.0f, 15.0f, 1.0f);         // Text size
    glColor3f(0.0f, 0.0f, 0.0f);        // Black text
    glLineWidth(5.0f);
    float spacing = 1.28f;
    drawLetter(0.0f, 0.0f, 1.0f, 'S');
    drawLetter(1.0f * spacing, 0.0f, 1.0f, 'N');
    drawLetter(2.0f * spacing, 0.0f, 1.0f, 'O');
    drawLetter(3.0f * spacing, 0.0f, 1.0f, 'R');
    drawLetter(4.0f * spacing, 0.0f, 1.0f, 'L');
    drawLetter(5.0f * spacing, 0.0f, 1.0f, 'A');
    drawLetter(6.0f * spacing, 0.0f, 1.0f, 'X');
    glPopMatrix();

    //Snorlax "ATTACK" text
    glPushMatrix();
    glTranslatef(10.0f, 113.0f, 0.0f); // Position adjustment
    glScalef(6.0f, 12.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(4.0f);
    drawLetter(0.0f, 0.0f, 1.0f, 'A');
    drawLetter(1.0f * spacing, 0.0f, 1.0f, 'T');
    drawLetter(2.0f * spacing, 0.0f, 1.0f, 'T');
    drawLetter(3.0f * spacing, 0.0f, 1.0f, 'A');
    drawLetter(4.0f * spacing, 0.0f, 1.0f, 'C');
    drawLetter(5.0f * spacing, 0.0f, 1.0f, 'K');
    glPopMatrix();

    //Kirby "ATTACK" text
    glPushMatrix();
    glTranslatef(744.0f, 113.0f, 0.0f); // Position adjustment
    glScalef(6.0f, 12.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(4.0f);
    drawLetter(0.0f, 0.0f, 1.0f, 'A');
    drawLetter(1.0f * spacing, 0.0f, 1.0f, 'T');
    drawLetter(2.0f * spacing, 0.0f, 1.0f, 'T');
    drawLetter(3.0f * spacing, 0.0f, 1.0f, 'A');
    drawLetter(4.0f * spacing, 0.0f, 1.0f, 'C');
    drawLetter(5.0f * spacing, 0.0f, 1.0f, 'K');
    glPopMatrix();

    //Snorlax "DEFENSE" text
    glPushMatrix();
    glTranslatef(10.0f, 73.0f, 0.0f);
    glScalef(6.0f, 12.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(4.0f);
    drawLetter(0.0f, 0.0f, 1.0f, 'D');
    drawLetter(1.0f * spacing, 0.0f, 1.0f, 'E');
    drawLetter(2.0f * spacing, 0.0f, 1.0f, 'F');
    drawLetter(3.0f * spacing, 0.0f, 1.0f, 'E');
    drawLetter(4.0f * spacing, 0.0f, 1.0f, 'N');
    drawLetter(5.0f * spacing, 0.0f, 1.0f, 'S');
    drawLetter(6.0f * spacing, 0.0f, 1.0f, 'E');
    glPopMatrix();

    //Kirby "DEFENSE" text
    glPushMatrix();
    glTranslatef(737.0f, 73.0f, 0.0f);
    glScalef(6.0f, 12.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(4.0f);
    drawLetter(0.0f, 0.0f, 1.0f, 'D');
    drawLetter(1.0f * spacing, 0.0f, 1.0f, 'E');
    drawLetter(2.0f * spacing, 0.0f, 1.0f, 'F');
    drawLetter(3.0f * spacing, 0.0f, 1.0f, 'E');
    drawLetter(4.0f * spacing, 0.0f, 1.0f, 'N');
    drawLetter(5.0f * spacing, 0.0f, 1.0f, 'S');
    drawLetter(6.0f * spacing, 0.0f, 1.0f, 'E');
    glPopMatrix();

    //Snorlax "SPEED" text
    glPushMatrix();
    glTranslatef(10.0f, 33.0f, 0.0f);
    glScalef(6.0f, 12.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(4.0f);
    drawLetter(0.0f, 0.0f, 1.0f, 'S');
    drawLetter(1.0f * spacing, 0.0f, 1.0f, 'P');
    drawLetter(2.0f * spacing, 0.0f, 1.0f, 'E');
    drawLetter(3.0f * spacing, 0.0f, 1.0f, 'E');
    drawLetter(4.0f * spacing, 0.0f, 1.0f, 'D');
    glPopMatrix();

    //Kirby "SPEED" text
    glPushMatrix();
    glTranslatef(751.0f, 33.0f, 0.0f);
    glScalef(6.0f, 12.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(4.0f);
    drawLetter(0.0f, 0.0f, 1.0f, 'S');
    drawLetter(1.0f * spacing, 0.0f, 1.0f, 'P');
    drawLetter(2.0f * spacing, 0.0f, 1.0f, 'E');
    drawLetter(3.0f * spacing, 0.0f, 1.0f, 'E');
    drawLetter(4.0f * spacing, 0.0f, 1.0f, 'D');
    glPopMatrix();

    //"KIRBY" text
    glPushMatrix();
    glTranslatef(732.0f, 170.0f, 0.0f);
    glScalef(9.0f, 15.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(5.0f);
    drawLetter(0.0f, 0.0f, 1.0f, 'K');
    drawLetter(1.0f * spacing, 0.0f, 1.0f, 'I');
    drawLetter(2.0f * spacing, 0.0f, 1.0f, 'R');
    drawLetter(3.0f * spacing, 0.0f, 1.0f, 'B');
    drawLetter(4.0f * spacing, 0.0f, 1.0f, 'Y');
    glPopMatrix();

    // ===== Restore 3D Projection =====
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void MyVirtualWorld::drawPlatformBase() {
    glRotatef(-90, 1, 0, 0);
    glScalef(3.0f, 3.0f, 3.0f);

    // Enhanced base with better shading
    float layerStep = 0.02f;
    for (float h = 0; h < platform_height; h += layerStep) {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= platform_segments; ++i) {
            float angle = 2 * M_PI * i / platform_segments;

            // Enhanced woody brown color with depth variation
            float depthFactor = 1.0f - (h / platform_height) * 0.3f;
            glColor3f(0.45f * depthFactor, 0.3f * depthFactor, 0.15f * depthFactor);

            float r_top = platform_radius;
            float r_bot = platform_radius;

            // Add normal for better lighting
            float nx = cos(angle);
            float ny = sin(angle);
            glNormal3f(nx, ny, 0.0f);

            glVertex3f(r_top * cos(angle), r_top * sin(angle), h + layerStep);
            glVertex3f(r_bot * cos(angle), r_bot * sin(angle), h);
        }
        glEnd();
    }

    // Draw bottom circle with enhanced color
    glPushMatrix();
    glColor3f(0.4f, 0.25f, 0.12f); // Darker woody brown for bottom
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 0.0f, -1.0f); // Downward normal
    for (int i = 0; i < platform_segments; ++i) {
        float angle = 2 * M_PI * i / platform_segments;
        float r = platform_radius;
        glVertex3f(r * cos(angle), r * sin(angle), 0);
    }
    glEnd();
    glPopMatrix();
}

// Include all the remaining drawing functions for letters and UI elements
void MyVirtualWorld::drawLetter(float x, float y, float scale, char letter) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);

    switch (letter) {
        case 'S': drawLetterS(); break;
        case 'N': drawLetterN(); break;
        case 'O': drawLetterO(); break;
        case 'R': drawLetterR(); break;
        case 'L': drawLetterL(); break;
        case 'A': drawLetterA(); break;
        case 'X': drawLetterX(); break;
        case 'T': drawLetterT(); break;
        case 'C': drawLetterC(); break;
        case 'K': drawLetterK(); break;
        case 'D': drawLetterD(); break;
        case 'E': drawLetterE(); break;
        case 'F': drawLetterF(); break;
        case 'P': drawLetterP(); break;
        case 'I': drawLetterI(); break;
        case 'B': drawLetterB(); break;
        case 'Y': drawLetterY(); break;
        case 'G': drawLetterG(); break;
        case 'M': drawLetterM(); break;
        case 'V': drawLetterV(); break;
        case 'W': drawLetterW(); break;
    }

    glPopMatrix();
}

// Letter drawing functions - keeping existing implementations
void MyVirtualWorld::drawLetterS() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8f, 1.0f);
    glVertex2f(0.6f, 1.0f);
    glVertex2f(0.4f, 1.0f);
    glVertex2f(0.2f, 1.0f);
    glVertex2f(0.1f, 0.95f);
    glVertex2f(0.05f, 0.85f);
    glVertex2f(0.1f, 0.75f);
    glVertex2f(0.2f, 0.65f);
    glVertex2f(0.4f, 0.55f);
    glVertex2f(0.9f, 0.25f);
    glVertex2f(0.85f, 0.15f);
    glVertex2f(0.7f, 0.05f);
    glVertex2f(0.5f, 0.0f);
    glVertex2f(0.3f, 0.0f);
    glVertex2f(0.1f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterN() {
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f); glVertex2f(0.0f, 1.0f);
    glVertex2f(0.0f, 1.0f); glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterO() {
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.8f, 0.0f);
    glVertex2f(1.0f, 0.5f);
    glVertex2f(0.8f, 1.0f);
    glVertex2f(0.2f, 1.0f);
    glVertex2f(0.0f, 0.5f);
    glEnd();
}

void MyVirtualWorld::drawLetterR() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.6f, 1.0f);
    glVertex2f(0.9f, 0.9f);
    glVertex2f(1.0f, 0.7f);
    glVertex2f(0.9f, 0.5f);
    glVertex2f(0.6f, 0.4f);
    glVertex2f(0.0f, 0.4f);
    glVertex2f(0.6f, 0.4f);
    glVertex2f(1.0f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterL() {
    glBegin(GL_LINES);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterA() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.5f, 1.0f);
    glVertex2f(1.0f, 0.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.25f, 0.5f);
    glVertex2f(0.75f, 0.5f);
    glEnd();
}

void MyVirtualWorld::drawLetterX() {
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f); glVertex2f(1.0f, 1.0f);
    glVertex2f(0.0f, 1.0f); glVertex2f(1.0f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterT() {
    glBegin(GL_LINES);
    glVertex2f(0.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glVertex2f(0.5f, 1.0f); glVertex2f(0.5f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterC() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(0.2f, 1.0f);
    glVertex2f(0.0f, 0.8f);
    glVertex2f(0.0f, 0.2f);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterK() {
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f); glVertex2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.5f); glVertex2f(1.0f, 1.0f);
    glVertex2f(0.0f, 0.5f); glVertex2f(1.0f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterD() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.6f, 1.0f);
    glVertex2f(1.0f, 0.8f);
    glVertex2f(1.0f, 0.2f);
    glVertex2f(0.6f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, 1.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterE() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.7f, 0.5f);
    glEnd();
}

void MyVirtualWorld::drawLetterF() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.7f, 0.5f);
    glEnd();
}

void MyVirtualWorld::drawLetterP() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, -0.2f);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.7f, 1.0f);
    glVertex2f(1.0f, 0.7f);
    glVertex2f(0.7f, 0.4f);
    glVertex2f(0.0f, 0.4f);
    glVertex2f(0.0f, -0.2f);
    glEnd();
}

void MyVirtualWorld::drawLetterI() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.5f, 1.0f);
    glVertex2f(0.5f, 0.0f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterB() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, 1.0f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.5f, 1.0f);
    glVertex2f(0.7f, 0.95f);
    glVertex2f(0.8f, 0.85f);
    glVertex2f(0.8f, 0.65f);
    glVertex2f(0.7f, 0.55f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.0f, 0.5f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.55f, 0.5f);
    glVertex2f(0.8f, 0.45f);
    glVertex2f(0.9f, 0.35f);
    glVertex2f(0.9f, 0.15f);
    glVertex2f(0.8f, 0.05f);
    glVertex2f(0.6f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterY(){
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.5f, 0.5f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(0.5f, 0.5f);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.5f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterG() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(0.2f, 1.0f);
    glVertex2f(0.0f, 0.8f);
    glVertex2f(0.0f, 0.2f);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.8f, 0.0f);
    glVertex2f(1.0f, 0.2f);
    glVertex2f(1.0f, 0.5f);
    glVertex2f(0.6f, 0.5f);
    glEnd();
}

void MyVirtualWorld::drawLetterM() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f, 0.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterV() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.5f, 0.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();
}

void MyVirtualWorld::drawLetterW() {
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(0.25f, 0.0f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.75f, 0.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();
}

// HP Bar and UI drawing functions
void MyVirtualWorld::drawOuterHPBar(float centerX, float centerY, float length, float height){
    float radius = height / 2.0f;
    float halfLength = (length - height) / 2.0f;
    // Draw central rectangle
    glBegin(GL_QUADS);
    glVertex2f(centerX - halfLength, centerY - radius);
    glVertex2f(centerX + halfLength, centerY - radius);
    glVertex2f(centerX + halfLength, centerY + radius);
    glVertex2f(centerX - halfLength, centerY + radius);
    glEnd();

    // Draw left semicircle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX - halfLength, centerY);
    for (int i = 90; i <= 270; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(centerX - halfLength + radius * cos(angle),
                   centerY + radius * sin(angle));
    }
    glEnd();

    // Draw right semicircle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX + halfLength, centerY);
    for (int i = -90; i <= 90; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(centerX + halfLength + radius * cos(angle),
                   centerY + radius * sin(angle));
    }
    glEnd();
}

void MyVirtualWorld::drawHPBar(float centerX, float centerY, float length, float height, float currentHP, float maxHP) {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    float radius = height / 2.0f;
    float barWidth = length - height;
    float barLeft = centerX - barWidth / 2.0f;
    float barRight = centerX + barWidth / 2.0f;
    float barBottom = centerY - radius;
    float barTop = centerY + radius;

    currentHP = std::max(0.0f, std::min(currentHP, maxHP));
    maxHP = std::max(1.0f, maxHP);
    float hpRatio = currentHP / maxHP;

    // Draw background (empty) part
    glColor3f(0.3f, 0.3f, 0.3f);
    drawRoundedRect(centerX, centerY, length, height, radius);

    if (hpRatio > 0) {
        // Color based on HP percentage
        if (hpRatio > 0.6f) {
            glColor3f(0.0f, 1.0f, 0.0f); // Green
        } else if (hpRatio > 0.3f) {
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        } else {
            glColor3f(1.0f, 0.0f, 0.0f); // Red
        }

        // Calculate filled rectangle width
        float fillLength = barWidth * hpRatio;
        float fillRight = barLeft + fillLength;

        // Draw filled rectangle part
        glBegin(GL_QUADS);
        glVertex2f(barLeft, barBottom);
        glVertex2f(fillRight, barBottom);
        glVertex2f(fillRight, barTop);
        glVertex2f(barLeft, barTop);
        glEnd();

        // Draw left semicircle
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(barLeft, centerY);
        for (int i = 90; i <= 270; i += 10) {
            float angle = i * M_PI / 180.0f;
            glVertex2f(barLeft + radius * cos(angle), centerY + radius * sin(angle));
        }
        glEnd();

        // Draw right semicircle only if fill reaches into right cap area
        if (fillRight >= barRight - 1e-5) {
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(barRight, centerY);
            for (int i = -90; i <= 90; i += 10) {
                float angle = i * M_PI / 180.0f;
                glVertex2f(barRight + radius * cos(angle), centerY + radius * sin(angle));
            }
            glEnd();
        }
    }
    glPopAttrib();
}

void MyVirtualWorld::drawRoundedRect(float x, float y, float w, float h, float r) {
    float halfW = (w - h)/2.0f;

    // Center rectangle
    glBegin(GL_QUADS);
    glVertex2f(x - halfW, y - r);
    glVertex2f(x + halfW, y - r);
    glVertex2f(x + halfW, y + r);
    glVertex2f(x - halfW, y + r);
    glEnd();

    // Left semicircle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x - halfW, y);
    for(int i = 90; i <= 270; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(x - halfW + r * cos(angle), y + r * sin(angle));
    }
    glEnd();

    // Right semicircle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + halfW, y);
    for(int i = -90; i <= 90; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(x + halfW + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void MyVirtualWorld::drawAttackBar(float centerX, float centerY, float length, float height){
    float radius = height / 2.0f;
    float halfLength = (length - height) / 2.0f;

    // Draw central rectangle
    glBegin(GL_QUADS);
    glVertex2f(centerX - halfLength, centerY - radius);
    glVertex2f(centerX + halfLength, centerY - radius);
    glVertex2f(centerX + halfLength, centerY + radius);
    glVertex2f(centerX - halfLength, centerY + radius);
    glEnd();

    // Draw left semicircle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX - halfLength, centerY);
    for (int i = 90; i <= 270; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(centerX - halfLength + radius * cos(angle),
                   centerY + radius * sin(angle));
    }
    glEnd();

    // Draw right semicircle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX + halfLength, centerY);
    for (int i = -90; i <= 90; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(centerX + halfLength + radius * cos(angle),
                   centerY + radius * sin(angle));
    }
    glEnd();
}

void MyVirtualWorld::drawHPStar() {
    glDisable(GL_CULL_FACE);

    const int num_points = 5;
    const float R = 0.4f;
    const float r = 0.2f;
    const float cx = 0.0f, cy = 0.0f;

    glColor3f(1.0f, 0.85f, 0.0f);  // Yellow
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);  // center

        for (int i = 0; i <= 2 * num_points; ++i) {
            float angle = i * M_PI / num_points;
            float radius = (i % 2 == 0) ? R : r;
            float x = cx + radius * sin(angle);
            float y = cy + radius * cos(angle);
            glVertex2f(x, y);
        }
    glEnd();
}

void MyVirtualWorld::drawKirbyHPBar(float centerX, float centerY, float length, float height, float kirbycurrentHP, float kirbymaxHP) {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    float radius = height / 2.0f;
    float barWidth = length - height;
    float barLeft = centerX - barWidth / 2.0f;
    float barRight = centerX + barWidth / 2.0f;
    float barBottom = centerY - radius;
    float barTop = centerY + radius;

    kirbycurrentHP = std::max(0.0f, std::min(kirbycurrentHP, kirbymaxHP));
    kirbymaxHP = std::max(1.0f, kirbymaxHP);
    float hpRatio = kirbycurrentHP / kirbymaxHP;

    // Draw background (empty) part
    glColor3f(0.3f, 0.3f, 0.3f);
    drawKirbyRoundedRect(centerX, centerY, length, height, radius);

    if (hpRatio > 0) {
        // Color based on HP percentage
        if (hpRatio > 0.6f) {
            glColor3f(0.0f, 1.0f, 0.0f); // Green
        } else if (hpRatio > 0.3f) {
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        } else {
            glColor3f(1.0f, 0.0f, 0.0f); // Red
        }

        // Calculate filled rectangle width
        float fillLength = barWidth * hpRatio;
        float fillRight = barLeft + fillLength;

        // Draw filled rectangle part
        glBegin(GL_QUADS);
        glVertex2f(barLeft, barBottom);
        glVertex2f(fillRight, barBottom);
        glVertex2f(fillRight, barTop);
        glVertex2f(barLeft, barTop);
        glEnd();

        // Draw left semicircle
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(barLeft, centerY);
        for (int i = 90; i <= 270; i += 10) {
            float angle = i * M_PI / 180.0f;
            glVertex2f(barLeft + radius * cos(angle), centerY + radius * sin(angle));
        }
        glEnd();

        // Draw right semicircle only if fill reaches into right cap area
        if (fillRight >= barRight - 1e-5) {
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(barRight, centerY);
            for (int i = -90; i <= 90; i += 10) {
                float angle = i * M_PI / 180.0f;
                glVertex2f(barRight + radius * cos(angle), centerY + radius * sin(angle));
            }
            glEnd();
        }
    }
    glPopAttrib();
}

void MyVirtualWorld::drawKirbyRoundedRect(float x, float y, float w, float h, float r) {
    float halfW = (w - h)/2.0f;

    // Center rectangle
    glBegin(GL_QUADS);
    glVertex2f(x - halfW, y - r);
    glVertex2f(x + halfW, y - r);
    glVertex2f(x + halfW, y + r);
    glVertex2f(x - halfW, y + r);
    glEnd();

    // Left semicircle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x - halfW, y);
    for(int i = 90; i <= 270; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(x - halfW + r * cos(angle), y + r * sin(angle));
    }
    glEnd();

    // Right semicircle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + halfW, y);
    for(int i = -90; i <= 90; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(x + halfW + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

// namespace BattleGround


void MyVirtualWorld::drawSegmentedPlatformTop() {
    glPushMatrix();
    glTranslatef(0, 0, platform_height + 0.004f);

    // Define the segments - we'll create a pattern similar to the image
    int num_rings = 3;
    int segments_per_ring[] = {1, 8, 16}; // Center, inner ring, outer ring
    float ring_radii[] = {0.8f, 2.8f, platform_radius}; // Radii for each ring

    // Enhanced golden-brown colors for wood texture
    float segment_colors[][3] = {
        {0.8f, 0.6f, 0.3f},   // Golden wood
        {0.75f, 0.55f, 0.25f}, // Medium golden brown
        {0.7f, 0.5f, 0.2f},    // Darker golden brown
        {0.85f, 0.65f, 0.35f}, // Light golden
        {0.72f, 0.52f, 0.22f}, // Wood variation
        {0.78f, 0.58f, 0.28f}  // Lighter wood variation
    };

    // Draw center circle with normal
    glColor3f(segment_colors[0][0], segment_colors[0][1], segment_colors[0][2]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0f, 0.0f, 1.0f); // Upward normal
    for (int i = 0; i < 24; ++i) {
        float angle = 2 * M_PI * i / 24;
        glVertex3f(ring_radii[0] * cos(angle), ring_radii[0] * sin(angle), 0);
    }
    glEnd();

    // Draw gaps between center and inner ring
    glColor3f(0.3f, 0.2f, 0.1f); // Dark wood gap color
    drawRingGap(ring_radii[0], ring_radii[1] - 0.8f, 0.05f);

    // Draw inner ring segments (8 segments)
    for (int seg = 0; seg < segments_per_ring[1]; ++seg) {
        float start_angle = (2 * M_PI * seg) / segments_per_ring[1];
        float end_angle = (2 * M_PI * (seg + 1)) / segments_per_ring[1];

        // Alternate colors for segments
        int color_index = (seg + 1) % 6;
        glColor3f(segment_colors[color_index][0], segment_colors[color_index][1], segment_colors[color_index][2]);

        drawPlatformSegment(ring_radii[1] - 0.7f, ring_radii[1], start_angle, end_angle);
    }

    // Draw gaps between inner and outer ring
    glColor3f(0.9f, 0.8f, 0.4f); // Golden gap
    drawRingGap(ring_radii[1], ring_radii[2] - 0.7f, 0.05f);

    // Draw outer ring segments (16 segments)
    for (int seg = 0; seg < segments_per_ring[2]; ++seg) {
        float start_angle = (2 * M_PI * seg) / segments_per_ring[2];
        float end_angle = (2 * M_PI * (seg + 1)) / segments_per_ring[2];

        // Alternate colors for segments
        int color_index = (seg + 2) % 6;
        glColor3f(segment_colors[color_index][0], segment_colors[color_index][1], segment_colors[color_index][2]);

        drawPlatformSegment(ring_radii[2] - 0.6f, ring_radii[2], start_angle, end_angle);
    }

    glPopMatrix();
}

// Helper function to draw individual platform segments with enhanced normals
void MyVirtualWorld::drawPlatformSegment(float inner_radius, float outer_radius, float start_angle, float end_angle) {
    int detail = 20; // Number of subdivisions for smooth curves

    glBegin(GL_POLYGON);

    // Draw the segment as a polygon with proper normals
    glNormal3f(0.0f, 0.0f, 1.0f); // Upward normal for top surface

    // Inner arc
    for (int i = 0; i <= detail; ++i) {
        float t = (float)i / detail;
        float angle = start_angle + t * (end_angle - start_angle);
        glVertex3f(inner_radius * cos(angle), inner_radius * sin(angle), 0);
    }

    // Outer arc (reversed)
    for (int i = detail; i >= 0; --i) {
        float t = (float)i / detail;
        float angle = start_angle + t * (end_angle - start_angle);
        glVertex3f(outer_radius * cos(angle), outer_radius * sin(angle), 0);
    }

    glEnd();

    // Add enhanced depth variation with proper normals
    glBegin(GL_QUADS);
    for (int i = 0; i < detail; ++i) {
        float t1 = (float)i / detail;
        float t2 = (float)(i + 1) / detail;
        float angle1 = start_angle + t1 * (end_angle - start_angle);
        float angle2 = start_angle + t2 * (end_angle - start_angle);

        // Calculate normal for side face
        float nx = cos((angle1 + angle2) / 2.0f);
        float ny = sin((angle1 + angle2) / 2.0f);
        glNormal3f(nx, ny, 0.0f);

        // Outer edge with enhanced depth
        glVertex3f(outer_radius * cos(angle1), outer_radius * sin(angle1), 0);
        glVertex3f(outer_radius * cos(angle2), outer_radius * sin(angle2), 0);
        glVertex3f(outer_radius * cos(angle2), outer_radius * sin(angle2), 0.02f);
        glVertex3f(outer_radius * cos(angle1), outer_radius * sin(angle1), 0.02f);
    }
    glEnd();
}

// Helper function to draw gaps between rings
void MyVirtualWorld::drawRingGap(float inner_radius, float outer_radius, float depth) {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= platform_segments; ++i) {
        float angle = 2 * M_PI * i / platform_segments;
        // Add normals for better lighting
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);

        glVertex3f(inner_radius * cos(angle), inner_radius * sin(angle), -depth);
        glVertex3f(outer_radius * cos(angle), outer_radius * sin(angle), -depth);
    }
    glEnd();
}

// Update your drawBattlePlatform() function to use the new segmented top:
void MyVirtualWorld::drawBattlePlatform() {
    glScalef(1.35f, 1.35f, 1.3f);
    drawPlatformBase();
    drawSegmentedPlatformTop(); // Changed from drawPlatformTop()
    drawStar();

    // Temporarily reduce lighting intensity for grass to prevent overexposure
    GLfloat current_diffuse[4];
    glGetLightfv(GL_LIGHT0, GL_DIFFUSE, current_diffuse);
    GLfloat current_ambient[4];
    glGetLightfv(GL_LIGHT0, GL_AMBIENT, current_ambient);

    // Reduce lighting for grass rendering - less reduction than before
    GLfloat reduced_diffuse[] = { current_diffuse[0] * 0.85f, current_diffuse[1] * 0.85f, current_diffuse[2] * 0.85f, current_diffuse[3] };
    GLfloat reduced_ambient[] = { current_ambient[0] * 0.9f, current_ambient[1] * 0.9f, current_ambient[2] * 0.9f, current_ambient[3] };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, reduced_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, reduced_ambient);

    // Draw grass with reduced lighting
    drawMultiLayerGrassRing(platform_radius * 1.000f, 0.39f, platform_height + 0.004f);

    // Restore original lighting
    glLightfv(GL_LIGHT0, GL_DIFFUSE, current_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, current_ambient);
}

void MyVirtualWorld::drawStar() {
    glPushMatrix();
    glTranslatef(0, 0, platform_height + 0.031f);
    glColor3f(0.46f, 0.86f, 0.95f);

    // Add normals for better lighting
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0, 0, 0);
    for (int i = 0; i <= 2 * star_points; ++i) {
        float angle = i * M_PI / star_points;
        float r = (i % 2 == 0) ? star_outer_radius : star_inner_radius;
        glVertex3f(r * cos(angle), r * sin(angle), 0);
    }
    glEnd();

    glColor3f(0.81f, 0.89f, 0.95f);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0, 0, 0.008f);
    for (int i = 0; i <= 24; ++i) {
        float angle = 2 * M_PI * i / 24;
        glVertex3f(0.62f * star_inner_radius * cos(angle), 0.62f * star_inner_radius * sin(angle), 0.008f);
    }
    glEnd();
    glPopMatrix();
}

void MyVirtualWorld::drawGrassLayerWide(float R1, float R2, float height, float z, int segs, float angle_offset, float colR, float colG, float colB) {
    // Moderate grass brightness - not too bright, not too dark
    float grassDimming = 0.8f; // Less dimming for better visibility
    glColor3f(colR * grassDimming, colG * grassDimming, colB * grassDimming);

    for (int i = 0; i < segs; ++i) {
        float angle = 2 * M_PI * i / segs + angle_offset;
        float next_angle = 2 * M_PI * (i + 1) / segs + angle_offset;
        float spike_h = height * (0.92f + 0.23f * ((i % 3 == 0) ? 1 : 0) + 0.12f * sin(angle * 5.4f));

        glBegin(GL_TRIANGLES);
        // Add proper normals for grass
        float avg_angle = (angle + next_angle) / 2.0f;
        float nx = cos(avg_angle);
        float ny = sin(avg_angle);
        glNormal3f(nx, ny, 0.5f);

        glVertex3f(R1 * cos(angle), R1 * sin(angle), z);
        glVertex3f(R1 * cos(next_angle), R1 * sin(next_angle), z);
        glVertex3f(R2 * cos(avg_angle), R2 * sin(avg_angle), z + spike_h);
        glEnd();
    }
}

void MyVirtualWorld::drawMultiLayerGrassRing(float base_r, float h, float base_z) {
    int segs = 700;
    // Balanced grass colors - bright enough to see clearly, dark enough to avoid overexposure
    drawGrassLayerWide(base_r * 1.040f, base_r * 1.027f, h * 1.40f, base_z + 0.015f, segs, 0.0f, 0.25f, 0.65f, 0.25f);  // Layer 1 - medium green
    drawGrassLayerWide(base_r * 1.025f, base_r * 1.017f, h * 1.22f, base_z + 0.012f, segs, M_PI / (segs * 1.15f), 0.30f, 0.75f, 0.35f);  // Layer 2 - brighter
    drawGrassLayerWide(base_r * 1.015f, base_r * 1.009f, h * 1.10f, base_z + 0.010f, segs, M_PI / (segs * 1.08f), 0.35f, 0.80f, 0.40f);  // Layer 3 - bright green
    drawGrassLayerWide(base_r * 1.008f, base_r * 1.003f, h * 0.99f, base_z + 0.005f, segs, M_PI / (segs * 0.96f), 0.32f, 0.75f, 0.35f);  // Layer 4 - medium-bright
    drawGrassLayerWide(base_r * 1.000f, base_r * 0.995f, h * 0.90f, base_z - 0.002f, segs, M_PI / (segs * 0.82f), 0.28f, 0.70f, 0.30f);  // Layer 5 - medium
    drawGrassLayerWide(base_r * 0.990f, base_r * 0.985f, h * 0.75f, base_z - 0.010f, segs, M_PI / (segs / 2), 0.22f, 0.60f, 0.25f);  // Layer 6 - darker medium
    drawGrassLayerWide(base_r * 0.978f, base_r * 0.973f, h * 0.60f, base_z - 0.020f, segs, M_PI / (segs / 3.4f), 0.18f, 0.50f, 0.20f);  // Layer 7 - darker base
}

void MyVirtualWorld::drawFloatingStone(float x, float y, float z, float scale, float angle) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0.15f, 0.9f, 0.4f);
    glScalef(scale, scale, scale);

    // Enhanced stone colors with better shading
    int stoneIndex = (int)(x + y + z) % 8;
    float timeColor = sin(elapsedTime * 0.3f + angle * 0.01f) * 0.1f + 0.9f; // Reduced variation for subtlety

    switch(stoneIndex % 8) {
        case 0: // Enhanced Red Stone
            glColor3f(0.7f * timeColor, 0.4f, 0.4f);
            break;
        case 1: // Enhanced Slate Blue
            glColor3f(0.45f, 0.5f, 0.7f * timeColor);
            break;
        case 2: // Enhanced Purple
            glColor3f(0.6f * timeColor, 0.45f, 0.65f);
            break;
        case 3: // Enhanced Sandstone
            glColor3f(0.8f, 0.6f * timeColor, 0.4f);
            break;
        case 4: // Enhanced Green
            glColor3f(0.4f, 0.7f * timeColor, 0.45f);
            break;
        case 5: // Enhanced Rose
            glColor3f(0.7f, 0.55f * timeColor, 0.6f);
            break;
        case 6: // Enhanced Gold
            glColor3f(0.8f, 0.7f * timeColor, 0.5f);
            break;
        case 7: // Enhanced Teal
            glColor3f(0.45f, 0.65f * timeColor, 0.7f);
            break;
    }

    glutSolidOctahedron();
    glPopMatrix();
}

void MyVirtualWorld::drawCloud3D(float x, float y, float z, float base_r) {
    struct Puff {
        float dx, dy, dz, r, alpha, pulse;
    };
    Puff puffs[] = {
        {  0.0f,  0.0f,  0.0f, 1.0f, 0.43f, 0.06f },
        { -0.75f, 0.19f, 0.05f, 0.73f, 0.31f, 0.11f },
        {  0.77f,-0.10f,-0.08f, 0.66f, 0.27f, 0.13f },
        { -0.38f, 0.40f,-0.21f, 0.54f, 0.21f, 0.12f },
        {  0.36f, 0.43f, 0.17f, 0.55f, 0.21f, 0.10f },
        {  0.00f,-0.55f, 0.00f, 0.46f, 0.17f, 0.09f },
        { -0.60f,-0.33f, 0.14f, 0.38f, 0.13f, 0.11f },
        {  0.64f,-0.36f,-0.15f, 0.36f, 0.13f, 0.08f },
        {  0.00f, 0.00f, 0.45f, 0.32f, 0.09f, 0.07f }
    };

    float t = elapsedTime;

    glPushMatrix();
    glTranslatef(x, y, z);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLboolean lightingWasEnabled;
    glGetBooleanv(GL_LIGHTING, &lightingWasEnabled);
    glDisable(GL_LIGHTING);

    for (int i = 0; i < 9; ++i) {
        float pulse = 1.0f + 0.06f * sin(t * 0.8f + puffs[i].pulse + i) * puffs[i].pulse;
        glPushMatrix();
        glTranslatef(puffs[i].dx * base_r, puffs[i].dy * base_r, puffs[i].dz * base_r);
        glColor4f(1.0f, 1.0f, 1.0f, puffs[i].alpha);
        glutSolidSphere(base_r * puffs[i].r * pulse, 30, 24);
        glPopMatrix();
    }
    glDisable(GL_BLEND);

    // Restore lighting state
    if (lightingWasEnabled) {
        glEnable(GL_LIGHTING);
    }

    glPopMatrix();
}

//Snorlax Base stats
void MyVirtualWorld::drawPokeball() {
    glPushMatrix();

    // Draw the red half
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i <= 180; i++) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(0.3f * cos(angle), 0.3f * sin(angle));
    }
    glEnd();

    // Draw the white half
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 180; i <= 360; i++) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(0.3f * cos(angle), 0.3f * sin(angle));
    }
    glEnd();

    glDisable(GL_DEPTH_TEST);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-0.3f, 0.0f);
    glVertex2f(0.3f, 0.0f);
    glEnd();
    glEnable(GL_DEPTH_TEST);

    // --- Black Center Button ---
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 360; ++i) {
        float angle = i * M_PI / 180.0f;
        glVertex3f(0.1f * cos(angle), 0.1f * sin(angle), 0.01f);
    }
    glEnd();

    // --- White Inner Dot ---
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 360; ++i) {
        float angle = i * M_PI / 180.0f;
        glVertex3f(0.06f * cos(angle), 0.06f * sin(angle), 0.02f);
    }
    glEnd();

    glPopMatrix();
}
