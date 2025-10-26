#pragma once
#include <Windows.h>
#include <cstdint>

namespace Hook {
    // VMT Hook 管理类
    class VmtHook {
    public:
        VmtHook();
        ~VmtHook();
        
        // 初始化 VMT 表（保存各接口的 vtable 指针）
        void Initialize(void** device, void** cmdQueue, void** cmdAllocator, 
                       void** cmdList, void** swapChain);
        
        // 安装 Hook（按合并索引）
        bool InstallHook(uint16_t index, void** originalFunc, void* hookFunc);
        
        // 卸载指定 Hook
        void UninstallHook(uint16_t index);
        
        // 卸载所有 Hook
        void UninstallAll();
        
    private:
        // 根据合并索引定位到具体 vtable 和局部索引
        bool LocateVmt(uint16_t index, void**& vmt, uint16_t& localIndex);
        
        // 各接口的 vtable 指针
        void** m_VmtDevice;
        void** m_VmtCommandQueue;
        void** m_VmtCommandAllocator;
        void** m_VmtCommandList;
        void** m_VmtSwapChain;
        
        // 保存原始函数指针（用于恢复）
        void* m_OriginalFuncs[150];
    };
}
