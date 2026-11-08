# TPC: clangSema (SemaRC99) uses the TPC MC layer.
set(LEX_TESTS_TPC_LIBS)
set(LEX_TESTS_TPC_COMPONENTS)
if("TPC" IN_LIST LLVM_TARGETS_TO_BUILD)
  set(LEX_TESTS_TPC_LIBS LLVMTPCDesc)
  set(LEX_TESTS_TPC_COMPONENTS TPCDesc)
endif()

add_clang_unittest(LexTests
  DependencyDirectivesScannerTest.cpp
  HeaderMapTest.cpp
  HeaderSearchTest.cpp
  LexerTest.cpp
  LexHLSLRootSignatureTest.cpp
  ModuleDeclStateTest.cpp
  NoTrivialPPDirectiveTracerTest.cpp
  PPCallbacksTest.cpp
  PPConditionalDirectiveRecordTest.cpp
  PPDependencyDirectivesTest.cpp
  PPMemoryAllocationsTest.cpp
  CLANG_LIBS
  clangAST
  clangBasic
  clangLex
  clangParse
  clangSema
  ${LEX_TESTS_TPC_LIBS}
  LINK_LIBS
  LLVMTestingAnnotations
  LLVMTestingSupport
  LLVM_COMPONENTS
  Support
  ${LEX_TESTS_TPC_COMPONENTS}
  )
