
#include "Debug.h"
#include "ByteBuffer.h"
#include "Assembler.h"
#include "VirtualMachine.h"
#include "FileIo.h"
#include "Panic.h"
#include "HeapObject.h"

void usage() {
    RUNTIME_ASSERT(false, "Usage:\n"
           "  fvm interpret <path to assembly file>\n"
           "  fvm assemble <path to assembly file>\n"
           "  fvm run <path to compiled file>\n");
}

int main(int argc, char** argv) {

    if (argc != 3) {
        usage();
    }

    std::string arg1(argv[1]);
    std::string arg2(argv[2]);

    if (arg1 == "interpret") {

        std::string contents;
        FileIo::readFileToString(arg2, contents);
        DEBUG("main", contents);
        std::vector<uint8_t> buffer;
        Assembler::assemble(contents, buffer);
        Writer writer(std::cout);
        VirtualMachine::interpret(buffer, writer);

    } else if (arg1 == "assemble") {

        std::string contents;
        FileIo::readFileToString(arg2, contents);
        DEBUG("main", contents);
        std::vector<uint8_t> buffer;
        Assembler::assemble(contents, buffer);
        std::string newFileName = arg2 + ".fbin";
        FileIo::writeFileContents(buffer, newFileName);

    } else if (arg1 == "run") {

        std::vector<uint8_t> buffer;
        FileIo::readFileToVector(arg2, buffer);
        Writer writer(std::cout);
        VirtualMachine::interpret(buffer, writer);

    } else {
        usage();
    }

    return 0;
}