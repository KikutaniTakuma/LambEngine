workspace "Project"
configurations
{
    "Debug",
    "Release",
}
platforms { "x64" }

project "Game"
    location      "Projects"
    targetdir ("./Bulid/%{prj.name}/%{cfg.buildcfg}")              -- 出力ディレクトリ
    objdir    ("./Build/%{prj.name}/%{cfg.buildcfg}/intermediate") -- 中間ディレクトリ
    language      "C++"
    cppdialect    "C++20"
    kind          "WindowedApp" -- .exe
    staticruntime "on"
    defines         { "x64" } -- プリプロセッサ
    disablewarnings { "4100", "4200" } -- C4100,C4200の警告を表示しないように
    files           { 
      "Projects/**.cpp", "Projects/**.h"
    }
    removefiles { 
      "Projects/Alfort/**.cpp","Projects/Alfort/**.h",
      "Projects/externals/**.cpp","Projects/externals/**.h"
    }
    links           { "Projects/Alfort/Alfort" }                  -- 外部プロジェクトとリンク
    includedirs { "$(ProjectDir)","$(ProjectDir)\\externals\\DirectXTK12\\Inc\\", "$(ProjectDir)\\Alfort\\" }

    -- 文字コードセット
    characterset "Unicode"

    -- 警告レベル4
    warnings "Extra"

    filter "configurations:Debug"                    -- 以下Debugの設定
      targetsuffix  "_debug"                             -- exeファイルのサフィックス(sample_d.exeのようになる)
      defines       { "_DEBUG", "DEBUG" }
      symbols       "On"                             -- シンボル情報作成
      optimize      "OFF"                            -- 最適化無し

    filter "configurations:Release"                  -- 以下Releaseの設定
    defines { "RELEASE", "NDEBUG" }
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



    project "Alfort"
    location      "Projects/Alfort"
    targetdir ("./Bulid/%{prj.name}/%{cfg.buildcfg}")              -- 出力ディレクトリ
    objdir    ("./Build/%{prj.name}/%{cfg.buildcfg}/intermediate") -- 中間ディレクトリ
    language      "C++"
    cppdialect    "C++20"
    kind          "StaticLib" -- .lib
    staticruntime "on"
    defines         { "x64" } -- プリプロセッサ
    disablewarnings { "4100", "4200" } -- C4100,C4200の警告を表示しないように
    files           { 
      "Projects/Alfort/**.cpp","Projects/Alfort/**.h"
    }
    removefiles { 
      "Projects/Alfort/**.cpp","Projects/Alfort/**.h",
      "Projects/externals/**.cpp","Projects/externals/**.h"
    }
    links           { 
      "./Projects/Alfort/StaticLibs/DirectXTK12/Release/DirectXTK12.lib",
      "./Projects/Alfort/StaticLibs/DirectXTex/Release/DirectXTex.lib"
   }                  -- 外部プロジェクトとリンク
   includedirs { "$(ProjectDir)", "$(SolutionDir)Projects\\externals\\DirectXTK12\\Inc\\", 
                  "$(SolutionDir)Projects\\externals\\imgui\\", "$(SolutionDir)Projects\\externals\\DirectXTex\\"
                }

    -- 文字コードセット
    characterset "Unicode"

    -- 警告レベル4
    warnings "Extra"

    filter "configurations:Debug"                    -- 以下Debugの設定
      targetsuffix  "_debug"                             -- exeファイルのサフィックス(sample_d.exeのようになる)
      defines       { "_DEBUG", "DEBUG" }
      symbols       "On"                             -- シンボル情報作成
      optimize      "OFF"                            -- 最適化無し

    filter "configurations:Release"                  -- 以下Releaseの設定
    defines { "RELEASE", "NDEBUG" }
    targetsuffix  "_release"
    optimize "Speed"                                  -- 最適化

          -- 最新のSDKを使用する
    filter "system:windows"
    systemversion "latest"

    filter { "platforms:x64" }
    architecture "x86_64"

    filter {}                                        -- フィルタのクリア