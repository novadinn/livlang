pushd "$(dirname "$0")"

cmake .
make -j8

popd