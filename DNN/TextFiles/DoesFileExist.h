Bool_t R3BTextFileGenerator::DoesFileExist(const std::string &s)
{
    // Checks whether a file or a path exists.
    // requires #include <sys/stat.h>
    struct stat buffer;
    return (stat (s.c_str(), &buffer) == 0);
}
