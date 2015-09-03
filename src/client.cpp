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

bool run_statement(std::string sql, sqlite3 **db, sqlite3_stmt **stmt) {
    if (sqlite3_open("/home/phablet/.local/share/webbrowser-app/bookmarks.sqlite", db)) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(*db) << std::endl;
        return false;
    }

    if (sqlite3_prepare_v2(*db, sql.data(), -1, stmt, NULL)) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(*db) << std::endl;
        return false;
    }

    return true;
}

BookmarkList get_bookmarks(std::string query, std::string folder, int sort) {
    BookmarkList bookmarks;

    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::string sql = "SELECT url, title, icon FROM bookmarks WHERE 1";
    if (query != "")
        sql += " AND (url LIKE '%' || ? || '%' OR title LIKE '%' || ?1 || '%')";
    if (folder != "")
        sql += " AND folderId = '" + folder + "'";
    if (sort == 0)
        sql += " ORDER BY length(title) > 0 DESC, title ASC";
    else
        sql += " ORDER BY created DESC";

    if (!run_statement(sql, &db, &stmt))
        goto exit;

    if (query != "") {
        if (sqlite3_bind_text(stmt, 1, query.data(), -1, SQLITE_STATIC)) {
            std::cerr << "Error binding text: " << sqlite3_errmsg(db) << std::endl;
            goto exit;
        }
    }

    {
        int res = sqlite3_step(stmt);
        while (res == SQLITE_ROW) {
            Bookmark b;
            b.url = sqlite3_column_string(stmt, 0, "");
            b.title = sqlite3_column_string(stmt, 1, b.url);
            b.icon = sqlite3_column_string(stmt, 2, "file:///usr/share/icons/suru/actions/scalable/stock_website.svg");
            bookmarks.emplace_back(b);
            res = sqlite3_step(stmt);
        }
        if (res != SQLITE_DONE) {
            std::cerr << "Error reading rows: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    exit:
    sqlite3_close(db);
    return bookmarks;
}

FolderList get_bookmark_folders() {
    FolderList folders;

    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::string sql = "SELECT folderId, folder FROM folders ORDER BY folder";

    if (!run_statement(sql, &db, &stmt))
        goto exit;

    {
        int res = sqlite3_step(stmt);
        while (res == SQLITE_ROW) {
            Folder f;
            f.id = sqlite3_column_string(stmt, 0, "-1");
            f.name = sqlite3_column_string(stmt, 1, "");
            folders.emplace_back(f);
            res = sqlite3_step(stmt);
        }
        if (res != SQLITE_DONE) {
            std::cerr << "Error reading rows: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    exit:
    sqlite3_close(db);
    return folders;
}

}
