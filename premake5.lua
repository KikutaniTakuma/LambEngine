workspace "Project"
configurations
{
    "Debug",
    "Release",
}
platforms { "x64" }

project "Game"
    location      "projects"
    targetdir (".../Bulid/%{prj.name}/%{cfg.buildcfg}")              -- 出力ディレクトリ
    objdir    (".../Build/%{prj.name}/%{cfg.buildcfg}/intermediate") -- 中間ディレクトリ
    language      "C++"
    cppdialect    "C++20"
    kind          "ConsoleApp" -- .exe
    staticruntime "on"
    defines         { "x64" } -- プリプロセッサ
    disablewarnings { "4100", "4200" } -- C4100,C4200の警告を表示しないように
    files           { 
      "main.cpp",
      "Projects/GlobalVariables/*.h", "Projects/GlobalVariables/*.cpp",
      "Projects/SceneManager/*.h", "Projects/SceneManager/*.cpp",
      "Projects/SceneManager/SubMenu/*.h", "Projects/SceneManager/SubMenu/*.cpp",
      "Projects/SceneManager/Fade/*.h", "Projects/SceneManager/Fade/*.cpp",
      "Projects/SceneManager/GameScene/*.h", "Projects/SceneManager/GameScene/*.cpp",
    }
    links           { "Alfort\\Alfort" }                  -- 外部プロジェクトとリンク

    characterset ("Unicode")

    -- 警告レベル4
    warnings "Extra"

    filter "configurations:Debug"                    -- 以下Debugの設定
      targetsuffix  "_debug"                             -- exeファイルのサフィックス(sample_d.exeのようになる)
      defines       { "_DEBUG", "_CONSOLE",'%(PreprocessorDefinitions)' }
      symbols       "On"                             -- シンボル情報作成
      optimize      "OFF"                            -- 最適化無し

    filter "configurations:Release"                  -- 以下Releaseの設定
    defines { "RELEASE" }
    targetsuffix  "_release"
    optimize "Speed"                                  -- 最適化

          -- 最新のSDKを使用する
    filter "system:windows"
    systemversion "latest"
    -- ビルド後イベント
    postbuildcommands { 'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"' }
    posrbuildcommands { 'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"' }

    filter { "platforms:x64" }
    architecture "x86_64"

    filter {}                                        -- フィルタのクリア