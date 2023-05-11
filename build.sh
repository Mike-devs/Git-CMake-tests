if [[ $# -eq 2 ]] ; then
    export root_install_folder=$2
    export build_type=$1
elif [[ $# -eq 1 ]] ; then
    export root_install_folder=$(pwd)
    if [[ "$1" == "Release" ]] ; then
        export build_type="Release"
    else
        export build_type="Debug"
    fi
elif [[ $# -eq 0 ]] ; then
    export root_install_folder=$(pwd)
    export build_type="Debug"
fi


# Build and install all the dependencies
for folder in drogon poco mongo-c-driver mongo-cxx-driver glaze
do
    if [ ! -d "$root_install_folder/ThirdParties/build/$build_type/$folder" ]; then
        mkdir -p "$root_install_folder/ThirdParties/build/$build_type/$folder"
    fi

    if [[ $folder ==  "mongo-c-driver" ]] ; then
        export SPECIFIC_PROJECT_FLAG="-DMONGOC_TEST_USE_CRYPT_SHARED=OFF"
    else
        export SPECIFIC_PROJECT_FLAG=
    fi

    pushd "$root_install_folder/ThirdParties/build/$build_type/$folder"
    cmake   "$root_install_folder/ThirdParties/src/$folder" \
            -DCMAKE_CXX_STANDARD=20 \
            -DCMAKE_INSTALL_PREFIX="$root_install_folder/ThirdParties/install/$build_type/$folder" \
            -DCMAKE_PREFIX_PATH="$cmake_prefix_path" -DCMAKE_BUILD_TYPE="$build_type" \
            $SPECIFIC_PROJECT_FLAG
    cmake --build . --config $build_type --target install
    popd

    # Update the list of folders to retrieve CMake packages
    if [[ -v cmake_prefix_path ]] ; then
        export cmake_prefix_path="$cmake_prefix_path;$root_install_folder/ThirdParties/install/$build_type/$folder"
    else
        export cmake_prefix_path="$root_install_folder/ThirdParties/install/$build_type/$folder"
    fi
done

# Build our project
if [ ! -d "$root_install_folder/cmake-build" ]; then
    mkdir -p "$root_install_folder/cmake-build"
fi

pushd  "$root_install_folder/cmake-build"
cmake .. -DCMAKE_PREFIX_PATH="$cmake_prefix_path" -DCMAKE_BUILD_TYPE="$build_type"
cmake --build . --config $build_type
popd