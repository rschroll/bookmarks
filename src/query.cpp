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
    try {
        // Start by getting information about the query
        const sc::CannedQuery &query(sc::SearchQueryBase::query());

        int sort = settings().at("sort").get_int();
        Client::BookmarkList bookmarks = Client::get_bookmarks(query.query_string(), sort);

        // Register a category for the current weather, with the title we just built
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

    } catch (domain_error &e) {
        // Handle exceptions being thrown by the client API
        cerr << e.what() << endl;
        reply->error(current_exception());
    }
}
