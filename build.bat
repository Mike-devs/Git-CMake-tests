@echo off
setlocal enabledelayedexpansion

REM Process install folder (or default one)
if [%~2]==[] (
    set root_install_folder=%cd%
) else (
    set root_install_folder=%~2
) 
REM Process Debug or Release compilation (or Debug by default)
if [%~1]==[Release] (
    set build_type=Release
) else (
    set build_type=Debug
)

set PATH=%PATH%;C:\Program Files\CMake\bin;%APPDATA%\Python\Python311\Scripts
if [%CONAN_USER_HOME%]==[] (
    set CONAN_USER_HOME=%root_install_folder%\ThirdParties\install\%build_type%\dependencies
)
set cmake_prefix_path=%CONAN_USER_HOME%

REM Build and install all the dependencies
for %%a in (drogon poco mongo-c-driver mongo-cxx-driver glaze) do (
    if not exist %root_install_folder%\ThirdParties\build\%build_type%\%%a mkdir %root_install_folder%\ThirdParties\build\%build_type%\%%a
    pushd %root_install_folder%\ThirdParties\build\%build_type%\%%a

    if "%%a" == "drogon" (
        REM Install drogon dependencies with Conan package manager
        conan install %root_install_folder%\ThirdParties\src\drogon -s compiler="Visual Studio" -s compiler.version=16 -s build_type=%build_type% -g cmake_paths
        REM Removal of openssl (static/dynamic lib mismatch between Drogon and Poco, plus we won't use it, https is managed through a reverse proxy)
        conan remove -f openssl/*
        set custom_cmake_flags=%root_install_folder%\ThirdParties\build\%build_type%\drogon\conan_paths.cmake
    )

    cmake.exe  %root_install_folder%\ThirdParties\src\%%a ^
                                            -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /FS /EHsc" -DCMAKE_C_FLAGS="/FS" ^
                                            -DCMAKE_INSTALL_PREFIX="%root_install_folder%\ThirdParties\install\%build_type%\%%a" ^
                                            -DCMAKE_PREFIX_PATH="%cmake_prefix_path%" -DCMAKE_BUILD_TYPE="%build_type%" ^
                                            -DCMAKE_TOOLCHAIN_FILE=!custom_cmake_flags!
    cmake.exe --build . --config %build_type% --target install
    popd

    REM Update the list of folders to retrieve CMake packages
    set cmake_prefix_path=!cmake_prefix_path!;%root_install_folder%\ThirdParties\install\%build_type%\%%a
)

REM Build our project
rmdir %root_install_folder%\cmake-build
mkdir %root_install_folder%\cmake-build
pushd  %root_install_folder%\cmake-build
cmake.exe .. -DCMAKE_PREFIX_PATH="%cmake_prefix_path%" -DCMAKE_BUILD_TYPE=%build_type% -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /EHsc"
cmake.exe --build . --config %build_type%
popd