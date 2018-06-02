
#include "Logger.h"
#include "ByteBuffer.h"
#include "Assembler.h"
#include "VirtualMachine.h"
#include "FileIo.h"
#include "Panic.h"

void usage() {
    panic_("Usage:\n"
           "  fvm interpret <path to fasm file>\n"
           "  fvm assemble <path to fasm file>\n"
           "  fvm run <path to fbim file>\n");
}

int main(int argc, char** argv) {

    if (argc != 3) {
        usage();
    }

    Logger::setDebug(false);

    if (!strcmp(argv[1], "interpret")) {

        std::string contents = FileIo::readFileToString(argv[2]);
        Logger::debug("main", contents);
        std::vector<uint8_t> buffer = Assembler::assemble(contents);
        VirtualMachine::interpret(buffer);

    } else if (!strcmp(argv[1], "assemble")) {

        std::string fileName(argv[2]);
        std::string contents = FileIo::readFileToString(fileName);
        Logger::debug("main", contents);
        std::vector<uint8_t> buffer = Assembler::assemble(contents);
        std::string newFileName = fileName + ".fbin";
        FileIo::writeFileContents(buffer, newFileName);

    } else if (!strcmp(argv[1], "run")) {

        std::vector<uint8_t> buffer = FileIo::readFileToVector(argv[2]);
        VirtualMachine::interpret(buffer);

    } else {
        usage();
    }

    return 0;
}