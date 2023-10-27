add_library(StbImage INTERFACE)
add_library(Evie::StbImage ALIAS StbImage)
target_include_directories(StbImage SYSTEM INTERFACE include)