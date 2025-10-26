#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <cstdint>

#if defined _M_X64
typedef uint64_t uintx_t;
#elif defined _M_IX86
typedef uint32_t uintx_t;
#endif

namespace Hook {
    // D3D12 Hook 管理类
    class D3D12Hook {
    public:
        D3D12Hook();
        ~D3D12Hook();
        
        // 初始化：创建临时设备并提取 vtable
        bool Initialize();
        
        // 获取方法表（用于索引换算）
        uintx_t* GetMethodsTable() { return m_MethodsTable; }
        
        // 获取各接口的 vtable
        void** GetDeviceVmt() { return m_VmtDevice; }
        void** GetCommandQueueVmt() { return m_VmtCommandQueue; }
        void** GetCommandAllocatorVmt() { return m_VmtCommandAllocator; }
        void** GetCommandListVmt() { return m_VmtCommandList; }
        void** GetSwapChainVmt() { return m_VmtSwapChain; }
        
    private:
        // 创建临时窗口
        bool CreateTempWindow();
        void DestroyTempWindow();
        
        uintx_t* m_MethodsTable;
        
        // 各接口的 vtable 指针
        void** m_VmtDevice;
        void** m_VmtCommandQueue;
        void** m_VmtCommandAllocator;
        void** m_VmtCommandList;
        void** m_VmtSwapChain;
        
        // 临时窗口
        WNDCLASSEXW m_WindowClass;
        HWND m_WindowHwnd;
    };
}
