#pragma once

#include <cpprest/http_msg.h>
#include <cpprest/json.h>

#include <string>

namespace uiiit {
namespace etsimec {

std::string mx2root(const std::string& aApiRoot);

web::json::value parseJson(web::http::http_request aReq);

} // namespace etsimec
} // namespace uiiit
