#include <iostream>

#include "scanner/DirScanner.h"

int main(int argc,char* argv[]) {
    const std::string path = argv[1];
    std::cout << ScanDirectory(path);

    return 0;
}
