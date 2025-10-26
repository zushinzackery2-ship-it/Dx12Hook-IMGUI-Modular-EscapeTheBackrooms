#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "../CppSDK/SDK/Engine_classes.hpp"

namespace GameData {
    // Actor信息结构
    struct ActorInfo {
        SDK::FVector Location;
        SDK::FVector2D ScreenLocation;
        std::string Name;
        std::string ClassName;
        std::string DisplayName;  // 简化的显示名称
        bool IsVisible;
        int Index;
        float Distance;  // 与玩家的距离（单位：厘米）
        
        ActorInfo() : Location(0,0,0), ScreenLocation(0,0), IsVisible(false), Index(-1), Distance(0.0f) {}
    };
    
    // 游戏数据类
    class DataCollector {
    public:
        DataCollector();
        ~DataCollector();
        
        void StartCollection();  // 启动数据收集线程
        void StopCollection();   // 停止数据收集
        
        // 获取数据（线程安全）
        std::vector<ActorInfo> GetActors();
        int GetActorCount();
        
    private:
        static DWORD WINAPI CollectionThread(LPVOID param);
        void CollectData();  // 实际数据收集逻辑
        
        bool m_Running;
        HANDLE m_Thread;
        CRITICAL_SECTION m_CriticalSection;
        std::vector<ActorInfo> m_Actors;
        int m_ActorCount;
        HMODULE m_GameBase;
        float m_OriginalSpeed;  // 缓存原始速度
    };
}
