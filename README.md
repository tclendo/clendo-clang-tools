# clendo-clang-tools

Installation:
- To install, you should have the llvm-project source repository downloaded and installed with clang and clang-tools-extra.
- Move this repository to the clang-tools-extra folder in the llvm-project repo.
- Somewhere in the clang-tools-extra CMakeLists.txt file, add the following line:
    add_subdirectory(clendo-clang-tools)
- From there you build llvm the same way you did above, and the executables should appear in the build/bin folder.
