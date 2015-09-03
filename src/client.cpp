#include <client.h>
#include <sqlite3.h>
#include <iostream>

namespace Client {

std::string sqlite3_column_string(sqlite3_stmt *stmt, int col, std::string def) {
    const unsigned char *txt = sqlite3_column_text(stmt, col);
    if (txt == NULL)
        return def;
    std::string res(reinterpret_cast<const char*>(txt));
    if (res == "")
        return def;
    return res;
}

BookmarkList get_bookmarks(std::string query, int sort) {
    BookmarkList bookmarks;

    sqlite3 *db;
    std::string sql = "SELECT url, title, icon FROM bookmarks";
    if (query != "")
        sql += " WHERE url LIKE '%' || ? || '%' OR title LIKE '%' || ?1 || '%'";
    if (sort == 0)
        sql += " ORDER BY length(title) > 0 DESC, title ASC";
    else
        sql += " ORDER BY created DESC";

    int res;
    res = sqlite3_open("/home/phablet/.local/share/webbrowser-app/bookmarks.sqlite", &db);
    if (res) {
        std::cerr << "A " << sqlite3_errmsg(db) << std::endl;
        goto exit;
    }

    sqlite3_stmt *stmt;
    res = sqlite3_prepare_v2(db, sql.data(), -1, &stmt, NULL);
    if (res) {
        std::cerr << "B " << sqlite3_errmsg(db) << std::endl;
        goto exit;
    }

    if (query != "") {
        res = sqlite3_bind_text(stmt, 1, query.data(), -1, SQLITE_STATIC);
        if (res) {
            std::cerr << "D " << sqlite3_errmsg(db) << std::endl;
            goto exit;
        }
    }

    res = sqlite3_step(stmt);
    while (res == SQLITE_ROW) {
        Bookmark b;
        b.url = sqlite3_column_string(stmt, 0, "");
        b.title = sqlite3_column_string(stmt, 1, b.url);
        b.icon = sqlite3_column_string(stmt, 2, "file:///usr/share/icons/suru/actions/scalable/stock_website.svg");
        bookmarks.emplace_back(b);
        res = sqlite3_step(stmt);
    }
    if (res != SQLITE_DONE) {
        std::cerr << "C " << sqlite3_errmsg(db) << std::endl;
    }

    exit:
    sqlite3_close(db);
    return bookmarks;
}

}
