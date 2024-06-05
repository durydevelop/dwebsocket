# CMake Doxygen support
This is a full customization support for creating Doxygen documentation with CMake.
## Features
* [X] CMakeLists.txt for add_subdirectory().
* [ ] Doxygen.cmake for include().
* [X] Use different custom themes.

## Usage
**Mode 1**: copy this folder in your project root and add
```cmake
    add_subdirectory(doxygen)
```
now, you can use ```make doc``` to build documentation
 

**Mode 2**: include(Doxygen)
- TODO

## Customize
Setting DOXYGEN_THEME variable, let you use custom theme:
* Create a new folder in doxygen/theme/
* Copy content of doxygen/themes/example folder (this is a minimal customization example conatining a footer, an header, a css and a logo)
* Now You can modify CMakeLists.txt according to https://www.doxygen.nl/manual/customize.html

All CMake valiables must be named DOXYGEN_\<name>

You can also set DOXYGEN_\<name> variable in main CMakeLists.txt **before** add_subdirectory() or include():

``` cmake
    set(DOXYGEN_THEME custom1)      # custom1 theme
    set(DOXYGEN_LANGUAGE Italian)   # Use italian language
    add_subdirectory(doxygen)
```
