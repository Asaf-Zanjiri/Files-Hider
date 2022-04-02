# Files Hider
Have you ever wanted to hide your files from prying eyes? Well you're in luck! With this application you will be able to hide files within your system that even your operating system won't know exists!
How does it work? The software is made with a driver, that hooks window's file query functions, and strips out the files you want hidden!

*put gif here*

## Features
- Intuitive User Interface to add/remove files to hide.
- Mini-Tray to minimize the app to run in the backgroud.
- Completely hide files/folders in file explorer / Native searches (e.g. cmd).
- Documented code to make it easier to follow.

## Brief Overview
Upon the driver start, the driver does multiple things:
- Register an IO device.
- Initialize IRP communications (To communicate with the GUI).
- Hook Window's Nt\* functions by replacing the function pointer in the SSDT table.
Upon the driver closing, all of the actions above would be reverted back to how they were before the driver started.

During the driver running, the driver does multiple things:
- Listen to IRP communications from the user and manage a dynamic list of paths the user wants to hide.
- Hooks Window's file querying functions, and iterates over the returned paths, and then checks if they appear in the hide list supplied by the user. The driver first checks by path length if the path exists, then by string value, to make the iteration process faster.

The user-interface manages the activation states & communications with the driver. To make it possible, the application must be ran as an administrator. To enforce that i've added a manifest file to the application to pervent the user interface from starting without the right permissions.
The user-interface has been made with Qt Library.

## Notes
- You can compile the driver's code in Visual-Studio & GUI with Qt 5.15.1 MinGW 32-bit (If you have a problem with Qt dependencies when compiling, look up windeployqt).
- Make sure you place both the driver and the GUI at the same folder (Otherwise the software might not find the driver).
- Software has only been tested on Windows 10 Pro (10.0.19044 Build 19044).
- The more paths you hide, the more you can anticipate performance delays.
- This software is only compatible with 32-bit machines (Mainly due to PatchGuard).
- Debug prints are still left inside the code. It's possible to view them by using software like DbgView (https://docs.microsoft.com/en-us/sysinternals/downloads/debugview).
- If you want to change the driver's name, simply change `unicodeDeviceName` & `unicodeSymLink` in driver.h (make sure you also edit the defines at mainwindow.h in the GUI code accordigly).

## Downloads
*insert pre-compiled download here*

## Citations
- https://www.unknowncheats.me/forum/c-and-c/59147-writing-drivers-perform-kernel-level-ssdt-hooking.html (Great reasource for information about drivers and hooking)
- https://github.com/bytecode77/r77-rootkit/ (Great example of hooking implementation)