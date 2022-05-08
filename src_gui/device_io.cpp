#include "device_io.h"

HANDLE device_io::GetHandleToIo(char* deviceName)
{
    return CreateFileA(deviceName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
}

bool device_io::CloseIoHandle(HANDLE hIo)
{
    if(hIo == INVALID_HANDLE_VALUE)
        return false;
    else
        CloseHandle(hIo);

    return true;
}

bool device_io::WriteToDevice(HANDLE hDevice, wchar_t* inBuffer)
{
    return WriteFile(hDevice, inBuffer, wcslen(inBuffer) * sizeof(wchar_t), 0, 0);
}
