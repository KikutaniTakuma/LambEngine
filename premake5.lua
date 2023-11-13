workspace "PreMakeTest"
configurations
{
    "Debug",
    "Release",
}
platforms { "x64" }

project "Test"
    location      "project"
    targetdir ("bin/%{prj.name}/%{cfg.buildcfg}")              -- 出力ディレクトリ
    objdir    ("bin/%{prj.name}/%{cfg.buildcfg}/intermediate") -- 中間ディレクトリ
    language      "C++"
    cppdialect    "C++20"
    kind          "ConsoleApp" -- .exe
    staticruntime "on"
    defines         { "x64" } -- プリプロセッサ
    disablewarnings { "4100", "4200" } -- C4100,C4200の警告を表示しないように
    files           { "**.h", "**.cpp", "**.hlsl", "**.hlsli" }
    --includedirs     { "include" }                    -- 追加のインクルードディレクトリ

    --libdirs       { "lib" }                        -- 追加のライブラリディレクトリ(必要であれば)
    --links         { "hoge.lib" }                   -- 追加の依存ファイル(必要であれば)
    --links           { "sampleLib" }                  -- 外部プロジェクトとリンク

    -- 警告レベル最大
    warnings "Extra"

    filter "configurations:Debug"                    -- 以下Debugの設定
      --targetsuffix  "_d"                             -- exeファイルのサフィックス(sample_d.exeのようになる)
      defines       { "DEBUG" }
      symbols       "On"                             -- シンボル情報作成
      optimize      "OFF"                            -- 最適化無し

    filter "configurations:Release"                  -- 以下Releaseの設定
    defines { "NDEBUG" }
    optimize "Full"                                  -- 最適化

          -- 最新のSDKを使用する
    filter "system:windows"
    systemversion "latest"
    -- ビルド後イベント
    postbuildcommands { 'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"' }

    filter { "platforms:x64" }
    architecture "x86_64"

    filter {}                                        -- フィルタのクリア