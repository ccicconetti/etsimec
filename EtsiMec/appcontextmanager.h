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
