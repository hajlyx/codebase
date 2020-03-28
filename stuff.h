#ifndef ZW_STUFF_H
#define ZW_STUFF_H

#include <string.h>
#include <assert.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <experimental/filesystem>

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

namespace zw
{
/** @brief Clip {value} by bound ({low}, {high}).

Note: The types of {low}, {high} and {value} must be same.
The type of return is also the same as {value}.
*/
template <class T>
T tClip(T value, T low, T high)
{
    return std::min(std::max(value, low), high);
}
/** @brief Compare struct used in fuction argsort.

The types of {a} and {b} are size_t, the sort order is low->high.
*/
template <class T>
struct index_cmp
{
    index_cmp(const T arr) : arr(arr) {}
    bool operator()(const size_t a, const size_t b) const
    {
        return arr[a] < arr[b];
    }
    const T arr;
};
/** @brief Get the order of the {vT} without change {vT}.

The type of {vIndex} is std::vector<size_t>, if lowToHigh is true, than {vIndex}
will be sorted as low->high, otherwise high->low.
*/
template <class T>
void argsort(std::vector<T> &vT, std::vector<size_t> &vIndex, bool lowToHigh)
{
    vIndex.resize(vT.size());
    std::iota(std::begin(vIndex), std::end(vIndex), 0); // Fill with 0, 1, ..., vT.size().

    if (lowToHigh)
    {
        std::sort(vIndex.begin(), vIndex.end(), index_cmp<std::vector<T> &>(vT));
    }
    else
    {
        std::sort(vIndex.rbegin(), vIndex.rend(), index_cmp<std::vector<T> &>(vT));
    }
}
/** @brief Split a string {s} to several {tokens} based on {delim}.

Note: this function will skip empty token. For example, string "123.45..678" will
be split to {123, 45, 678}.
*/
void split(const std::string &s, std::vector<std::string> &tokens, const char delim)
{
    std::string::size_type lastPos = s.find_first_not_of(delim, 0);
    std::string::size_type pos = s.find_first_of(delim, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        tokens.emplace_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delim, pos);
        pos = s.find_first_of(delim, lastPos);
    }
}
/** @brief Lambda: Determine if {k} is equal to any one of {args}

Example 1, isOneOf("png", "jpg", "bmp")->false, because {k}->"png" is not
equal to any one of {args}->["jpg", "bmp"].
Example 2, isOneOf("png", "jpg", "bmp", "png")->true. {k}->"png", {args}->
["jpg", "bmp", "png"].
*/
auto isOneOf = [](auto &&k, auto &&... args) -> bool { return ((args == k) || ...); };
/** @brief Determine if {fileName} is a image file.

The {fileName} is just a file name like "dog.jpg" or "fish.jpg" instead of a path
like "/tmp/animal/dog.jpg", so this function will not check if {fileName} is
available.
*/
bool isImageFile(std::string fileName)
{
    std::string suffix = fileName.substr(fileName.find_last_of('.') + 1);
    return isOneOf(suffix, "jpg", "jpeg", "bmp", "png", "tif", "tiff");
}
/** @brief find absolute path {vFile} for all images in folder {path}, optinal
recursive {subFolder}

if {subFolder} is true, then this function will find images recursively, otherwise
not. Need to add link flag -lstdc++fs.
*/
void getImageFile(std::vector<std::string> &vFile, std::string path, bool subFolder)
{
    if (path.empty() || !std::experimental::filesystem::exists(path))
    {
        printf("The path [%s] is not available\n", path.c_str());
        return;
    }

    std::string spath = path;
    if (spath[spath.length() - 1] != '/')
        spath.append("/");
    std::string subpath = spath;

#ifdef _WIN32
    intptr_t hFile = 0;
    struct _finddata_t fileinfo;

    hFile = _findfirst((spath + "*").c_str(), &fileinfo);
    if (hFile == -1)
        return;

    do
    {
        if (subFolder && (fileinfo.attrib & _A_SUBDIR))
        {
            if ((strcmp(fileinfo.name, ".") != 0) && (strcmp(fileinfo.name, "..") != 0))
            {
                getImageFile(vFile, spath + fileinfo.name, subFolder);
            }
        }
        else
        {
            if (isImageFile(fileinfo.name))
            {
                vFile.push_back(subpath + fileinfo.name);
            }
        }
    } while (_findnext(hFile, &fileinfo) == 0);
    _findclose(hFile);
#else
    struct dirent *direntp;
    DIR *dirp = opendir(spath.c_str());

    if (dirp == NULL)
        return;

    while ((direntp = readdir(dirp)) != NULL)
    {
        if (subFolder && (direntp->d_type == DT_DIR))
        {
            if ((strcmp(direntp->d_name, ".") != 0) && (strcmp(direntp->d_name, "..") != 0))
            {
                getImageFile(vFile, spath + direntp->d_name, subFolder);
            }
        }
        else
        {
            if (isImageFile(direntp->d_name))
            {
                vFile.push_back(subpath + direntp->d_name);
            }
        }
    }
    closedir(dirp);
#endif
}
} // namespace zw
#endif // ZW_STUFF_H