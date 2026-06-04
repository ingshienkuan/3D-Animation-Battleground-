/*
 * Kirby.hpp
 * Header for Kirby character class with floating and particle effects
 * Features: Animation system, 3D star particles, floating mechanics
 */

#ifndef YP_KIRBY_HPP
#define YP_KIRBY_HPP

#include <cmath>
#include <GL/glut.h>
#include <vector>

namespace Kirby
{
    // Star particle structure for burst effect
    struct Star {
        float x, y, z;          // Position
        float dx, dy, dz;       // Velocity
        float life;             // Remaining life time
    };

    class MyVirtualWorld
    {
    private:
        // Rotation for idle animation
        GLfloat rotX, rotY, rotZ;
        GLfloat rotSpeedX, rotSpeedY, rotSpeedZ;

        // Body geometry parameters
        GLdouble bodyRadius;
        GLint slices, stacks;

        // Animation and rendering control
        bool isAnimating;
        bool isWireframe;

        // Floating mechanics
        float floatHeight;
        float floatSpeed;
        bool floatUpward;

        // Star burst particle system
        bool burstActive;
        float burstRadius;
        std::vector<Star> stars;
        float screenFlashAlpha;

    public:
        MyVirtualWorld();

        // Core functions
        void init();
        void draw();
        void update(float elapseTime);
        void toggleAnimation();
        void triggerBurst();

        // Star particle system functions
        void updateStars(float deltaTime);
        void drawStars();
        void draw3DStar(float x, float y, float z, float size);

        // Public animation state
        bool isFloating;

        // HP system for battle mechanics
        float currentHP = 100.0f;
        float maxHP = 100.0f;
    };
}

#endif // YP_KIRBY_HPP
