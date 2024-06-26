# FRASCR

Linux-based application built to run calculations and output the results as png files. While the core application doesn't do much more than process command-line arguments or config files, the application includes dynamically-loaded code for doing all the dirty work. Why?
 -  For old-schoolers who want to use nitty-gritty C and haven't yet learned about the frustration of Rust... 
 -  Writing a new program every time you want to, say, analyze a slightly different fractal set, can be super laborious
 -  But, using a higher-level language might mean code that should run under a minute won't
 -  Also, using gnuplot to produce a new graphical output could incur many minutes of extra execution
 -  And, Mathematica is too expensive because, say, your institution doesn't want to foot the bill
 -  Oh yeah, and if like me you couldn't find a single colorspace library in C for some reason

The framework of the project itself has so few constraints that you can use it to generate "fly-through" fractal videos, on the condition you write the lib files necessary. In fact, there's nothing requiring you to use it to compute fractals.

<img src="https://github.com/eightbitastronomy/frascr/blob/main/data/exp3_mand_bw_1.png" width=50% height=50%>
<img src="https://github.com/eightbitastronomy/frascr/blob/main/data/mand_clr_1.png" width=50% height=50%>

## Current functionality
 -  Command-line argument processing OR json-based configuration file processing
 -  NB: command-line args only offer a little flexibility; use a json config file instead
 -  Mandelbrot set calculation for a quadratic function
 -  png output in either black & white (more useful than it might seem)
 -  png output in 8-bit or 16-bit hue-shift color
 -  output in text only, but that's nothing to write home about

## Dependencies / level of neediness:
 - C99 due to use of stdint.h  
 - cmake build system
 - libpng 1.6.40 or higher, I think
 - zlib
 - json-c
 - and patience
 
## Installation and configuration:
 -  No installation. Download, cd into build, run "cmake ..", then "cmake --build .". Your executable will be "frascr". Take a look at the config files in the source or use "frascr -h" to get an idea of how to run it.
 - contact me if you have questions or issues

## For the future:
 - [ ]  Expand the color library into something more independent and fully-functioning (WIP)
 - [ ]  Make a libcolorpng that uses GPU for the color conversions & interpolation
 - [ ]  The sky's the limit

## Contact / Info:
Author: Miguel Abele
Contact: eightbitastronomy@protonmail.com
License:
 - see file, LICENSE, provided with source code.
 - File bitorder.h has no licensing and is free to use.
 - see color library files for info on data/equations not copyrightable by me, some of which appearing to be in the public domain.
