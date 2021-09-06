# ELO[328]Procesamiento digital de imagenes.

## OpenCV C++ project on Linux

...

## OpenCV C++ project on Windows

* Opencv-4.4.0
* MinGW

### Compilation and execution

`g++ *.cpp -Wall -o video -L "C:\opencv440\install\x64\mingw\lib" -I "C:\opencv440\install\include" -lopencv_highgui440 -lopencv_imgcodecs440 -lopencv_core440 -lopencv_videoio440 -lopencv_imgproc440 icon.res -mwindows`

or just

`make`

and finally

`video.exe`

### Details

* I used `env.h` for important global variables.

