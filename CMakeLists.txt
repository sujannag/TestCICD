cmake_minimum_required(VERSION 3.16)
project(helloworld LANGUAGES C CXX)

if(NOT TARGET CycloneDDS-CXX::ddscxx)
  find_package(CycloneDDS-CXX REQUIRED)
endif()

idlcxx_generate(TARGET dds_demo_data FILES dds_demo.idl WARNINGS no-implicit-extensibility)

add_executable(dds_demo_publisher publisher.cpp)
add_executable(dds_demo_subscriber subscriber.cpp)

# Link both executables to idl data type library and ddscxx.
target_link_libraries(dds_demo_publisher CycloneDDS-CXX::ddscxx dds_demo_data)
target_link_libraries(dds_demo_subscriber CycloneDDS-CXX::ddscxx dds_demo_data)

set_property(TARGET dds_demo_publisher PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
set_property(TARGET dds_demo_subscriber PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})