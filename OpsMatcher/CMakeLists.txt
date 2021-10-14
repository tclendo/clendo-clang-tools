set(LLVM_LINK_COMPONENTS support)

add_clang_executable(ops-matcher
  OpsMatcher.cc
  )

target_link_libraries(ops-matcher
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  )
