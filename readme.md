# Maze finder
A 2,5d doom-like game about solving mazes. Made mainly as an exercise. Written in C++ using the SFML graphics library, however SFML is only used for keyboard access and setting the values of specific pixels, the rendering logic is written by myself based on [this tutorial](https://github.com/vinibiavatti1/RayCastingTutorial).

## Features
### 3d-like perspective
![image](./screenshots/Screenshot%20from%202024-03-09%2023-24-01.png)
![image](./screenshots/Screenshot%20from%202024-03-09%2023-22-47.png)
![image](./screenshots/Screenshot%20from%202024-03-09%2023-23-16.png)

### Texture suppport
![image](./screenshots/Screenshot%20from%202024-03-09%2023-20-17.png)
![image](./screenshots/Screenshot%20from%202024-03-09%2023-59-34.png)
Any .ppm P3 image can be loaded as a texture.

### Procedural generation
![image](./screenshots/Screenshot%20from%202024-03-09%2023-25-16.png)
Mazes are generated procedurally, increasing in difficulty.

## Compilation
Dependency: [SFML](https://www.sfml-dev.org/)

Set settings in the [settings.txt](./settings.txt) and run
```bash
cd src && make main && mv main ../main
```

## Controls
- up / down - forwards / backwards
- left / right - look left / right
- left alt + left / right - move left / right
- spacebar - toggle minimap