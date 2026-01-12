add_executable(blockyarchiver
    main.cpp

    src/noname24/blockyarchiver.cpp
)
target_include_directories(blockyarchiver PUBLIC
    include
)
target_link_libraries(blockyarchiver PRIVATE
    blockybinary
)
