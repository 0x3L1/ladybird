/*
 * Copyright (c) 2022-2023, Linus Groh <linusg@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Forward.h>
#include <LibGC/Ptr.h>
#include <LibJS/Forward.h>
#include <LibWeb/Bindings/PlatformObject.h>
#include <LibWeb/Bindings/RequestPrototype.h>
#include <LibWeb/Fetch/Body.h>
#include <LibWeb/Fetch/BodyInit.h>
#include <LibWeb/Fetch/Headers.h>
#include <LibWeb/Fetch/Infrastructure/HTTP/Responses.h>
#include <LibWeb/Forward.h>

namespace Web::Fetch {

// https://fetch.spec.whatwg.org/#responseinit
struct ResponseInit {
    u16 status;
    String status_text;
    Optional<HeadersInit> headers;
};

// https://fetch.spec.whatwg.org/#response
class Response final
    : public Bindings::PlatformObject
    , public BodyMixin {
    WEB_PLATFORM_OBJECT(Response, Bindings::PlatformObject);
    GC_DECLARE_ALLOCATOR(Response);

public:
    [[nodiscard]] static GC::Ref<Response> create(JS::Realm&, GC::Ref<Infrastructure::Response>, Headers::Guard);
    static WebIDL::ExceptionOr<GC::Ref<Response>> construct_impl(JS::Realm&, Optional<BodyInit> const& body = {}, ResponseInit const& init = {});

    virtual ~Response() override;

    // ^BodyMixin
    virtual Optional<MimeSniff::MimeType> mime_type_impl() const override;
    virtual GC::Ptr<Infrastructure::Body> body_impl() override;
    virtual GC::Ptr<Infrastructure::Body const> body_impl() const override;
    virtual Bindings::PlatformObject& as_platform_object() override { return *this; }
    virtual Bindings::PlatformObject const& as_platform_object() const override { return *this; }

    [[nodiscard]] GC::Ref<Infrastructure::Response> response() const { return m_response; }

    // JS API functions
    [[nodiscard]] static GC::Ref<Response> error(JS::VM&);
    static WebIDL::ExceptionOr<GC::Ref<Response>> redirect(JS::VM&, String const& url, u16 status);
    static WebIDL::ExceptionOr<GC::Ref<Response>> json(JS::VM&, JS::Value data, ResponseInit const& init = {});
    [[nodiscard]] Bindings::ResponseType type() const;
    [[nodiscard]] String url() const;
    [[nodiscard]] bool redirected() const;
    [[nodiscard]] u16 status() const;
    [[nodiscard]] bool ok() const;
    [[nodiscard]] String status_text() const;
    [[nodiscard]] GC::Ref<Headers> headers() const;
    [[nodiscard]] WebIDL::ExceptionOr<GC::Ref<Response>> clone() const;

    // Pull in json() from the BodyMixin, which gets lost due to the static json() above
    using BodyMixin::json;

private:
    Response(JS::Realm&, GC::Ref<Infrastructure::Response>);

    virtual void initialize(JS::Realm&) override;
    virtual void visit_edges(Cell::Visitor&) override;

    WebIDL::ExceptionOr<void> initialize_response(ResponseInit const&, Optional<Infrastructure::BodyWithType> const&);

    // https://fetch.spec.whatwg.org/#concept-response-response
    // A Response object has an associated response (a response).
    GC::Ref<Infrastructure::Response> m_response;

    // https://fetch.spec.whatwg.org/#response-headers
    // A Response object also has an associated headers (null or a Headers object), initially null.
    GC::Ptr<Headers> m_headers;
};

}
