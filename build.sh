if [[ $# -eq 0 ]] ; then
    export root_install_folder=$(pwd)
else
    export root_install_folder=$1
fi

# Build and install all the dependencies
for folder in poco mongo-c-driver mongo-cxx-driver
do
    if [ ! -d "$root_install_folder/ThirdParties/build/$folder" ]; then
        mkdir -p "$root_install_folder/ThirdParties/build/$folder"
    fi

    if [[ $folder ==  "mongo-c-driver" ]] ; then
        export SPECIFIC_PROJECT_FLAG="-DMONGOC_TEST_USE_CRYPT_SHARED=OFF"
    else
        export SPECIFIC_PROJECT_FLAG=
    fi

    pushd "$root_install_folder/ThirdParties/build/$folder"
    cmake   "$root_install_folder/ThirdParties/src/$folder" \
            -DCMAKE_CXX_STANDARD=17 \
            -DCMAKE_INSTALL_PREFIX="$root_install_folder/ThirdParties/install/$folder" \
            -DCMAKE_PREFIX_PATH="$cmake_prefix_path" \
            $SPECIFIC_PROJECT_FLAG
    cmake --build . --target install
    popd

    # Update the list of folders to retrieve CMake packages
    if [[ -v cmake_prefix_path ]] ; then
        export cmake_prefix_path="$cmake_prefix_path;$root_install_folder/ThirdParties/install/$folder"
    else
        export cmake_prefix_path="$root_install_folder/ThirdParties/install/$folder"
    fi
done

# Build our project
if [ ! -d "$root_install_folder/cmake-build" ]; then
    mkdir -p "$root_install_folder/cmake-build"
fi

pushd  "$root_install_folder/cmake-build"
cmake .. -DCMAKE_PREFIX_PATH="$cmake_prefix_path"
cmake --build .
popd