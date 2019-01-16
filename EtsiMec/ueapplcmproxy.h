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
  FRIEND_TEST(TestGrpcUeAppLcmProxy, test_lambda);

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
