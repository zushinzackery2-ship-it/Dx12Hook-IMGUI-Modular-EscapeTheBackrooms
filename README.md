# D3D12 Game Hook Framework

License: MIT (见仓库根目录 `LICENSE`)

**通用 D3D12 Hook 框架 + UE 游戏功能实现**

采用模块化设计的 DirectX 12 Hook 框架，Hook 层通用于所有 D3D12 游戏，游戏功能专为 Unreal Engine 游戏优化。

## 重要法律/风险声明（请在使用前阅读）

- 本项目仅供安全研究与学习使用。请勿将本项目用于在线或多人游戏的作弊、入侵或其他违法活动。作者对任何滥用行为导致的后果不承担责任。

## ✨ 特性

- 🎯 **通用 D3D12 Hook** - Hook 模块适用于所有 D3D12 游戏
- 🎮 **UE 游戏支持** - 使用 DUMPER-7 生成的 CppSDK，支持 UE4/UE5 游戏
- 🏗️ **模块化架构** - 代码清晰，职责分明
- 🎨 **ImGui UI** - 现代化游戏内菜单
- 🔧 **易于扩展** - 添加新功能只需修改 3 个文件

## 📁 项目结构（简要）

```
D3D12-Hook-ImGui/
├── Hook/              # Hook 模块 (通用 D3D12)
│   ├── VmtHook.cpp    # VMT 钩子实现
│   ├── D3D12Hook.cpp  # D3D12 初始化
│   └── HookManager.cpp # Hook 统一管理
├── Core/              # 核心模块 (UE 游戏功能)
│   ├── Config.h       # 配置管理
│   ├── GameData.cpp   # 游戏数据收集
│   └── GameMemory.h   # 内存读写
├── UI/                # 界面模块
│   └── Renderer.cpp   # ImGui 渲染
├── ImGui/             # ImGui 库
├── CppSDK/            # UE SDK (DUMPER-7 生成)
└── main.cpp           # 入口点
```

> 架构说明：
> - `Hook/` 层是**通用的**，可用于任何 D3D12 游戏
> - `Core/` 层实现了 UE 游戏特定功能
> - `CppSDK/` 由 [DUMPER-7](https://github.com/Encryqed/Dumper-7) 生成，包含游戏的类结构

## 🚀 快速开始

### 先决条件

- Windows 10 / Windows 11
- Visual Studio 2022（v143 工具集）和对应的 C++ 工作负载
- Windows SDK（建议安装最新版本）
- 具备 DirectX 12 支持的 GPU
- 推荐使用 Developer Command Prompt for VS 2022 来运行构建脚本

### 编译

方式 1（推荐）：在 Windows 中运行批处理脚本

```
# 在项目根目录的 Developer Command Prompt for VS 2022 中运行：
.\build_modular.bat
# 该脚本会自动配置并编译项目（包含 CppSDK），并输出 DLL（例如 `Etb_Esp.dll`）。
```

注意：`build_modular.bat` 里包含一些针对本地环境的硬编码项（例如 `vcvars64.bat` 的绝对路径、输出目录 `%OUTDIR%`、以及编译器选项 `/MD` 或 `/MT` 等）。在不同的机器上你通常需要根据本地 Visual Studio 安装路径和偏好修改脚本：

- 确认并修改 `call "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"` 为你本机的 vcvars64 路径，或在 Developer Command Prompt 中直接运行脚本以避免路径问题。
- 如需使用 MSBuild/VCXPROJ 的配置，请考虑将脚本改为调用：

```
msbuild d3d12imgui.sln /p:Configuration=Release /p:Platform=x64
```

- 注意命令行长度限制与编译并行性：脚本中以单条 `cl.exe` 编译大量源文件时可能遇到长度限制或速度问题，使用 `msbuild` 可更可靠并且使用项目文件中的设置（例如 CRT 选择 `/MT` vs `/MD`）。


方式 2：使用 Visual Studio 手动编译

```
# 打开 d3d12imgui.sln
# 选择 Release x64 配置
# 生成解决方案
```

注意：VS 解决方案未直接包含所有 `CppSDK/SDK/*.cpp` 文件（头文件过多可能影响编辑器性能）。若在 VS 中手动编译，需手动将 `CppSDK/SDK/*.cpp` 添加到项目中，或优先使用 `build_modular.bat`。

### 使用

1. 将生成的 `Etb_Esp.dll`（示例名）注入到目标游戏进程（注：注入与作弊行为存在法律/账号风险，请谨慎）。
2. 按 `INSERT` 键打开/关闭菜单
3. 快捷键示例：
   - `F1` - 物品透视
   - `F2` - 加速功能
   - `速度倍率` 滑条 - 调整 `APawn + 0x98 (CustomTimeDilation)`，通常支持 0.5x ~ 5.0x（视游戏与引擎版本有效性而定）

## 🔧 添加新功能

只需修改 3 个文件：

```cpp
// 1. Core/Config.h - 添加配置
namespace Features {
    inline bool NewFeature_Enabled = false;
}

// 2. Core/GameData.cpp - 添加逻辑
void DataCollector::CollectData() {
    if (Config::Features::NewFeature_Enabled) {
        // 你的逻辑
    }
}

// 3. UI/Renderer.cpp - 添加 UI
void Renderer::RenderMenu() {
    ImGui::Checkbox("新功能", &Config::Features::NewFeature_Enabled);
}
```

## 📋 系统要求

- Windows 10/11
- Visual Studio 2022 (v143)
- DirectX 12 支持
- 目标游戏：Unreal Engine 4/5 (D3D12)

## 🔄 适配其他游戏

### 适配其他 D3D12 游戏
Hook 层无需修改，只需：
1. 使用 [DUMPER-7](https://github.com/Encryqed/Dumper-7) 为目标游戏生成新的 CppSDK
2. 替换 `CppSDK/` 文件夹
3. 更新 `Core/Config.h` 中的偏移地址
4. 修改 `Core/GameData.cpp` 中的游戏逻辑

### 适配其他渲染 API
- **D3D11**: 修改 `Hook/D3D12Hook.cpp` → `D3D11Hook.cpp`
- **D3D9/OpenGL**: 需重写 Hook 模块

## 📝 技术栈

| 组件 | 技术 |
|------|------|
| Hook 技术 | VMT Hook (通用) |
| 渲染 API | DirectX 12 |
| UI 框架 | ImGui |
| 游戏引擎 | Unreal Engine 4/5 |
| SDK 生成 | DUMPER-7 |
| 编译器 | MSVC (C++20) |
| 架构 | 模块化设计 |

## 常见故障排查（快速）

- 找不到头文件或编译错误：请确认已安装 C++ 工作负载与 Windows SDK，并在 Developer Command Prompt 中运行 `build_modular.bat`。
- 链接错误：检查是否缺少 `CppSDK/SDK/*.cpp` 被添加到构建中（脚本通常会处理，手动编译时需留意）。
- 注入失败或被杀进程：某些安全软件或游戏反作弊会阻止注入，需在受控环境中测试。

## ⚠️ 免责声明

本项目仅供学习研究使用，请勿用于非法用途。使用本项目造成的任何后果由使用者自行承担。

## 贡献与许可证

- 本项目采用 MIT 许可证（见 `LICENSE`）。欢迎提交 issue 或 PR，但请确保用途合规。

---
更新时间：2025-10-26
