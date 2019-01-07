/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ ETSI MEC library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/etsimec/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "EtsiMec/appinfo.h"
#include "Rest/server.h"
#include "Support/macros.h"

#include <mutex>
#include <string>

namespace uiiit {
namespace etsimec {

/**
 * Manager of client-side UE application context.
 *
 * It contacts the UE App LCM proxy and gets back the referenceUri.
 * Also, it runs an HTTP server to receive notifiations from the UE App LCM
 * proxy.
 *
 * For each application the notification URI indicated upon context creation is
 * equal to the base notification URI passed to the ctor + / + the UE
 * application ID, which is unique. This allows the manager to match incoming
 * notifications for multiple applications.
 */
class AppContextManager : public rest::Server
{
  NONCOPYABLE_NONMOVABLE(AppContextManager);

  //! Context descriptor.
  struct Desc {
    std::string theReferenceUri;
    std::string theContextId;
  };

 public:
  /**
   * \param aNotificationUri The URI of the REST server to receive notifications
   * for updates of the AppContext created.
   * \param aProxyUri The URI of the UE App LCM proxy to contact.
   */
  explicit AppContextManager(const std::string& aNotificationUri,
                             const std::string& aProxyUri);

  //! Delete all the open contexts from the UE app LCM proxy.
  ~AppContextManager() override;

  /**
   * Create a context for the application with the given information.
   *
   * \param aAppInfo The application information.
   * \return The UE application ID (chosen by the manager) and reference URI
   * (returned by the LCM proxy and to be used by the application to use the
   * service).
   *
   * \throw if aAppInfo is invalid or if the context cannot be created for any
   * reason (including server refusing to do so).
   */
  std::pair<std::string, std::string> contextCreate(const AppInfo& aAppInfo);

  /**
   * \return the current reference URI of an application reference by its ID.
   *
   * \throw std::runtime_error if the given UE application ID does not exist.
   */
  std::string referenceUri(const std::string& aUeAppId) const;

  //! \return the URI of the ETSI UE application LCM proxy.
  const std::string& proxyUri() const noexcept {
    return theProxyUri;
  }

 private:
  void handleNotification(web::http::http_request aReq);

 private:
  const std::string theNotificationUri;
  const std::string theProxyUri;

  mutable std::mutex theMutex; // protects theReferenceUris
  // key: UE application ID, value: corresponding context descriptor
  std::unordered_map<std::string, Desc> theContexts;
};

} // namespace etsimec
} // namespace uiiit
