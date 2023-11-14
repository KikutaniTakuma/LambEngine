workspace "Alfort"
configurations
{
    "Debug",
    "Release",
}
platforms { "x64" }


project "Alfort"
    location      "Projects/Alfort"
    targetdir ("./Build/%{prj.name}/%{cfg.buildcfg}")              -- 出力ディレクトリ
    objdir    ("./Build/%{prj.name}/%{cfg.buildcfg}/intermediate") -- 中間ディレクトリ
    language      "C++"
    cppdialect    "C++20"
    kind          "StaticLib" -- .lib
    staticruntime "on"
    defines         { "x64" } -- プリプロセッサ
    disablewarnings { "4100", "4200" } -- C4100,C4200の警告を表示しないように
    files           { 
      "Projects/Alfort/**.cpp","Projects/Alfort/**.h",
      "Projects/externals/**.cpp","Projects/externals/**.h"
    }
    removefiles { 
      "Projects/externals/DirectXTex/**.cpp","Projects/externals/DirectXTex/**.h",
      "Projects/externals/DirectXTK12/**.cpp","Projects/externals/DirectXTK12/**.h",
      "Projects/externals/nlohmann/**.cpp","Projects/externals/nlohmann/**.h"
    }
    links           { 
      "$(ProjectDir)StaticLibs\\DirectXTex\\Release\\DirectXTex.lib",
      "$(ProjectDir)StaticLibs\\DirectXTK12\\Release\\DirectXTK12.lib"
   }                  -- 外部プロジェクトとリンク
   includedirs { "$(ProjectDir)", "$(SolutionDir)Projects\\externals\\DirectXTK12\\Inc\\", 
                  "$(SolutionDir)Projects\\externals\\imgui\\", "$(SolutionDir)Projects\\externals\\DirectXTex\\"
                }

    -- 文字コードセット
    characterset "Unicode"

    -- 警告レベル4
    warnings "Extra"

    filter "configurations:Debug"                    -- 以下Debugの設定
      defines       { "_DEBUG", "DEBUG" }
      symbols       "On"                             -- シンボル情報作成
      optimize      "OFF"                            -- 最適化無し

    filter "configurations:Release"                  -- 以下Releaseの設定
    defines { "RELEASE", "NDEBUG" }
    optimize "Speed"                                  -- 最適化

          -- 最新のSDKを使用する
    filter "system:windows"
    systemversion "latest"

    filter { "platforms:x64" }
    architecture "x86_64"

    filter "action:vs*"
        buildoptions { "/MP" } -- /MPフラグを有効にする

    filter {}                                        -- フィルタのクリア
    buildoptions { "/utf-8" }



project "Game"
    location      "Projects"
    targetdir ("./Build/%{prj.name}/%{cfg.buildcfg}")              -- 出力ディレクトリ
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
    links           { "Projects\\Alfort\\Alfort" }                  -- 外部プロジェクトとリンク
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
    postbuildcommands { 'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"','copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"' }

    filter { "platforms:x64" }
    architecture "x86_64"

    filter "action:vs*"
        buildoptions { "/MP" } -- /MPフラグを有効にする

    filter {}                                        -- フィルタのクリア
    buildoptions { "/utf-8" }