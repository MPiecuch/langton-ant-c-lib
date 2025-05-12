# Langton's Ant
![logo](img/logo.png)
## Description
[Detailed description on Wikipedia](https://en.wikipedia.org/wiki/Langton%27s_ant)
## Installation
```
make
sudo make install //optional
```

Command `make` will create build folder with structure:
```
build
├── ant
│   ├── include
│   │   └── ant.h
│   ├── libant.a
│   └── libant.so
└── ant.o
```

Command `make install` by default installs this library in `/usr/lib` and `/usr/include`.
One can install library in other localization by defining PREFIX.
```
make install PREFIX=./install
```

## Build and run tests

After building library, run:
```
make test
cd test
./test
```