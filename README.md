# The Ball Lines Game

This is a Qt4 desktop application. The balls and the scene are drawn using the `QPainter` library.

![](/screenshots/level-1.png?raw=true "Lines at level 1")<br/>
![](/screenshots/level-3.png?raw=true "Lines at level 3")<br/>
![](/screenshots/moving-1.png?raw=true "Moving ball")<br/>
![](/screenshots/moving-2.png?raw=true "Moving ball")

#### Usage
Download [bin/Lines.zip](bin/Lines.zip?raw=true), unzip. You'll find the `Lines` executable inside.
It's compiled under Ubuntu 20.04. Run it from command line: `./Lines`.
Enjoy the game :sparkles:

##### Manual Compilation
Install Qt4. Under Ubuntu 20.04 you'll get the `qmake-qt4` makefile generator.

```bash
cd path/to/lines

# generate Makefile
qmake-qt4 src/Lines.pro

# Compile the executable
make all clean

# Run the game
./Lines
```

#### Author
[Meliq Pilosyan](https://github.com/melopilosyan)
