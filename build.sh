
BUILD_DIR=./build

export DASH_BASE=${HOME}/opt/dash-0.3.0
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


mkdir -p $BUILD_DIR
rm -Rf $BUILD_DIR/*

(cd $BUILD_DIR && \
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$BUILD_DIR/install/ \
        \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF \
        \
        -DENABLE_LOGGING=OFF \
        -DHWLOC_PREFIX=${HWLOC_BASE} \
        -DENABLE_PAPI=OFF \
        \
        -DPAPI_PREFIX=${PAPI_BASE} \
        -DDART_PREFIX=${DART_BASE} \
        \
        -DGTEST_LIBRARY_PATH=${HOME}/opt/gtest/lib \
        -DGTEST_INCLUDE_PATH=${HOME}/opt/gtest/include \
  .. && \
  make install)

