# Pacman Remake! 
Recreating the arcade classic using C++ and the Staffordshire University S2D Library</br>
Year 1 Project by May Broome.

![alt text](https://raw.githubusercontent.com/Kiemimi/FOGGS2021_Pacman/49616cf1869147e5f04ef736a6f1b66dd5cbbaa2/Pagegif.gif "Pacman Gameplay")

## Controls ⌨️
**WASD:** Movement</br>
**Space:** Start Game</br>
**P:** Pause</br>

## Features! ✨

* Level loading from a .txt file
* Point scoring
* Animation handling
* Sound playing
* AABB collision
* Powerup framework

## The Ghost! 👻
#### Blinky: 
1. Calculates the distance between itself and Pacman
2. Adds all possible movements to a "choice" array
3. Calculates a heuristic cost depending on distance and if the move is possible
4. Moves based on the lowest heuristic cost

## TODO: 🔭
* Polish the collision code to feel less jagged around corners
* Tune ghost movement to travel in the center of corridors
* Add the other ghosts
* Create a high score table
