# JohnConwayGameofLife
A demonstration of the classic Game of Life. This game was originally invented by the British Mathmetician, John Conway.
This version, besides the game's basic algorithm, includes the following features:

- Renders the world using the olcPixelGameEngine by OneLoneCoder.com
- Allows the user to customize the size of the world and the update rate via command-line arguments.
- Allows for camera panning when the world exceeds 1024 X 768. (via WASD keys)

## Usage
The game can be built for any platform that the olcPixelGameEngine supports.

In order to customize the world, the following arguments are supported:

- "--worldWidth" controls the number of cells in the x-axis.
 - Default: 256
- "--worldHeight" controls the number of cells in the y-axis.
 - Default: 192
- "--updatesPerSecond" controls the number of times the simulation will try to update every second.
 - Default: 60

To keep the size of the window consistent, the pixel size of each cell will be adjusted if the world size is less than 1024 x 768.

## Batch Files

This repository includes .bat files to quickly showcase larger life worlds with. In order to use these files, please ensure that the
game has been built in **release** mode using the CPU type indicated in the name.
