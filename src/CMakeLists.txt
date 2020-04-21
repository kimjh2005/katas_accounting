cmake_minimum_required(VERSION 3.5)

project(accounting)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( accounting accounting accounting.cpp )
add_contract( notifyhook notifyhook notifyhook.cpp )

message(STATUS "${PROJECT_SOURCE_DIR}")

target_include_directories( ${PROJECT_NAME} 
	PUBLIC 
	${CMAKE_SOURCE_DIR}/../include
	${accounting_SOURCE_DIR}/../external/eosio.contracts/contracts/eosio.token/include
	
)
#target_ricardian_directory( accounting ${CMAKE_SOURCE_DIR}/../ricardian )
