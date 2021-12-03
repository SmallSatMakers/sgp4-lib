# SGP4 Library

[![Build Status](https://travis-ci.org/SmallSatMakers/sgp4-lib.svg?branch=master)](https://travis-ci.org/SmallSatMakers/sgp4-lib)

SGP4 orbit propagator library forked from https://github.com/dnwrnr/sgp4.
Includes support for multi-object vectors, reading TLE files from disk and updating TLE information from text file URL.

## Building
```
   $ make -j$(NPROC)
```

## Test
Test programs are provided for runtime testing, pass prediction and object tracking in `examples` directory. Build process creates `.out` binaries in this directory.

## Linking with your own program
The build process creates a static library `libsgp4.a`, that can be linked against a custom program.
e.g.
```
   $ g++ -O2 -std=c++11 -Wall -I /path/to/sgp4-lib/include example.cpp -o example.out /path/to/sgp4-lib/libsgp4.a -lm
```
The included `Makefile` can be used as a reference if needed.
#### License

    Copyright 2017 Daniel Warner

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
