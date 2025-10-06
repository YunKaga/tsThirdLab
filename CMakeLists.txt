cmake_minimum_required(VERSION 3.15) # минимальная версия cmake

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION ${PATCH_VERSION})

project(lb3 VERSION ${PROJECT_VESRION})  # название проекта

configure_file(version.h.in version.h)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_FILES # переменная с именами исходников
	main.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES}) # название итогого приложения и из чего его собирать
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
