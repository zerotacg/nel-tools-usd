#include <learning-vcpkg/example/example.hpp>

#include <fmt/color.h>

using namespace learning_vcpkg;

int main() {
    fmt::print(fmt::emphasis::bold, "learning-vcpkg: ");
    example::helloWorld();
    return 0;
}
