#ifndef BATTLEGROUND_HPP
#define BATTLEGROUND_HPP

namespace BattleGround {
class MyVirtualWorld {
public:
    MyVirtualWorld();
    void init();
    void draw();
    void tickTime();
    void drawPlatform();
    void drawLetter(float x, float y, float scale, char letter);

private:
    // Private helper methods to organize code
    void drawPlatformBase();
    void drawPlatformTop();
    // Add these function declarations to your MyVirtualWorld class in BattleGround.hpp:
    void drawSegmentedPlatformTop();
    void drawPlatformSegment(float inner_radius, float outer_radius, float start_angle, float end_angle);
    void drawRingGap(float inner_radius, float outer_radius, float depth);
    void drawStar();
    void drawGrassLayerWide(float R1, float R2, float height, float z, int segs, float angle_offset, float colR, float colG, float colB);
    void drawMultiLayerGrassRing(float base_r, float h, float base_z);
    void drawBattlePlatform();
    void drawCloud3D(float x, float y, float z, float base_r);
    void drawFloatingStone(float x, float y, float z, float scale, float angle);
    void drawGradientBackground();
    void drawPokeball();
    void drawLetterS();
    void drawLetterN();
    void drawLetterO();
    void drawLetterR();
	void drawLetterL();
    void drawLetterA();
    void drawLetterX();
    void drawLetterT();
    void drawLetterC();
    void drawLetterK();
    void drawLetterD();
    void drawLetterE();
    void drawLetterF();
    void drawLetterP();
    void drawLetterI();
    void drawLetterB();
    void drawLetterY();
    void drawLetterG();  // Added for "GAME"
    void drawLetterM();  // Added for "GAME" and "OVER"
    void drawLetterV();  // Added for "OVER"
    void drawLetterW();  // Added for "WINS"
    void drawHPBar(float centerX, float centerY, float length, float height, float currentHP, float maxHP);
    void drawRoundedRect(float x, float y, float w, float h, float r);
    void drawRoundedRectOutline(float x, float y, float w, float h, float r);
    void drawOuterHPBar(float centerX, float centerY, float length, float height);
    void drawAttackBar(float centerX, float centerY, float length, float height);
    void drawDefenseBar(float centerX, float centerY, float length, float height);
    void drawSpeedBar(float centerX, float centerY, float length, float height);
    void drawHPStar();
    void drawKirbyOuterHPBar(float centerX, float centerY, float length, float height);
    void drawKirbyRoundedRect(float x, float y, float w, float h, float r);
    void drawKirbyHPBar(float centerX, float centerY, float length, float height, float kirbycurrentHP, float kirbymaxHP);
    float currentHP();
    float maxHP();

    // Constants
    static const float platform_radius;
    static const float platform_height;
    static const int platform_segments;
    static const int star_points;
    static const float star_outer_radius;
    static const float star_inner_radius;

    // Instance variables
    float elapsedTime;
    float stoneZ[16];
    float stoneAngle[16];
    float cloudX[12];
};
} // namespace BattleGround

#endif // BATTLEGROUND_HPP
