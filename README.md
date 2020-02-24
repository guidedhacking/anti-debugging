# GuidedHacking - Debugme 

Implementation of some anti-debugging techniques on a (bad looking) Win32 application. The idea is to cover most used anti-debugging methods, so feel free to drop a Pull Request anytime 😏.

![](https://i.imgur.com/7nLJ5Ff.png)

## How to use it

You can compile yourself with Visual Studio 2019+ (no special instructions needed) or just download the binary on the release tab. Fire it up, attach a debugger and start enabling detection methods. Then, try to bypass some and have fun.

## How to add a new anti debugging method 

1. Create a new file `.h` on the Methods folder with the name of the method. 
2. Implement your anti debugging function on the new file. This function will run on the main loop if enabled. It has to return a `bool` stating if a debugger was detected or not.
2. On `anti-debugging.cpp`, find the lines where it creates instances of `AntiDebugMethod` class on the heap. Create a new instance, passing the pointer to your function, coordinates for the button on the UI and the method name like:
```cpp
    AntiDebugMethod* sixthOne = new AntiDebugMethod(MethodUnhandledException, 270, 220, "UnhandledExceptionFilter");
	sixthOne->createGUI(hWnd);`
```
## TODO
- Add more methods
- Add support for x64 (there were some specifics I would have to deal for a few methods to support x64, and I got lazy to do it on V1).
- Change the UI (either make it look good/modern or completely shitty like a Win95 program).

## Credits

Big shoutout to everyone at [guidedhacking.com](https://guidedhacking.com) that helped me with the development of this application. It's indeed the best resource to learn game hacking and reverse engineering. You should check it out if you haven't.
