cmake_minimum_required(VERSION 3.10)
project(dragon-lox)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

set(LLVM_DEP_LIBS
        LLVMLTOLLVMPasses LLVMObjCARCOpts LLVMSymbolize LLVMDebugInfoPDB LLVMDebugInfoDWARF LLVMMIRParser LLVMFuzzMutate
        LLVMCoverage LLVMTableGen LLVMDlltoolDriver LLVMOrcJIT LLVMTestingSupport LLVMXCoreDisassembler LLVMXCoreCodeGen LLVMXCoreDesc
        LLVMXCoreInfo LLVMXCoreAsmPrinter LLVMSystemZDisassembler LLVMSystemZCodeGen LLVMSystemZAsmParser LLVMSystemZDesc
        LLVMSystemZInfo LLVMSystemZAsmPrinter LLVMSparcDisassembler LLVMSparcCodeGen LLVMSparcAsmParser LLVMSparcDesc
        LLVMSparcInfo LLVMSparcAsmPrinter LLVMPowerPCDisassembler LLVMPowerPCCodeGen LLVMPowerPCAsmParser
        LLVMPowerPCDesc LLVMPowerPCInfo LLVMPowerPCAsmPrinter LLVMNVPTXCodeGen LLVMNVPTXDesc LLVMNVPTXInfo
        LLVMNVPTXAsmPrinter LLVMMSP430CodeGen LLVMMSP430Desc LLVMMSP430Info LLVMMSP430AsmPrinter LLVMMipsDisassembler
        LLVMMipsCodeGen LLVMMipsAsmParser LLVMMipsDesc LLVMMipsInfo LLVMMipsAsmPrinter LLVMLanaiDisassembler
        LLVMLanaiCodeGen LLVMLanaiAsmParser LLVMLanaiDesc LLVMLanaiAsmPrinter LLVMLanaiInfo LLVMHexagonDisassembler
        LLVMHexagonCodeGen LLVMHexagonAsmParser LLVMHexagonDesc LLVMHexagonInfo LLVMBPFDisassembler LLVMBPFCodeGen
        LLVMBPFAsmParser LLVMBPFDesc LLVMBPFInfo LLVMBPFAsmPrinter LLVMARMDisassembler LLVMARMCodeGen LLVMARMAsmParser
        LLVMARMDesc LLVMARMInfo LLVMARMAsmPrinter LLVMARMUtils LLVMAMDGPUDisassembler LLVMAMDGPUCodeGen LLVMAMDGPUAsmParser
        LLVMAMDGPUDesc LLVMAMDGPUInfo LLVMAMDGPUAsmPrinter LLVMAMDGPUUtils LLVMAArch64Disassembler LLVMAArch64CodeGen
        LLVMAArch64AsmParser LLVMAArch64Desc LLVMAArch64Info LLVMAArch64AsmPrinter LLVMAArch64Utils LLVMObjectYAML
        LLVMLibDriver LLVMOption gtest_main gtest LLVMWindowsManifest LLVMX86Disassembler LLVMX86AsmParser LLVMX86CodeGen
        LLVMGlobalISel LLVMSelectionDAG LLVMAsmPrinter LLVMX86Desc LLVMMCDisassembler LLVMX86Info LLVMX86AsmPrinter
        LLVMX86Utils LLVMMCJIT LLVMLineEditor LLVMInterpreter LLVMExecutionEngine LLVMRuntimeDyld LLVMCodeGen LLVMTarget
        LLVMCoroutines LLVMipo LLVMInstrumentation LLVMVectorize LLVMScalarOpts LLVMLinker LLVMIRReader LLVMAsmParser
        LLVMInstCombine LLVMBitWriter LLVMAggressiveInstCombine LLVMTransformUtils LLVMAnalysis LLVMProfileData
        LLVMObject LLVMMCParser LLVMMC LLVMDebugInfoCodeView LLVMDebugInfoMSF LLVMBitReader LLVMCore LLVMBinaryFormat
        LLVMSupport LLVMDemangle
        )

find_package(LLVM REQUIRED)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

link_directories(${LLVM_LIBRARY_DIRS})
link_libraries(${LLVM_LIBS})

add_executable(dragon-lox
        include/expr.hpp
        include/lox.hpp
        include/scanner.hpp
        include/token.hpp
        src/cli.cpp
        src/lox.cpp
        src/scanner.cpp
        src/token.cpp
        include/visitor.h
        src/parser.cpp
        include/parser.h
        src/ast_printer.cpp
        include/ast_printer.hpp
        include/visitable_types.hpp
        include/runtime_err.hpp
        src/codegen.cpp
        include/codegen.hpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_EXE_LINKER_FLAGS ${LLVM_LD_FLAGS_STRING})
