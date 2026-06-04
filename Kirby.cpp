#include "Kirby.hpp"
#include <iostream>
#include <ctime>
#include <cmath>

using namespace std;

namespace Kirby
{
    MyVirtualWorld::MyVirtualWorld()
    {
        isFloating = false;
        floatHeight = 0.0f;
        floatSpeed = 1.0f;
        floatUpward = true;

        rotX = 0.0f;
        rotY = 0.0f;
        rotZ = 0.0f;

        rotSpeedX = 0.2f;
        rotSpeedY = 0.6f;
        rotSpeedZ = 0.0f;

        bodyRadius = 5.0;
        slices = 30;
        stacks = 30;

        isAnimating = true;
        isWireframe = false;

        burstActive = false;
        burstRadius = 0.0f;
    }

    void MyVirtualWorld::init()
    {
        srand((unsigned int)time(NULL));
        cout << "Kirby Character Initialized" << endl;

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_NORMALIZE);
        glShadeModel(GL_SMOOTH);

        // IMPROVED LIGHTING SETUP - Balanced with nice reflections for Kirby

        // Main light (key light) - Enhanced for better Kirby reflections
        GLfloat light0_pos[] = { 8.0f, 12.0f, 8.0f, 1.0f };      // Good position for Kirby
        GLfloat light0_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat light0_diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };    // Bright diffuse for good lighting
        GLfloat light0_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };   // Good specular for reflections

        glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

        // Fill light - Reduces harsh shadows on Kirby's round body
        GLfloat light1_pos[] = { -8.0f, 6.0f, 4.0f, 1.0f };
        GLfloat light1_ambient[] = { 0.15f, 0.15f, 0.15f, 1.0f };
        GLfloat light1_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };    // Good fill light
        GLfloat light1_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };

        glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
        glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

        // Back rim light - Helps separate Kirby from background
        GLfloat light2_pos[] = { 0.0f, 8.0f, -8.0f, 1.0f };
        GLfloat light2_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        GLfloat light2_diffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat light2_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };   // Nice rim lighting

        glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
        glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);

        // Global ambient - Balanced for Kirby
        GLfloat global_ambient[] = { 0.15f, 0.15f, 0.15f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        // IMPROVED MATERIAL PROPERTIES - Perfect for Kirby's shiny but not too shiny look
        GLfloat mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };       // Good ambient response
        GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };       // Good diffuse response
        GLfloat mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };      // Moderate specular - not too shiny
        GLfloat mat_shininess[] = { 45.0f };                       // Perfect shininess for Kirby

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

        glEnable(GL_DEPTH_TEST);
    }

    void drawKirbyEye(float x, float y, float z, float radius, GLUquadric* quad)
    {
        float forwardZ = z + 0.5f;

        glColor3f(0.0f, 0.0f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, forwardZ);
        glScalef(0.8f, 2.0f, 0.6f);
        gluSphere(quad, radius, 20, 20);
        glPopMatrix();

        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
        glTranslatef(x, y + radius * 0.4f, forwardZ + 0.01f);
        glScalef(0.6f, 1.5f, 0.6f);
        gluSphere(quad, radius, 20, 20);
        glPopMatrix();

        glColor3f(0.0f, 0.5f, 1.0f);
        glPushMatrix();
        glTranslatef(x, y - radius * 0.5f, forwardZ + 0.01f);
        glScalef(0.6f, 1.4f, 0.6f);
        gluSphere(quad, radius, 20, 20);
        glPopMatrix();
    }

    // Solid 3D Star implementation - Simple and robust approach
    void drawSolidStar(float x, float y, float z, float size) {
        const int NUM_POINTS = 5;
        const float OUTER_RADIUS = size;
        const float INNER_RADIUS = size * 0.4f;
        const float STAR_THICKNESS = size * 0.3f;

        glPushMatrix();
        glTranslatef(x, y, z);

        // Generate star outline points
        float starPoints[NUM_POINTS * 2][2];  // [x, y] coordinates

        for (int i = 0; i < NUM_POINTS * 2; i++) {
            float angle = (i * M_PI / NUM_POINTS) - (M_PI / 2.0f);
            float radius = (i % 2 == 0) ? OUTER_RADIUS : INNER_RADIUS;
            starPoints[i][0] = radius * cos(angle);
            starPoints[i][1] = radius * sin(angle);
        }

        // Set golden color with better material properties for reflections
        glColor3f(1.0f, 0.8f, 0.2f);

        // Enhanced material for stars
        GLfloat star_ambient[] = { 0.4f, 0.3f, 0.1f, 1.0f };
        GLfloat star_diffuse[] = { 1.0f, 0.8f, 0.2f, 1.0f };
        GLfloat star_specular[] = { 0.7f, 0.6f, 0.3f, 1.0f };    // Nice golden specular
        GLfloat star_shininess[] = { 60.0f };                     // Shiny but not too shiny

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, star_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, star_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, star_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, star_shininess);

        // Draw front face
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, STAR_THICKNESS / 2.0f);  // Center point
        for (int i = 0; i <= NUM_POINTS * 2; i++) {
            int idx = i % (NUM_POINTS * 2);
            glVertex3f(starPoints[idx][0], starPoints[idx][1], STAR_THICKNESS / 2.0f);
        }
        glEnd();

        // Draw back face
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(0.0f, 0.0f, -STAR_THICKNESS / 2.0f);  // Center point
        for (int i = NUM_POINTS * 2; i >= 0; i--) {  // Reverse order for proper winding
            int idx = i % (NUM_POINTS * 2);
            glVertex3f(starPoints[idx][0], starPoints[idx][1], -STAR_THICKNESS / 2.0f);
        }
        glEnd();

        // Draw side faces (extrusion)
        glBegin(GL_QUADS);
        for (int i = 0; i < NUM_POINTS * 2; i++) {
            int next = (i + 1) % (NUM_POINTS * 2);

            float x1 = starPoints[i][0];
            float y1 = starPoints[i][1];
            float x2 = starPoints[next][0];
            float y2 = starPoints[next][1];

            // Calculate normal for this side face
            float dx = x2 - x1;
            float dy = y2 - y1;
            float nx = -dy;  // Perpendicular to edge
            float ny = dx;
            float norm = sqrt(nx * nx + ny * ny);
            if (norm > 0.0f) {
                nx /= norm;
                ny /= norm;
            }

            glNormal3f(nx, ny, 0.0f);

            // Create quad for this side
            glVertex3f(x1, y1, -STAR_THICKNESS / 2.0f);  // Bottom left
            glVertex3f(x2, y2, -STAR_THICKNESS / 2.0f);  // Bottom right
            glVertex3f(x2, y2, STAR_THICKNESS / 2.0f);   // Top right
            glVertex3f(x1, y1, STAR_THICKNESS / 2.0f);   // Top left
        }
        glEnd();

        // Restore default material
        GLfloat default_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat default_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
        GLfloat default_shininess[] = { 45.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, default_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, default_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, default_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, default_shininess);

        glPopMatrix();
    }

    // Main star drawing function
    void drawStar(float x, float y, float z, float size) {
        drawSolidStar(x, y, z, size);
    }

    void MyVirtualWorld::draw()
    {
        if (burstActive)
        {
            glPushMatrix();
            glTranslatef(0.0f, floatHeight, 0.0f);

            GLboolean lightingWasOn;
            glGetBooleanv(GL_LIGHTING, &lightingWasOn);
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);

            glColor4f(1.0f, 0.2f, 0.2f, 0.5f);
            glLineWidth(3.0f);
            glutWireSphere(burstRadius, 20, 20);

            glLineWidth(1.0f);
            if (lightingWasOn) glEnable(GL_LIGHTING);
            glEnable(GL_DEPTH_TEST);
            glPopMatrix();

            // Draw 3D solid stars with proper lighting
            for (std::vector<Star>::iterator it = stars.begin(); it != stars.end(); ++it)
            {
                drawStar(it->x, it->y, it->z, 0.4f * it->life);
            }
        }

        // Kirby drawing code with enhanced materials
        glTranslatef(0.0f, floatHeight, 0.0f);

        glPushMatrix();
        glRotatef(rotX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotY, 0.0f, 1.0f, 0.0f);
        glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

        // Enhanced pink material for Kirby's body
        GLfloat kirby_ambient[] = { 0.4f, 0.2f, 0.3f, 1.0f };     // Pink ambient
        GLfloat kirby_diffuse[] = { 1.0f, 0.6f, 0.9f, 1.0f };     // Pink diffuse
        GLfloat kirby_specular[] = { 0.7f, 0.5f, 0.6f, 1.0f };    // Pink specular
        GLfloat kirby_shininess[] = { 50.0f };                     // Nice shine

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, kirby_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kirby_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kirby_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, kirby_shininess);

        glColor3f(1.0f, 0.6f, 0.9f);
        GLUquadricObj* body = gluNewQuadric();
        gluQuadricDrawStyle(body, isWireframe ? GLU_LINE : GLU_FILL);
        gluQuadricNormals(body, GLU_SMOOTH);

        gluSphere(body, bodyRadius, slices, stacks);

        // Enhanced red material for feet
        GLfloat red_ambient[] = { 0.3f, 0.1f, 0.1f, 1.0f };
        GLfloat red_diffuse[] = { 1.0f, 0.3f, 0.3f, 1.0f };
        GLfloat red_specular[] = { 0.6f, 0.3f, 0.3f, 1.0f };
        GLfloat red_shininess[] = { 40.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, red_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, red_shininess);

        glColor3f(1.0f, 0.3f, 0.3f);
        glPushMatrix();
        glTranslatef(-bodyRadius/1.8f, -bodyRadius*0.85f, 0.0f);
        glScalef(1.3f, 0.8f, 1.5f);
        gluSphere(body, bodyRadius/2.5f, slices, stacks);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(bodyRadius/1.8f, -bodyRadius*0.85f, 0.0f);
        glScalef(1.3f, 0.8f, 1.5f);
        gluSphere(body, bodyRadius/2.5f, slices, stacks);
        glPopMatrix();

        // Back to pink for arms
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, kirby_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kirby_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kirby_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, kirby_shininess);

        glColor3f(1.0f, 0.6f, 0.9f);
        glPushMatrix();
        glTranslatef(-bodyRadius*1.0f, 0.0f, 0.0f);
        glScalef(0.4f, 0.4f, 0.4f);
        gluSphere(body, bodyRadius, slices, stacks);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(bodyRadius*1.0f, 0.0f, 0.0f);
        glScalef(0.4f, 0.4f, 0.4f);
        gluSphere(body, bodyRadius, slices, stacks);
        glPopMatrix();

        float eyeRadius = bodyRadius / 6.0f;
        float eyeZ = bodyRadius * 0.85f;
        drawKirbyEye(-bodyRadius * 0.25f, bodyRadius * 0.25f, eyeZ, eyeRadius, body);
        drawKirbyEye(bodyRadius * 0.25f, bodyRadius * 0.25f, eyeZ, eyeRadius, body);

        // Enhanced mouth material
        GLfloat mouth_ambient[] = { 0.3f, 0.0f, 0.15f, 1.0f };
        GLfloat mouth_diffuse[] = { 1.0f, 0.0f, 0.5f, 1.0f };
        GLfloat mouth_specular[] = { 0.5f, 0.2f, 0.3f, 1.0f };
        GLfloat mouth_shininess[] = { 30.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mouth_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mouth_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mouth_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mouth_shininess);

        glColor3f(1.0f, 0.0f, 0.5f);
        glPushMatrix();
        glTranslatef(0.0f, -bodyRadius*0.2f, bodyRadius*0.8f);
        glScalef(0.4f, 0.7f, 0.8f);
        gluSphere(body, bodyRadius/3.5f, slices, stacks);
        glPopMatrix();

        // Enhanced cheek material
        GLfloat cheek_ambient[] = { 0.35f, 0.1f, 0.25f, 1.0f };
        GLfloat cheek_diffuse[] = { 1.0f, 0.3f, 0.7f, 1.0f };
        GLfloat cheek_specular[] = { 0.6f, 0.3f, 0.4f, 1.0f };
        GLfloat cheek_shininess[] = { 35.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cheek_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cheek_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cheek_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, cheek_shininess);

        glColor3f(1.0f, 0.3f, 0.7f);
        glPushMatrix();
        glTranslatef(-bodyRadius*0.5f, -0.8f, bodyRadius*0.7f);
        glScalef(1.5f, 0.8f, 2.0f);
        gluSphere(body, bodyRadius/8.0f, slices, stacks);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(bodyRadius*0.5f, -0.8f, bodyRadius*0.7f);
        glScalef(1.5f, 0.8f, 2.0f);
        gluSphere(body, bodyRadius/8.0f, slices, stacks);
        glPopMatrix();

        gluDeleteQuadric(body);
        glPopMatrix();
    }

    void MyVirtualWorld::update(float deltaTime)
    {
        if (isFloating)
        {
            floatHeight = 1.5f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.006f);
        }
        else
        {
            floatHeight = 0.0f;
        }

        if (burstActive)
        {
            burstRadius += deltaTime * 14.0f;
            if (burstRadius > 10.0f)
            {
                burstActive = false;
                burstRadius = 0.0f;
                stars.clear();
            }

            std::vector<Star>::iterator it = stars.begin();
            while (it != stars.end())
            {
                it->x += it->dx * deltaTime;
                it->y += it->dy * deltaTime;
                it->z += it->dz * deltaTime;
                it->life -= deltaTime * 0.3f;

                if (it->life <= 0.0f)
                {
                    it = stars.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
    }

    void MyVirtualWorld::triggerBurst()
    {
        burstActive = true;
        burstRadius = 0.1f;

        stars.clear();
        for (int i = 0; i < 30; ++i)
        {
            Star star;
            star.x = 0.0f;
            star.y = floatHeight;
            star.z = 0.0f;

            float angle = (float)i / 50.0f * 2.0f * 3.1415926f;
            float speed =70.0f * ((rand() % 100) / 100.0f);
            star.dx = cos(angle) * speed;
            star.dy = sin(angle) * speed;
            star.dz = ((rand() % 200) / 100.0f - 1.0f) * speed;
            star.life = 6.0f;

            stars.push_back(star);
        }
    }
}
