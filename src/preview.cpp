/* Copyright 2015 Robert Schroll
 *
 * This file is part of Browser Bookmarks Scope and is distributed under the
 * terms of the GPL. See the file LICENSE for full details.
 */

#include <preview.h>
#include <localization.h>

#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Result.h>
#include <unity/scopes/VariantBuilder.h>

#include <iostream>

namespace sc = unity::scopes;

using namespace std;

Preview::Preview(const sc::Result &result, const sc::ActionMetadata &metadata) :
    sc::PreviewQueryBase(result, metadata) {
}

void Preview::cancelled() {
}

void Preview::run(sc::PreviewReplyProxy const& reply) {
    sc::Result res = result();
    sc::ColumnLayout layout1col(1), layout2col(2);

    // Single column layout
    layout1col.add_column( { "image_widget", "header_widget", "openers" } );

    // Two column layout
    layout2col.add_column( { "image_widget" } );
    layout2col.add_column( { "header_widget", "openers" } );

    // Register the layouts we just created
    reply->register_layout( { layout1col, layout2col } );

    // Define the image section
    sc::PreviewWidget image("image_widget", "image");
    // It has a single "source" property, mapped to the result's "art" property
    image.add_attribute_mapping("source", "art");

    // Define the header section
    sc::PreviewWidget header("header_widget", "header");
    // It has a "title" and a "subtitle" property
    header.add_attribute_mapping("title", "title");
    header.add_attribute_mapping("subtitle", "uri");

    sc::PreviewWidget openers("openers", "actions");
    {
        sc::VariantBuilder builder;
        builder.add_tuple({ { "id", sc::Variant("open browser") },
                            { "label", sc::Variant(_("Open")) },
                            { "uri", sc::Variant(res.uri()) } });
        openers.add_attribute_value("actions", builder.end());
    }

    // Push each of the sections
    reply->push( { image, header, openers } );
}

