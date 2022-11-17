if [[ $# -eq 0 ]] ; then
    export root_install_folder=$(pwd)
else
    export root_install_folder=$1
fi

# Build and install all the dependencies
for folder in poco mongo-c-driver
do
    if [[ -v cmake_prefix_path ]] ; then
        export cmake_prefix_path="$cmake_prefix_path;$root_install_folder/ThirdParties/install/$folder"
    else
        export cmake_prefix_path="$root_install_folder/ThirdParties/install/$folder"
    fi

    if [ ! -d "$root_install_folder/ThirdParties/build/$folder" ]; then
        mkdir -p "$root_install_folder/ThirdParties/build/$folder"
    fi

    pushd "$root_install_folder/ThirdParties/build/$folder"
    cmake "$root_install_folder/ThirdParties/src/$folder" -DCMAKE_INSTALL_PREFIX="$root_install_folder/ThirdParties/install/$folder"
    cmake --build . --target install
    popd
done

# Build our project
if [ ! -d "$root_install_folder/cmake-build" ]; then
    mkdir -p "$root_install_folder/cmake-build"
fi

pushd  "$root_install_folder/cmake-build"
cmake .. -DCMAKE_PREFIX_PATH="$cmake_prefix_path"
cmake --build .
popd