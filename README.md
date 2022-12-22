# Slippi SSBM C
## Project Slippi
This repository is part of the Project Slippi ecosystem. For more information about all of the Project Slippi projects, visit https://github.com/project-slippi/project-slippi.

## Setup
Initialize the m-ex submodule in your local project
```
git submodule init
git submodule update
```
## Build Instructions
### Windows
1. In order to build this project you will need to [download](https://github.com/devkitPro/installer/releases) the `devkitPro` tools for Gamecube / Wii.
2. Before running the build command you need to create an output directory for m-ex .dat files
```
mkdir build
```
Then to build the .dat files to ./output
```
build.bat
```
