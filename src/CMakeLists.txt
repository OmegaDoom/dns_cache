set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dns_cache/dns_cache.cpp)
   
set(MAIN_SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    PARENT_SCOPE)

set(TEST_SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
    PARENT_SCOPE)
