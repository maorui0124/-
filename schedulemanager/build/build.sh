#!/bin/bash

# 脚本假设它已经位于build目录中

# 获取脚本文件名，以防止删除自身
script_name=$(basename "$0")

# 删除除data文件夹及其内容和脚本文件以外的所有文件和目录
# 使用 -prune 选项排除data目录及其所有子目录和文件
find . -mindepth 1 -type d -name 'data' -prune -o ! -name "$script_name" -exec rm -rf {} +

echo "Cleanup completed, except for the script itself and data folder."

# 编译项目，假设使用CMake和Make
# 确保CMakeLists.txt位于合适的目录中
echo "Starting build process..."
if cmake .. && make; then
    echo "Build succeeded."
else
    echo "Build failed."
    exit 1
fi

# 运行编译好的应用，假设可执行文件名为 MyApp
echo "Running application..."
# ./schedule_manager run

echo "Script completed."
