/*
 * Snorlax.hpp
 * Header for Snorlax character class with Hyper Beam attack system
 * Features: Complex body modeling, mouth animation, beam effects
 */

#ifndef YP_SNORLAX_HPP
#define YP_SNORLAX_HPP

#include "CGLabmain.hpp"

namespace Snorlax {

class MyVirtualWorld {
public:
    MyVirtualWorld();
    void init();          // Initialize Snorlax character
    void draw();          // Draw complete Snorlax model
    void tickTime();      // Update animations and effects

    // Hyper Beam attack system
    float beamLength = 0.0f;
    void startHyperBeam();
    float showBeam = false;
    float beamFlashDuration = 2.0f;  // Flash duration before beam appears
    float maxBeamLength = 20.0f;
    float beamSpeed = 0.4f;          // Beam growth speed

    // Beam flash effect variables
    bool beamFlashActive = false;
    float beamFlashRadius = 0.0f;
    float maxBeamFlashRadius = 1.5f;
    float beamFlashSpeed = 0.2f;     // Flash expansion speed
    bool beamFlashVisible = false;   // Flash visibility state
    float beamFlashTimer = 0.0f;     // Flash duration timer

    // Mouth animation system
    bool mouthAnimating = false;     // Animation trigger
    bool mouthIsOpen = false;        // Current mouth state
    float mouthOpenProgress = 0.0f;  // Animation progress (0.0 = closed, 1.0 = fully open)

    // HP system for battle mechanics
    float currentHP = 100.0f;
    float maxHP = 100.0f;

private:
    // Character geometry parameters
    float size;
    float xPos, yPos, zPos;         // Position variables
    float bodyRadius;
    GLUquadric* body;
    int slices;
    int stacks;

    // Drawing functions for different body parts
    void drawBody();                // Main body with belly
    void drawArms();                // Arms with claws
    void drawLegs();                // Legs with feet and claws
    void drawHead();                // Head with facial features
    void drawMouth();               // Animated mouth with teeth
    void drawHyperBeam();           // Main beam attack effect
    void drawBeamFlash();           // Initial flash effect
};

} // namespace Snorlax

#endif // YP_SNORLAX_HPP
