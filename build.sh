
BUILD_DIR=./build

export DASH_BASE=${HOME}/opt/dash-0.3.0
export DART_BASE=$DASH_BASE

if [ "${PAPI_BASE}" = "" ]; then
  PAPI_BASE=$PAPI_HOME
fi

mkdir -p $BUILD_DIR
rm -Rf $BUILD_DIR/*

(cd $BUILD_DIR && \
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$BUILD_DIR/install/ \
        \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF \
        \
        -DENABLE_LOGGING=OFF \
        \
        -DPAPI_PREFIX=${PAPI_BASE} \
        -DDART_PREFIX=${DART_BASE} \
  .. && \
  make install)

