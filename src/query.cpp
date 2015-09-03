#include <query.h>
#include <localization.h>

#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>

#include <iostream>

namespace sc = unity::scopes;

using namespace std;


const static string BOOKMARK_TEMPLATE =
        R"(
{
        "schema-version": 1,
        "template": {
        "category-layout": "grid",
        "card-size": "small"
        },
        "components": {
        "title": "title",
        "art" : {
        "field": "art"
        },
        "subtitle": "subtitle"
        }
        }
        )";

Query::Query(const sc::CannedQuery &query, const sc::SearchMetadata &metadata) :
    sc::SearchQueryBase(query, metadata) {
}

void Query::cancelled() {
}


void Query::run(sc::SearchReplyProxy const& reply) {
    // Start by getting information about the query
    const sc::CannedQuery &query(sc::SearchQueryBase::query());

    // The empty string here is important; it denotes the department to use when none has been
    // selected by the user.
    sc::Department::SPtr all = sc::Department::create("", query, _("All bookmarks"));
    Client::FolderList folders = Client::get_bookmark_folders();
    for (const Client::Folder folder : folders) {
        sc::Department::SPtr dept = sc::Department::create(folder.id, query, folder.name);
        all->add_subdepartment(dept);
    }
    reply->register_departments(all);

    int sort = settings().at("sort").get_int();
    Client::BookmarkList bookmarks =
            Client::get_bookmarks(query.query_string(), query.department_id(), sort);

    auto cat = reply->register_category("bookmarks", "", "",
                                        sc::CategoryRenderer(BOOKMARK_TEMPLATE));

    for (const Client::Bookmark bookmark : bookmarks) {
        sc::CategorisedResult res(cat);
        res.set_uri(bookmark.url);
        res.set_title(bookmark.title);
        res.set_art(bookmark.icon);
        res.set_intercept_activation();

        if (!reply->push(res)) {
            // Query has been cancelled.
            return;
        }
    }
}

