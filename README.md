# 3D Battleground – Animation FYP Project

## Overview
This project is a 3D Pokémon-inspired battle simulation featuring Kirby and Snorlax models. Developed using **OpenGL**, it includes animations, interactive gameplay, HP/stat management, and user controls for a full battle experience.

---

## Features
- 3D models of Kirby and Snorlax with animated movements.
- HP bars and stat system for each character.
- Interactive user controls to simulate attacks and skills.
- OpenGL rendering with lighting and camera control.
- Asset management for textures, animations, and audio.

---

## Project Structure
```text
Animation-3D-Battleground/
│
├─ assets/              # 3D models, textures, and audio files
│   ├─ models/          # OBJ or other model files
│   ├─ textures/        # PNG/JPG texture images
│   └─ audio/           # Sound effects and music
│
├─ src/                 # Source code files
│   ├─ main.cpp         # Main executable program
│   ├─ animation.cpp    # Animation system for characters
│   ├─ battle.cpp       # Battle logic, HP and stats system
│   └─ utils.cpp        # Utility functions
│
├─ shaders/             # GLSL shader programs
│   ├─ vertex_shader.glsl
│   └─ fragment_shader.glsl
│
├─ COMBINED.cbp         # Project configuration file
├─ COMBINED.layout      # IDE layout file
├─ README.md            # Project documentation
└─ demo_video_link.txt  # Link to demo video
