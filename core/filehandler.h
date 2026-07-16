#ifndef ZIPBIKE_FILEHANDLER_H
#define ZIPBIKE_FILEHANDLER_H

#include <string>
#include <vector>
#include "container.h"

struct archiveEntry {
    std::string path;
    bool isDirectory;
    container data;

    archiveEntry() : isDirectory(false) {}
};

class filehandler {
public:

    filehandler();

    bool save(container& c, std::string filePath);
    container load(std::string filePath);

    bool saveArchive(const std::vector<archiveEntry>& entries, std::string filePath);
    bool loadArchive(std::string filePath, std::vector<archiveEntry>& entries);

    bool fileExists(std::string filePath);

    std::string ensureExtension(std::string filePath);
};

#endif
