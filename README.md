# NX Engine

## Description

3D Game Engine built in C++ using OpenGL. Originally a personal project kept local and private.

## Images

![Physics 1](images/physics_1.gif)

![Physics 2](images/physics_2.gif)

![Physics 3](images/physics_3.gif)

![Physics 4](images/physics_4.gif)

## Background

My nth attempt at making a game engine using OpenGL and C++, second or third in the 'modern' wave of game engines I attempted to make that were not in Java and featured things such as ECS. This engine spun off into my various attempts at learning different game engine features such as writing my own physics engine, rendering oceans, and buoyancy/ship physics. Supersedes/builds from [new-engine](https://github.com/forenoonwatch/new-engine) and is extended by [block-systems](https://github.com/forenoonwatch/block-systems) and [ocean-rendering](https://github.com/forenoonwatch/ocean-rendering).

Started in September of 2019 and sporadically worked on until August of 2020, with later minor tweaks mainly to continue support for ocean-rendering.

## Features

This engine, being a private personal project, is an absolute mess and different features are scattered in different folders and across different commits. Check my website for more information.

More information on the later features can be found in the projects:

- [new-engine](https://github.com/forenoonwatch/new-engine)
- [block-systems](https://github.com/forenoonwatch/block-systems)
- [ocean-rendering](https://github.com/forenoonwatch/ocean-rendering)

### General

- OpenGL Rendering Engine
- Deferred PBR/IBL shading
- Physics Engine
- ECS system
- Screen-space decals

### Later Commits

- ECS through EnTT
- Physics through Bullet (personal physics engine abandoned)

### Earlier Commits

#### Physics Engine

- Support for sphere, plane, convex polygon, and capsule colliders
- Implementation of SAT and GJK
- Collision Groups
- Islanding
- Inactive object sleeping
- Sequential Impulse solver

## Building / Usage

Use GCC and GNU Make. Good luck.

## Future Work

- Vulkan-based rendering engine
- Revamp of physics engine to make it more stable and actually usable beyond tinkering

