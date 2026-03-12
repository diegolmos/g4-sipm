cmake_minimum_required(VERSION 3.16)
project(simple_g4sim_scint_sipm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Geant4
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# ROOT
find_package(ROOT REQUIRED CONFIG COMPONENTS Core RIO Tree)

add_executable(xyplane_sim
  main.cc
  src/ActionInitialization.cxx
  src/DetectorConstruction.cxx
  src/EventAction.cxx
  src/Materials.cxx
  src/PrimaryGeneratorAction.cxx
  src/RunAction.cxx
  src/SensitiveDetector.cxx
  src/SteppingAction.cxx
)

target_include_directories(xyplane_sim PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(xyplane_sim PRIVATE
  ${Geant4_LIBRARIES}
  ROOT::Core
  ROOT::RIO
  ROOT::Tree
)

# Copiar macros al build dir para poder hacer: /control/execute vis.mac
file(GLOB MACROS "${PROJECT_SOURCE_DIR}/macro/*.mac")
foreach(_m ${MACROS})
  configure_file(${_m} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()
