cmake_minimum_required(VERSION 3.5)

project(accounting)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

include(${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake)

add_contract( accounting accounting accounting.cpp )
add_contract( notifyhook notifyhook notifyhook.cpp )

message(STATUS "EOSIO_CDT_ROOT = ${EOSIO_CDT_ROOT}")

target_include_directories( ${PROJECT_NAME} 
	PUBLIC 
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_BINARY_DIR}/deps/eosio.contracts/contracts/eosio.token/include
)

set_target_properties ( accounting
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/accounting"
)


set_target_properties ( notifyhook
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/notifyhook"
)

#target_ricardian_directory( accounting ${CMAKE_SOURCE_DIR}/../ricardian )
