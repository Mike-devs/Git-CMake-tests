@echo off
setlocal enabledelayedexpansion

if [%~1]==[] (
    set root_install_folder=%cd%) else (
    set root_install_folder=%~1
)

REM Build and install all the dependencies
for %%a in (poco mongo-c-driver) do (
    if not exist %root_install_folder%\ThirdParties\build\%%a mkdir %root_install_folder%\ThirdParties\build\%%a
    pushd %root_install_folder%\ThirdParties\build\%%a
    if [!cmake_prefix_path!]==[] (
        set cmake_prefix_path=%root_install_folder%\ThirdParties\install\%%a) else (
        set cmake_prefix_path=!cmake_prefix_path!;%root_install_folder%\ThirdParties\install\%%a
    )
    "C:\Program Files\CMake\bin\cmake.exe" %root_install_folder%\ThirdParties\src\%%a -DCMAKE_CXX_FLAGS="/FS /EHsc" -DCMAKE_C_FLAGS="/FS" -DCMAKE_INSTALL_PREFIX="%root_install_folder%\ThirdParties\install\%%a"
    "C:\Program Files\CMake\bin\cmake.exe" --build . --target install
    popd
)

REM Build our project
if not exist %root_install_folder%\cmake-build mkdir %root_install_folder%\cmake-build
pushd  %root_install_folder%\cmake-build
"C:\Program Files\CMake\bin\cmake.exe" .. -DCMAKE_PREFIX_PATH="!cmake_prefix_path!"
"C:\Program Files\CMake\bin\cmake.exe" --build .
popd