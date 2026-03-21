//
// Created by IliyaD on 26.02.2026.
//

#ifndef ZIPBIKE_FILEHANDLER_H
#define ZIPBIKE_FILEHANDLER_H

#include <string>
#include "container.h"

class filehandler {
public:

    filehandler();
    bool save(container& c, std::string filePath);

    container load(std::string filePath);
    bool fileExists(std::string filePath);

private:

 
    std::string ensureExtension(std::string filePath);
};

#endif // ZIPBIKE_FILEHANDLER_H