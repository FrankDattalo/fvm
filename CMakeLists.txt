cmake_minimum_required(VERSION 3.10)
project(fvm)

set(CMAKE_CXX_STANDARD 17)

add_executable(fvm
        main.cpp
        Debug.cpp
        Debug.h
        ByteBuffer.cpp
        ByteBuffer.h
        Assert.h
        OpCode.cpp
        OpCode.h
        Assembler.cpp
        Assembler.h
        Panic.h
        VirtualMachine.cpp
        VirtualMachine.h
        FileIo.cpp
        FileIo.h
        Stack.h
        Object.h
        Writer.cpp
        Writer.h GarbageCollector.cpp GarbageCollector.h Heap.cpp Heap.h HeapObject.cpp HeapObject.h)
