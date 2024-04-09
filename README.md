# FRASCR

Linux-based application built to run calculations and output the results as png files. While the core application doesn't do much more than process command-line arguments or config files, the application includes dynamically-loaded code for doing all the dirty work. Why?
 -  For old-schoolers who want to use nitty-gritty C
 -  and haven't yet learned about Rust... Writing a new program every time you want to, say, analyze a slightly different fractal set, can be super laborious
 -  But, using a higher-level language might mean code that should run under a minute won't
 -  Also, using gnuplot to produce a new graphical output could incur many minutes of extra execution
 -  And, Mathematica is too expensive because, say, your institution doesn't want to foot the bill
 -  Oh yeah, and if like me you couldn't find a single colorspace library in C for some reason

## Current functionality
 -  Command-line argument processing OR json-based configuration file processing
 -  Mandelbrot set calculation for a quadratic function
 -  png output in either black & white (more useful than it might seem) or in 8-bit hue-shift color
 -  output in text only, but that's nothing to write home about

## Dependencies / level of neediness:
 - cmake build system
 - libpng
 - zlib
 - json-c
 - and patience
 
## Installation and configuration:
 -  No installation. Download, cd into build, run "cmake ..", then "cmake --build .". Your executable will be "frascr". Take a look at the config file in the source or use "frascr -h" to get an idea of how to run it.
 - contact me if you have questions or issues

## For the future:
 - [ ]  Rework so that 8-bit and 16-bit channel output can be selected as a config option
 - [ ]  Implement more goshdarn fractal libraries
 - [ ]  Expand the color library into something more independent and fully-functioning
 - [ ]  The skies the limit

## Contact / Info:
Author: Miguel Abele
Contact: eightbitastronomy@protonmail.com
License: see file, LICENSE, provided with source code.
