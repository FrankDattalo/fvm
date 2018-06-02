cmake_minimum_required(VERSION 3.10)
project(fvm)

set(CMAKE_CXX_STANDARD 17)

add_executable(fvm main.cpp Logger.cpp Logger.h ByteBuffer.cpp ByteBuffer.h Assert.cpp Assert.h OpCode.cpp OpCode.h Assembler.cpp Assembler.h Panic.cpp Panic.h VirtualMachine.cpp VirtualMachine.h FileIo.cpp FileIo.h Stack.h)