#include "HookManager.h"

namespace Hook {
    
    HookManager::HookManager() : m_Initialized(false) {
    }
    
    HookManager::~HookManager() {
        UninstallAll();
    }
    
    HookManager& HookManager::Instance() {
        static HookManager instance;
        return instance;
    }
    
    bool HookManager::Initialize() {
        if (m_Initialized) return true;
        
        // 初始化 D3D12 Hook（提取 vtable）
        if (!m_D3D12Hook.Initialize()) {
            return false;
        }
        
        // 初始化 VMT Hook
        m_VmtHook.Initialize(
            m_D3D12Hook.GetDeviceVmt(),
            m_D3D12Hook.GetCommandQueueVmt(),
            m_D3D12Hook.GetCommandAllocatorVmt(),
            m_D3D12Hook.GetCommandListVmt(),
            m_D3D12Hook.GetSwapChainVmt()
        );
        
        m_Initialized = true;
        return true;
    }
    
    bool HookManager::InstallHook(uint16_t index, void** originalFunc, void* hookFunc) {
        if (!m_Initialized) return false;
        return m_VmtHook.InstallHook(index, originalFunc, hookFunc);
    }
    
    void HookManager::UninstallHook(uint16_t index) {
        if (m_Initialized) {
            m_VmtHook.UninstallHook(index);
        }
    }
    
    void HookManager::UninstallAll() {
        if (m_Initialized) {
            m_VmtHook.UninstallAll();
            m_Initialized = false;
        }
    }
}
