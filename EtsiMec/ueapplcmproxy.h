#pragma once

#include "EtsiMec/appcontext.h"
#include "EtsiMec/appinfo.h"
#include "EtsiMec/etsimecserver.h"
#include "Support/macros.h"

#include <map>
#include <mutex>
#include <string>

namespace uiiit {
namespace etsimec {

/**
 * The UE application LCM proxy, as defined in ETSI GS MEC 003 v1.1.1 (2016-03).
 */
class UeAppLcmProxy : public EtsiMecServer
{
  NONCOPYABLE_NONMOVABLE(UeAppLcmProxy);

 public:
  explicit UeAppLcmProxy(const std::string& aApiRoot);

  static const std::string& staticApiName() noexcept;
  static const std::string& staticApiVersion() noexcept;
  const std::string&        apiName() const noexcept override;
  const std::string&        apiVersion() const noexcept override;

  /**
   * Add a new application. Update if already present.
   *
   * \return true if added, false if updated.
   */
  bool addApp(const AppInfo& aAppInfo);

  /**
   * Remove an app.
   *
   * \return true if actually removed, false otherwise.
   */
  bool delApp(const AppInfo& aAppInfo);

 protected:
  /**
   * Create a context for the given UE application.
   *
   * \param aClientAddress the address of the client issuing the request.
   * \param aRequest the UE application context requested.
   * \return an AppContext structure: if the context is present it means that
   * the creation has been authorized, otherwise it is refused.
   */
  virtual AppContext createContext(const std::string& aClientAddress,
                                   const AppContext&  aRequest) = 0;

  /**
   * Update the callbackReference of the given request.
   *
   * \return true if the context is present and the request correct, false
   * otherwise.
   */
  virtual bool updateContext(const AppContext& aRequest) = 0;

  /**
   * Destroy the UE application context with the given context ID.
   *
   * \return true if the context is present and has been deleted with success,
   * false otherwise.
   */
  virtual bool deleteContext(const std::string& aContextId) = 0;

  //! \return true if the given application exists, false otherwise.
  bool exists(const AppInfo& aAppInfo) const;

 private:
  static std::string mangle(const AppInfo& aAppInfo);

  void handleAppList(web::http::http_request aReq) const;
  void handleContextCreate(web::http::http_request aReq);
  void handleContextDelete(web::http::http_request aReq);
  void handleContextUpdate(web::http::http_request aReq);

 private:
  mutable std::mutex theMutex;

  // map of applications available, protected by theMutex
  // key: appName + appProvider + appSoftVersion
  std::map<std::string, AppInfo> theApplications;

  // static configuration
  static const std::string theApiName;
  static const std::string theApiVersion;
};

} // namespace etsimec
} // namespace uiiit
