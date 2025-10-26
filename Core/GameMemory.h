#pragma once
#include <Windows.h>
#include <cstdint>

namespace GameMemory {
    // 内存读取
    template<typename T>
    inline T Read(DWORD64 address) {
        if (!address || IsBadReadPtr((void*)address, sizeof(T))) {
            return T{};
        }
        __try {
            return *(T*)address;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            return T{};
        }
    }
    
    // 内存写入
    template<typename T>
    inline bool Write(DWORD64 address, T value) {
        if (!address || IsBadWritePtr((void*)address, sizeof(T))) {
            return false;
        }
        __try {
            *(T*)address = value;
            return true;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }
    
    // 快捷方式
    inline DWORD64 ReadPtr(DWORD64 address) { return Read<DWORD64>(address); }
    inline float ReadFloat(DWORD64 address) { return Read<float>(address); }
    inline int ReadInt(DWORD64 address) { return Read<int>(address); }
    inline bool WriteFloat(DWORD64 address, float value) { return Write<float>(address, value); }
}
