Super Mario Bros – SFML 3 Clone

Welcome to my personal take on the legendary Super Mario Bros, built from the ground up using C++ and the latest SFML 3 framework. This project is a labor of love where I’ve focused on recreating the tight physics, iconic level design, and nostalgic atmosphere of the original NES classic while utilizing modern development standards.

Project Overview

In this project, I have implemented a fully functional platforming engine that handles complex tile-based collisions, entity management, and state-driven animations. My goal was to move beyond a simple clone and create a robust codebase that demonstrates how classic 2D mechanics can be efficiently programmed in a modern C++ environment.

Technical Features

SFML 3 Integration: I am utilizing the newest features of SFML 3, including the updated event system (using std::optional and type-safe event handling) and improved rendering techniques.

Custom Retro Text Engine: Pentru a păstra estetica 8-bit și pentru a evita problemele de compatibilitate ale fonturilor standard (.ttf), am creat de la zero un sistem de randare a textului bazat pe un Bitmap Font. Acesta mapează caracterele direct pe o foaie de sprite-uri (sprite sheet), oferind un text pixel-perfect și eliminând dependențele externe de fonturi.

Custom Physics Engine: I developed a custom collision system that manages Mario’s interactions with the environment, including gravity, momentum, and pixel-perfect landing on platforms.

Dynamic Tilemaps: I built a MapManager that handles different tile types—from breakable bricks and interactive question blocks to solid pipes and hidden items.

AI & Entity System: I've implemented a polymorphic system for enemies and power-ups. Whether it's a Goomba patrolling a platform or a Mushroom spawning and bouncing off walls, each entity has its own unique behavior logic.

Game State Management: The game tracks scores, coin counts, lives, and world progression across multiple levels, providing a complete "game loop" experience.

Why I Built This
This repository serves as a showcase of my skills in object-oriented programming (OOP) and game engine architecture. By recreating Mario, I tackled the fundamental challenges of game development: synchronization of logic and rendering, memory management through smart pointers, and creating an intuitive user interface.
