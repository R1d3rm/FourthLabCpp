cmake_minimum_required(VERSION 3.15)

# Версия проекта
set(PATCH_VERSION "0.0.1" CACHE INTERNAL "Patch version")  # или "1", но лучше семантическая версия
project(laba4 VERSION ${PATCH_VERSION})  # CMake сам установит PROJECT_VERSION

# Генерация version.h (опционально)
configure_file(version.h.in version.h)

# Настройки C++
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исходники
set(SRC_FILES
    main.cpp
)

# Исполняемый файл
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Установка
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_CONTACT "example@example.com")

include(CPack)