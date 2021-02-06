# JohnConwayGameofLife
A demonstration of the classic Game of Life. This game was originally invented by the British Mathmetician, John Conway.
This version was created using the olcPixelGameEngine by OneLoneCoder.com. An executable file can be downloaded from the release section.

The Game of Life is a 2 dimensional grid of cells that perform a simple simulation. A cell in the game can either be alive(white) or dead(black). The game follows the following rules:

- A cell that is **alive** keeps living if it has 2 or 3 live neighbors, otherwise it dies.
- A cell that is **dead** comes to life if it has exactly 3 live neighbors.

## Controls

The Game of Life is just a simulation, and on it's own it has no user input. This version, however, includes some user controls that add a little bit to the experience:

- Pause and resume the simulation using the **spacebar**.
- Specify custom dimensions for the world using command-line arguments(see below).
- Pan around the world using the WASD keys(only usable if the world size exceeds 1024x768).

Note: The size of each cell(in pixels) will adjust to the different grid sizes you give it.

## Command-line arguments

This game achieves custom world size and look through 3 command-line arguments. They are:

- **--width** The width of the grid. Default: 256
- **--height** The height of the grid. Default: 192
- **--seed** Used to generate different worlds based on the value. Default: 0

The release zip folder contains some .bat files that include some sample custom sizes. The **--seed** argument is a bit limited in how varied it can make the world. 
It is just a random number between 0 and 1 after all.
