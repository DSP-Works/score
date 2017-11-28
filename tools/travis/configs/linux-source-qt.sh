#!/bin/sh
set +eux
export CC=/usr/bin/clang-4.0
export CXX=/usr/bin/clang++-4.0
export BOOST_ROOT=/opt/boost
export CMAKE_COMMON_FLAGS="-GNinja"
QT_ENV_SCRIPT=$(find /opt -name 'qt*-env.sh')
source $QT_ENV_SCRIPT

set -eux
