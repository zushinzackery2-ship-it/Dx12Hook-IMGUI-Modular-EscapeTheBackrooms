#pragma once
#include "D3D12Hook.h"
#include "VmtHook.h"

namespace Hook {
    // Hook 管理器（单例）
    class HookManager {
    public:
        // 获取单例
        static HookManager& Instance();
        
        // 初始化 Hook 系统
        bool Initialize();
        
        // 安装 Hook
        bool InstallHook(uint16_t index, void** originalFunc, void* hookFunc);
        
        // 卸载 Hook
        void UninstallHook(uint16_t index);
        
        // 卸载所有 Hook
        void UninstallAll();
        
    private:
        HookManager();
        ~HookManager();
        
        // 禁止拷贝
        HookManager(const HookManager&) = delete;
        HookManager& operator=(const HookManager&) = delete;
        
        D3D12Hook m_D3D12Hook;
        VmtHook m_VmtHook;
        bool m_Initialized;
    };
}
