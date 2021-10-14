set(LLVM_LINK_COMPONENTS
  Support
  )

add_clang_executable(class-inheritance ClassInheritance.cc)

target_link_libraries(class-inheritance
  PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  )
