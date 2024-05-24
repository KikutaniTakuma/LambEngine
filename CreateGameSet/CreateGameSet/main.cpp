#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include <functional>

#include <Windows.h>
#undef max
#undef min

std::filesystem::path NormalizePath(const std::filesystem::path& fullPath, const std::filesystem::path& parentDir) {
    // 親ディレクトリがフルパスの先頭にあるかを確認
    if (fullPath.string().find(parentDir.string()) == 0) {
        return std::filesystem::relative(fullPath, parentDir);
    }
    else {
        throw std::runtime_error("Specified parent directory is not a parent of the full path");
    }
}

void CopyFilesWithStructure(const std::filesystem::path& sourceDir, const std::filesystem::path& destDir, const std::function<bool(const std::filesystem::path&)>& filter) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(sourceDir)) {
        if (filter(entry.path())) {
            // 元のディレクトリ構造を保つための相対パスを作成
            std::filesystem::path relativePath = std::filesystem::relative(entry.path(), sourceDir);
            std::filesystem::path destPath = destDir / relativePath;

            // 必要なディレクトリを作成
            std::filesystem::create_directories(destPath.parent_path());

            // ファイルをコピー
            std::filesystem::copy(entry.path(), destPath, std::filesystem::copy_options::overwrite_existing);
        }
    }
}

int main() {
    try {
        std::ifstream file;
        file.open("./Projects/Game/Log/LoadResrouce.log");

        std::vector<std::filesystem::path> data;
        std::string line;
        while (std::getline(file, line)) {
            data.push_back(line);
        }


        if (std::filesystem::exists("./Game/")) {
            std::filesystem::create_directories("./Game/");
        }

        CopyFilesWithStructure("./Projects/Game/Resources/", "./Game/Resources/", [&data](const std::filesystem::path& path){
            auto&& tmp = NormalizePath(path, "./Projects/Game/Resources/");
            for (auto& i : data) {
                if (i == tmp) {
                    return true;
                }
            }

            return false;
            }
        );
        CopyFilesWithStructure("./Projects/Game/Resources/Datas", "./Game/Resources/Datas", [](const std::filesystem::path& path) {
            return path.has_extension();
            }
        );

    }
    catch (const std::exception& err) {
        std::cout << err.what();
        return -1;
    }

    return 0;
}