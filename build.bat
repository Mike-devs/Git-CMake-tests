@echo off
if [%~1]==[] (
    set root_install_folder=%cd%) else (
    set root_install_folder=%~1
)

REM /FS is needed otherwise pdb generation file can fail when run from command line
REM /EHsc is needed to suppress tons of warnings when /FS is used

REM Build and install Poco
if not exist %root_install_folder%\ThirdParties\build\poco mkdir %root_install_folder%\ThirdParties\build\poco
pushd %root_install_folder%\ThirdParties\build\poco
"C:\Program Files\CMake\bin\cmake.exe" %root_install_folder%\ThirdParties\src\poco -DCMAKE_CXX_FLAGS="/FS /EHsc" -DCMAKE_C_FLAGS="/FS" -DCMAKE_INSTALL_PREFIX="%root_install_folder%\ThirdParties\install\poco"
"C:\Program Files\CMake\bin\cmake.exe" --build . --target install
popd

REM Build and install mongo-c-driver
if not exist %root_install_folder%\ThirdParties\build\mongo-c-driver mkdir %root_install_folder%\ThirdParties\build\mongo-c-driver
pushd %root_install_folder%\ThirdParties\build\mongo-c-driver
"C:\Program Files\CMake\bin\cmake.exe" %root_install_folder%\ThirdParties\src\mongo-c-driver -DCMAKE_CXX_FLAGS="/FS /EHsc" -DCMAKE_C_FLAGS="/FS" -DCMAKE_INSTALL_PREFIX="%root_install_folder%\ThirdParties\install\mongo-c-driver"
"C:\Program Files\CMake\bin\cmake.exe" --build . --target install
popd

REM Build our project
if not exist %root_install_folder%\cmake-build mkdir %root_install_folder%\cmake-build
pushd  %root_install_folder%\cmake-build
"C:\Program Files\CMake\bin\cmake.exe" .. -DCMAKE_PREFIX_PATH="%root_install_folder%\ThirdParties\install\poco;%root_install_folder%\ThirdParties\install\mongo-c-driver"
"C:\Program Files\CMake\bin\cmake.exe" --build .
popd