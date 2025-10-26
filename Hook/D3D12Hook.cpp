#include "D3D12Hook.h"
#include <cstring>

namespace Hook {
    
    D3D12Hook::D3D12Hook() 
        : m_MethodsTable(nullptr)
        , m_VmtDevice(nullptr)
        , m_VmtCommandQueue(nullptr)
        , m_VmtCommandAllocator(nullptr)
        , m_VmtCommandList(nullptr)
        , m_VmtSwapChain(nullptr)
        , m_WindowHwnd(nullptr) {
        memset(&m_WindowClass, 0, sizeof(m_WindowClass));
    }
    
    D3D12Hook::~D3D12Hook() {
        if (m_MethodsTable) {
            free(m_MethodsTable);
            m_MethodsTable = nullptr;
        }
        DestroyTempWindow();
    }
    
    bool D3D12Hook::CreateTempWindow() {
        m_WindowClass.cbSize = sizeof(WNDCLASSEXW);
        m_WindowClass.style = CS_HREDRAW | CS_VREDRAW;
        m_WindowClass.lpfnWndProc = DefWindowProcW;
        m_WindowClass.hInstance = GetModuleHandleW(NULL);
        m_WindowClass.lpszClassName = L"D3D12HookTempWindow";
        
        if (!RegisterClassExW(&m_WindowClass)) return false;
        
        m_WindowHwnd = CreateWindowExW(0, m_WindowClass.lpszClassName, L"Temp", 
                                       WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, 
                                       NULL, NULL, m_WindowClass.hInstance, NULL);
        
        return m_WindowHwnd != nullptr;
    }
    
    void D3D12Hook::DestroyTempWindow() {
        if (m_WindowHwnd) {
            DestroyWindow(m_WindowHwnd);
            m_WindowHwnd = nullptr;
        }
        if (m_WindowClass.lpszClassName) {
            UnregisterClassW(m_WindowClass.lpszClassName, m_WindowClass.hInstance);
        }
    }
    
    bool D3D12Hook::Initialize() {
        // 创建临时窗口
        if (!CreateTempWindow()) return false;
        
        // 加载模块
        HMODULE d3d12Module = GetModuleHandleA("d3d12.dll");
        HMODULE dxgiModule = GetModuleHandleA("dxgi.dll");
        if (!d3d12Module || !dxgiModule) {
            DestroyTempWindow();
            return false;
        }
        
        // 获取 CreateDXGIFactory
        auto createDXGIFactory = (long(__stdcall*)(const IID&, void**))GetProcAddress(dxgiModule, "CreateDXGIFactory");
        if (!createDXGIFactory) {
            DestroyTempWindow();
            return false;
        }
        
        // 创建 DXGI Factory
        IDXGIFactory* factory = nullptr;
        if (FAILED(createDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory))) {
            DestroyTempWindow();
            return false;
        }
        
        // 枚举适配器
        IDXGIAdapter* adapter = nullptr;
        if (factory->EnumAdapters(0, &adapter) == DXGI_ERROR_NOT_FOUND) {
            factory->Release();
            DestroyTempWindow();
            return false;
        }
        
        // 获取 D3D12CreateDevice
        auto d3d12CreateDevice = (long(__stdcall*)(IUnknown*, D3D_FEATURE_LEVEL, const IID&, void**))
                                 GetProcAddress(d3d12Module, "D3D12CreateDevice");
        if (!d3d12CreateDevice) {
            adapter->Release();
            factory->Release();
            DestroyTempWindow();
            return false;
        }
        
        // 创建 D3D12 Device
        ID3D12Device* device = nullptr;
        if (FAILED(d3d12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&device))) {
            adapter->Release();
            factory->Release();
            DestroyTempWindow();
            return false;
        }
        
        // 创建 Command Queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Priority = 0;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 0;
        
        ID3D12CommandQueue* commandQueue = nullptr;
        if (FAILED(device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**)&commandQueue))) {
            device->Release();
            adapter->Release();
            factory->Release();
            DestroyTempWindow();
            return false;
        }
        
        // 创建 Command Allocator
        ID3D12CommandAllocator* commandAllocator = nullptr;
        if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
                                                   __uuidof(ID3D12CommandAllocator), (void**)&commandAllocator))) {
            commandQueue->Release();
            device->Release();
            adapter->Release();
            factory->Release();
            DestroyTempWindow();
            return false;
        }
        
        // 创建 Command List
        ID3D12GraphicsCommandList* commandList = nullptr;
        if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, NULL, 
                                              __uuidof(ID3D12GraphicsCommandList), (void**)&commandList))) {
            commandAllocator->Release();
            commandQueue->Release();
            device->Release();
            adapter->Release();
            factory->Release();
            DestroyTempWindow();
            return false;
        }
        
        // 创建 Swap Chain
        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        swapChainDesc.BufferDesc.Width = 100;
        swapChainDesc.BufferDesc.Height = 100;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.OutputWindow = m_WindowHwnd;
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        
        IDXGISwapChain* swapChain = nullptr;
        if (FAILED(factory->CreateSwapChain(commandQueue, &swapChainDesc, &swapChain))) {
            commandList->Release();
            commandAllocator->Release();
            commandQueue->Release();
            device->Release();
            adapter->Release();
            factory->Release();
            DestroyTempWindow();
            return false;
        }
        
        // 提取 vtable 指针
        m_VmtDevice = *(void***)device;
        m_VmtCommandQueue = *(void***)commandQueue;
        m_VmtCommandAllocator = *(void***)commandAllocator;
        m_VmtCommandList = *(void***)commandList;
        m_VmtSwapChain = *(void***)swapChain;
        
        // 复制方法表（用于索引换算）
        m_MethodsTable = (uintx_t*)calloc(150, sizeof(uintx_t));
        if (m_MethodsTable) {
            memcpy(m_MethodsTable, m_VmtDevice, 44 * sizeof(uintx_t));
            memcpy(m_MethodsTable + 44, m_VmtCommandQueue, 19 * sizeof(uintx_t));
            memcpy(m_MethodsTable + 44 + 19, m_VmtCommandAllocator, 9 * sizeof(uintx_t));
            memcpy(m_MethodsTable + 44 + 19 + 9, m_VmtCommandList, 60 * sizeof(uintx_t));
            memcpy(m_MethodsTable + 44 + 19 + 9 + 60, m_VmtSwapChain, 18 * sizeof(uintx_t));
        }
        
        // 释放临时对象
        swapChain->Release();
        commandList->Release();
        commandAllocator->Release();
        commandQueue->Release();
        device->Release();
        adapter->Release();
        factory->Release();
        DestroyTempWindow();
        
        return true;
    }
}
