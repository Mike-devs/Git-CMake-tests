@echo off
setlocal enabledelayedexpansion

if [%~1]==[] (
    set root_install_folder=%cd%) else (
    set root_install_folder=%~1
)

set PATH=%PATH%;C:\Program Files\CMake\bin;%APPDATA%\Python\Python311\Scripts
if [%CONAN_USER_HOME%]==[] (
    set CONAN_USER_HOME=%root_install_folder%\ThirdParties\install\dependencies
)
set cmake_prefix_path=%CONAN_USER_HOME%

REM Build and install all the dependencies
for %%a in (drogon poco mongo-c-driver mongo-cxx-driver glaze) do (
    if not exist %root_install_folder%\ThirdParties\build\%%a mkdir %root_install_folder%\ThirdParties\build\%%a
    pushd %root_install_folder%\ThirdParties\build\%%a

    if "%%a" == "drogon" (
        REM Install drogon dependencies with Conan package manager
        conan install %root_install_folder%\ThirdParties\src\drogon -s compiler="Visual Studio" -s compiler.version=16 -s build_type=Debug -g cmake_paths
        REM Removal of openssl (static/dynamic lib mismatch between Drogon and Poco, plus we won't use it, https is managed through a reverse proxy)
        conan remove -f openssl/*
        REM conan install %root_install_folder%\ThirdParties\src\drogon -s compiler="Visual Studio" -s compiler.version=16 -s build_type=Release -g cmake_paths
        set custom_cmake_flags=%root_install_folder%\ThirdParties\build\drogon\conan_paths.cmake
    )

    cmake.exe  %root_install_folder%\ThirdParties\src\%%a ^
                                            -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /FS /EHsc" -DCMAKE_C_FLAGS="/FS" ^
                                            -DCMAKE_INSTALL_PREFIX="%root_install_folder%\ThirdParties\install\%%a" ^
                                            -DCMAKE_PREFIX_PATH="%cmake_prefix_path%" -DCMAKE_BUILD_TYPE=Debug ^
                                            -DCMAKE_TOOLCHAIN_FILE=!custom_cmake_flags!
    cmake.exe --build . --target install
    popd

    REM Update the list of folders to retrieve CMake packages
    set cmake_prefix_path=!cmake_prefix_path!;%root_install_folder%\ThirdParties\install\%%a
)

REM Build our project
if not exist %root_install_folder%\cmake-build mkdir %root_install_folder%\cmake-build
pushd  %root_install_folder%\cmake-build
@echo on
echo %cmake_prefix_path%
cmake.exe .. -DCMAKE_PREFIX_PATH="%cmake_prefix_path%"
cmake.exe --build .
popd