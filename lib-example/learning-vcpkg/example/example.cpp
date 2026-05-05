#include <learning-vcpkg/example/example.hpp>

#include <fmt/color.h>

void learning_vcpkg::example::helloWorld() {
    fmt::print(fg(fmt::color::forest_green), "Hello World!\n");
}
