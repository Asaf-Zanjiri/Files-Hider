#ifndef DEVICE_IO_H
#define DEVICE_IO_H

#include "windows.h"

class device_io
{
public:
    /// Create and return a handle to an IO device.
    static HANDLE GetHandleToIo(char* deviceName);

    /// Close handle to an IO device.
    static bool CloseIoHandle(HANDLE hIo);

    /// Write a wchar string to an open IO device.
    static bool WriteToDevice(HANDLE hDevice, wchar_t* inBuffer);
};

#endif // DEVICE_IO_H
