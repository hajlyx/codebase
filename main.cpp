#include "stuff.h"

void test_split()
{
    // func: split
    std::vector<std::string> tokens;
    zw::split("11111..11..1111", tokens, '.');
    for (auto token : tokens)
    {
        std::cout << token << std::endl;
    }
}

void test_getImageFile()
{
    // func: getImageFile
    std::vector<std::string> vFile;
    zw::getImageFile(vFile, "/home/zw/zwspace/alldata/goggles", true);
    for (auto path : vFile)
    {
        std::cout << path << std::endl;
    }
}

void test_argsort()
{
    // func: argsort
    std::vector<size_t> vIndex;
    std::vector<float> vScore = {0.5, 0.2, 0.3, 0.9, 1.2};
    zw::argsort(vScore, vIndex, true);
    for (auto ind : vIndex)
    {
        std::cout << ind << "\t";
    }
    std::cout << std::endl;
}

int main()
{
    // test_split();
    test_getImageFile();
    // test_argsort();
}