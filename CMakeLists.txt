cmake_minimum_required(VERSION 3.5)

project(easy-stun)

add_executable(${PROJECT_NAME}  src/es_msg.c
                                src/es_node.c
                                src/es_node_local.c
                                src/es_node_local_cr.c
                                src/es_node_local_loop.c
                                src/es_node_remote.c
                                src/es_params.c
                                src/main.c)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DDBG_LEVEL=4)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto OpenSSL::SSL)

install(TARGETS ${PROJECT_NAME}
        RUNTIME
        DESTINATION bin
        COMPONENT applications)

