#pragma once
#include "../ImGui/imgui.h"
#include "../Core/GameData.h"

namespace UI {
    // UI渲染器
    class Renderer {
    public:
        static void Initialize();      // 初始化ImGui样式
        static void RenderMenu(GameData::DataCollector* dataCollector);  // 渲染菜单
        static void RenderESP(GameData::DataCollector* dataCollector);   // 渲染ESP
        
    private:
        static void SetupStyle();      // 设置霓虹风格
    };
}
