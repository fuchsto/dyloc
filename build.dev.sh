
BUILD_DIR=./build.dev

export DASH_BASE=${HOME}/opt/dash-0.4.0-dev
export DART_BASE=$DASH_BASE

if [ "${PAPI_BASE}" = "" ]; then
  PAPI_BASE=$PAPI_HOME
fi

# To use an existing installation of gtest instead of downloading the sources
# from the google test subversion repository, use:
#
#                    -DGTEST_LIBRARY_PATH=${HOME}/opt/gtest/lib \
#                    -DGTEST_INCLUDE_PATH=${HOME}/opt/gtest/include \
#


INSTALL_BASE="$(pwd)/$BUILD_DIR/install"

mkdir -p $BUILD_DIR
rm -Rf $BUILD_DIR/*

(cd $BUILD_DIR && \
  cmake -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_BASE/dyloc-0.1.0-dev \
        \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        \
        -DBUILD_TESTS=ON \
        \
        -DENABLE_LOGGING=ON \
        -DENABLE_PAPI=ON \
        \
        -DPAPI_PREFIX=${PAPI_BASE} \
        -DDART_PREFIX=${DART_BASE} \
        \
        -DGTEST_LIBRARY_PATH=${HOME}/opt/gtest/lib \
        -DGTEST_INCLUDE_PATH=${HOME}/opt/gtest/include \
  .. && \
  make install)

