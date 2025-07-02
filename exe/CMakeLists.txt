# Set the exe file set
set(TEST_EXE
    xRayGeant4
)
# for now put the executables in the build root for compatibility with existing scripts
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# Line exe files to their include files
foreach(exe ${TEST_EXE})
    add_executable(${exe} ${exe}.cpp)
    foreach(HEADER_PATH ${HEADERS})
        get_filename_component(DIR ${HEADER_PATH} DIRECTORY)
        target_include_directories(${exe} PRIVATE ${CMAKE_SOURCE_DIR}/${DIR})
    endforeach()
    # target_link_libraries(${exe} myProject ROOT::Core ROOT::Gpad ROOT::Hist ROOT::Tree)
    target_link_libraries(${exe} myProject ${Geant4_LIBRARIES} ROOT::Core ROOT::Gpad ROOT::Hist ROOT::Tree)
    target_link_libraries(${exe} ${OpenCV_LIBS})
    set_target_properties(${exe} PROPERTIES OUTPUT_NAME ${exe}.out)
endforeach()
