#ifndef CLIENT_H_
#define CLIENT_H_

#include <atomic>
#include <deque>
#include <map>
#include <memory>
#include <string>

namespace Client {

struct Bookmark {
    std::string url;
    std::string title;
    std::string icon;
};

typedef std::deque<Bookmark> BookmarkList;

BookmarkList get_bookmarks(std::string query, std::string folder, int sort);

struct Folder {
    std::string id;
    std::string name;
};

typedef std::deque<Folder> FolderList;

FolderList get_bookmark_folders();

}

#endif // CLIENT_H_

