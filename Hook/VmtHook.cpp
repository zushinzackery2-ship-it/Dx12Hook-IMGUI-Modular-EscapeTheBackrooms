#include "VmtHook.h"
#include <cstring>

namespace Hook {
    
    VmtHook::VmtHook() 
        : m_VmtDevice(nullptr)
        , m_VmtCommandQueue(nullptr)
        , m_VmtCommandAllocator(nullptr)
        , m_VmtCommandList(nullptr)
        , m_VmtSwapChain(nullptr) {
        memset(m_OriginalFuncs, 0, sizeof(m_OriginalFuncs));
    }
    
    VmtHook::~VmtHook() {
        UninstallAll();
    }
    
    void VmtHook::Initialize(void** device, void** cmdQueue, void** cmdAllocator, 
                             void** cmdList, void** swapChain) {
        m_VmtDevice = device;
        m_VmtCommandQueue = cmdQueue;
        m_VmtCommandAllocator = cmdAllocator;
        m_VmtCommandList = cmdList;
        m_VmtSwapChain = swapChain;
    }
    
    bool VmtHook::LocateVmt(uint16_t index, void**& vmt, uint16_t& localIndex) {
        if (index < 44) {
            vmt = m_VmtDevice;
            localIndex = index;
        } else if (index < 44 + 19) {
            vmt = m_VmtCommandQueue;
            localIndex = index - 44;
        } else if (index < 44 + 19 + 9) {
            vmt = m_VmtCommandAllocator;
            localIndex = index - (44 + 19);
        } else if (index < 44 + 19 + 9 + 60) {
            vmt = m_VmtCommandList;
            localIndex = index - (44 + 19 + 9);
        } else if (index < 44 + 19 + 9 + 60 + 18) {
            vmt = m_VmtSwapChain;
            localIndex = index - (44 + 19 + 9 + 60);
        } else {
            return false;
        }
        return vmt != nullptr;
    }
    
    bool VmtHook::InstallHook(uint16_t index, void** originalFunc, void* hookFunc) {
        if (index >= 150 || !hookFunc) return false;
        
        void** vmt = nullptr;
        uint16_t local = 0;
        if (!LocateVmt(index, vmt, local)) return false;
        
        // 保存原始函数指针
        void* orig = vmt[local];
        if (originalFunc) *originalFunc = orig;
        m_OriginalFuncs[index] = orig;
        
        // 修改 vtable
        DWORD oldProt;
        if (!VirtualProtect(&vmt[local], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProt))
            return false;
        
        vmt[local] = hookFunc;
        VirtualProtect(&vmt[local], sizeof(void*), oldProt, &oldProt);
        return true;
    }
    
    void VmtHook::UninstallHook(uint16_t index) {
        if (index >= 150) return;
        
        void** vmt = nullptr;
        uint16_t local = 0;
        if (!LocateVmt(index, vmt, local)) return;
        
        void* orig = m_OriginalFuncs[index];
        if (!orig) return;
        
        // 恢复原始指针
        DWORD oldProt;
        if (!VirtualProtect(&vmt[local], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProt))
            return;
        
        vmt[local] = orig;
        VirtualProtect(&vmt[local], sizeof(void*), oldProt, &oldProt);
        m_OriginalFuncs[index] = nullptr;
    }
    
    void VmtHook::UninstallAll() {
        for (uint16_t i = 0; i < 150; ++i) {
            if (m_OriginalFuncs[i]) {
                UninstallHook(i);
            }
        }
    }
}
