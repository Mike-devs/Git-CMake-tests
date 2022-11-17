if [[ $# -eq 0 ]] ; then
    export root_install_folder=$(pwd)
else
    export root_install_folder=$1
fi

# Build and install Poco
if [ ! -d "$root_install_folder/ThirdParties/build/poco" ]; then
    mkdir -p "$root_install_folder/ThirdParties/build/poco"
fi

pushd "$root_install_folder/ThirdParties/build/poco"
cmake "$root_install_folder/ThirdParties/src/poco" -DCMAKE_INSTALL_PREFIX="$root_install_folder/ThirdParties/install/poco"
cmake --build . --target install
popd

# Build and install mongo-c-driver
if [ ! -d "$root_install_folder/ThirdParties/build/mongo-c-driver" ]; then
    mkdir -p "$root_install_folder/ThirdParties/build/mongo-c-driver"
fi

pushd "$root_install_folder/ThirdParties/build/mongo-c-driver"
cmake "$root_install_folder/ThirdParties/src/mongo-c-driver" -DCMAKE_INSTALL_PREFIX="$root_install_folder/ThirdParties/install/mongo-c-driver"
cmake --build . --target install
popd

# Build our project
if [ ! -d "$root_install_folder/cmake-build" ]; then
    mkdir -p "$root_install_folder/cmake-build"
fi

pushd  "$root_install_folder/cmake-build"
cmake .. -DCMAKE_PREFIX_PATH="$root_install_folder/ThirdParties/install/poco;$root_install_folder/ThirdParties/install/mongo-c-driver"
cmake --build .
popd