
# 3D Maze multi-monitor mod
This is a mod to enable multi-monitor support on the 3D Maze Windows 95 screensaver.

## How to use
- **You need to enable "Full Screen" mode on the configurations for the multi-monitor support to work properly**, or else you'll most likely see a black screen on the other monitors ("most likely" because sometimes it decides to render the maze on them with full screen mode off - no idea why it doesn't just always render it. It seems to only render when there's a notification that appears above the screensaver for some reason).
- Download and copy the 3 files to some folder of your choice.
- Right click the SCR file and click Install.
- Configure it through the window and from the INI file and that's it!

Tested and working on 2 and 3 monitors (should work on any number of monitors though). If only one monitor is attached to the computer, the mod doesn't do anything at all about the multi-monitor support and lets the screensaver do its normal job.

## Download
Go to the [Releases](https://github.com/Edw590/3DMazeMod/releases) and download the most recent one.

## Pictures
<img src="Pictures/Dual-monitor 3D Maze.png">

## How I did it
First of all I need to thank @\_lumberjack\_ on Discord for helping me with this. They gave me pointers to be able to achieve this (the right Win32 function to use).

I did this in a week (was a quick project it seems), and in that week I got various ideas (some from me, others from random people and friends):
- Render the textures in all windows contexts, one at a time, sharing textures between contexts with `wglShareLists()` (this would work perfectly, but I had problems with the approach: picture is loaded slowly; resolution maxed out always; wrong colors on ceiling and floor; no transparency in Windows logo; white background on the rat)
- Move the main window where all is drawn between the monitors very fast (was blinking too much, couldn't go with it)
- Use `glReadPixels()` to copy pixels between the windows (a white image would appear on the other monitors because of an error out of that function: 1281 == 0x501 == GL_INVALID_VALUE)
- Launch multiple instances of the maze on the various monitors (should work, but would have different mazes - and anyway, there's another problem: when I'd move the mouse over one program, the others would remain running, because they're different instances)
- **Final and used approach:** use `StretchBlt()` to copy pixels between contexts (copies what's rendered on the main window to the other windows). This function copies and stretches or shrinks the image as needed for the new window dimensions.

I began by modifying the EXE manually with IDA and HxD. I'd assemble the instructions on IDA and copy the modified bytes to HxD to patch the EXE. I kept on it for days while I was trying to see if I could pull this off, and only with 2 monitors with specific resolutions and positions relatively to the main monitor.

After some time I decided to finally move to making and injecting a DLL (for the first time) on the EXE (very near of what I did with the F1DP (Fallout 1 DOS Patcher) project). That enabled me to finally automate the code and detect the monitors automatically (I could have done this in Assembly, but would have taken more time and would be nonsense when I can do it in C).

So the project is written in C + Inline Assembly. The Inline Assembly is to patch directly the EXE, while C code is called from the Assembly instructions. I took ideas and functions from F1DP.

Note: to get some functions to be accessible from the EXE, I had to find a way to insert the references to the functions in the EXE (to the PE's IID) - I used `IIDKing 2.01` by SantMat as suggested by the book `Portable Executable File Format - A Reverse Engineer View`. Later I realized the functions were no longer needed and so I removed all references again, but the program was very useful to test other approaches until I got to the working one.

Note 2: in the middle of the path I got my hands on the source of the maze program. Apparently it comes with Visual Studio 5 from 1997. I don't know how to compile it, so I kept patching the already compiled program and used the source as a guide for the mods. Was still really cool to (without the source) manage to:
- create a new section on the PE file and add code to it that could be called from the original code section;
- create a new window;
- switch the windows between monitors very fast;
- share the textures and switch the context to the second monitor.

Only didn't do more because would be nonsense when I could just learn to code a DLL and do it mostly in C.

## License
This project is licensed under Apache 2.0 License -  [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0).
