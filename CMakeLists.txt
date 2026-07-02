cmake_minimum_required (VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# === TRICK: Natvrdo nastavený vcpkg toolchain (vlož to přesně SEM) ===
# set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project ("chess" CXX)

# Odsud dál pokračuje tvůj kód...
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

# 2. Definice tvého spustitelného programu
add_executable (chess 
    "source/main.cpp" 
    "source/app.cpp" "include/app.h"
    "include/fpsCounter.h" 
    "include/renderer.h" "source/renderer.cpp"
    "include/global.h" 
    "source/board.cpp" "include/board.h"
    "source/Vector2.cpp" "include/Vector2.h")

# 3. Prolinkování knihoven (vcpkg používá moderní jmenné prostory s dvojtečkou)
target_link_libraries(chess PRIVATE 
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)

# 4. Include adresář pro tvé vlastní hlavičkové soubory
target_include_directories(chess PUBLIC include)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET chess PROPERTY CXX_STANDARD 20)
endif()

add_custom_command(TARGET chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    "$<TARGET_FILE_DIR:chess>/resources"
)