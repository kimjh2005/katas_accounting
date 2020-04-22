project(kata1)

cmake_minimum_required(VERSION 3.5)

include(ExternalProject)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
set( EOSIO_CDT_REPO "$ENV{EOSIO_CDT_ROOT}" )
find_package( eosio.cdt PATHS ${EOSIO_CDT_REPO}/../modules )

# get git repo and build eosio.contracgts
ExternalProject_Add(
        eosio_contracts
        GIT_REPOSITORY "https://github.com/EOSIO/eosio.contracts.git"
        GIT_TAG "v1.9.1"
        SOURCE_DIR ${CMAKE_BINARY_DIR}/deps/eosio.contracts
        BINARY_DIR ${CMAKE_BINARY_DIR}/deps_build/eosio.contracts
        CMAKE_ARGS ""
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        TEST_COMMAND ""
        INSTALL_COMMAND ""
        BUILD_ALWAYS 1
)


add_subdirectory(src)
add_subdirectory(tests)

add_dependencies(accounting eosio_contracts)
add_dependencies(accounting_test accounting eosio_contracts)