# Examples
add_executable(run_vel run_vel.cpp)
add_executable(read_enc read_enc.cpp)
add_executable(move_gimbal move_gimbal.cpp)

# Link against the main library
target_link_libraries(run_vel robomaster_comm)
target_link_libraries(read_enc robomaster_comm)
target_link_libraries(move_gimbal robomaster_comm)

# Compiler options
target_compile_options(run_vel PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(read_enc PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(move_gimbal PRIVATE -Wall -Wextra -Wpedantic)
