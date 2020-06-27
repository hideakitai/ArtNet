# TeensyDirtySTLErrorSolution

dirty solution to use STL in teensy

## Introduction

Sometimes we want to use STL on Teensy, but Teensy 3.x in Arduino IDE does not support STL by default. Though STL is enabled on Teensy 4.x, but you have to enable it manually on Teensy 3.x. If you use PlatformIO, you can also use STL on Teensy 3.x.

|             | Teensy 4.x        | Teensy 3.x        |
| ----------- | ----------------- | ----------------- |
| PlatformIO  | Almost Compilable | Almost Compilable |
| Arduino IDE | Almost Compilable | NOT Compilable    |


## Enable STL in Arduino IDE

To enable STL on Teensy 3.x using Arduino IDE, add `-lstdc++` to `build flags`, which is defined in `boards.txt`. Many STL can be used by adding this flag. You have to do this procedure every time you upgrade the version of Teensyduino.

| OS      | default location                                                |
| ------- | --------------------------------------------------------------- |
| macOS   | `Arduino.app/Contents/Java/hardware/teensy/avr/boards.txt`      |
| Windows | `C:\Program Files (x86)\Arduino\hardware\teensy\avr\boards.txt` |


#### Before

``` boards.txt
 teensy36.build.flags.libs=-larm_cortexM4lf_math -lm
 teensy35.build.flags.libs=-larm_cortexM4lf_math -lm
 teensy31.build.flags.libs=-larm_cortexM4l_math -lm
```

#### After

``` boards.txt
 teensy36.build.flags.libs=-larm_cortexM4lf_math -lm -lstdc++
 teensy35.build.flags.libs=-larm_cortexM4lf_math -lm -lstdc++
 teensy31.build.flags.libs=-larm_cortexM4l_math -lm -lstdc++
```

## Do you still have compile error?

Yes, maybe you still have compilation error in both 4.x and 3.x for some STLs... Then you should add this library (header file) to your project. Some dirty solution I found is included in this header file.

``` C++
#include <TeensyDirtySTLErrorSolution.h>
```

## Warning

This method is not recommended because STL is not enabled by default... (if you know the reason, please let me know). I am not responsible for any trouble that may occur as a result of the use of this library. Please use at your own risk.  


## License

MIT

