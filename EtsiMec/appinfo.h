#pragma once

#include "EtsiMec/appcharcs.h"
#include "Rest/datatype.h"
#include "Support/thrower.h"

#include <iostream>
#include <string>

namespace uiiit {
namespace etsimec {

/**
 * ETSI MEC application information.
 *
 * See ETSI GS MEC 016 v1.1.1 (2017-09).
 */
class AppInfo : public rest::DataType
{
 public:
  //! The message in which this structure appears.
  enum Message {
    AppContextRequest  = 0,
    AppContextResponse = 1,
    ApplicationList    = 2,
  };

  /**
   * Application information, as in an AppContext request.
   *
   * \throw std::runtime_error if appName or appProvider or appSoftVersion are
   * longer than 32 characters, or if aAppDescription is longer than 128
   * characters.
   *
   * \throw std::runtime_error if appName or appProvider.
   */
  explicit AppInfo(const std::string& aAppName,
                   const std::string& aAppProvider,
                   const std::string& aAppSoftVersion,
                   const std::string& aAppDescription,
                   const std::string& aAppPackageSource);

  /**
   * Application information, as in an AppContext response.
   *
   * \throw std::runtime_error if appName or appProvider or appSoftVersion are
   * longer than 32 characters, or if aAppDescription is longer than 128
   * characters.
   *
   * \throw std::runtime_error if appName or appProvider or aReferenceUri are
   * empty.
   */
  explicit AppInfo(const std::string& aAppName,
                   const std::string& aAppProvider,
                   const std::string& aAppSoftVersion,
                   const std::string& aAppDescription,
                   const std::string& aReferenceUri,
                   const std::string& aAppPackageSource);

  /**
   * Application information, as in ApplicationList.
   */
  explicit AppInfo(const std::string& aAppName,
                   const std::string& aAppProvider,
                   const std::string& aAppSoftVersion,
                   const std::string& aAppDescription,
                   const AppCharcs&   aAppCharcs);

  /**
   * Build from JSON object.
   *
   * \throw std::runtime_error if invalid.
   */
  explicit AppInfo(const web::json::value& aJson);

  /**
   * \return an AppInfo structure of type AppContextResponse from one of type
   * AppContextRequest.
   *
   * \throw std::runtime_error if this AppInfo is not of type AppContextRequest
   * \throw std::runtime_error if aReferenceUri is empty.
   */
  AppInfo makeAppContextResponse(const std::string& aReferenceUri) const;

  /**
   * \return an AppInfo structure of type AppContextRequest from one of type
   * ApplicationList.
   *
   * \throw std::runtime_error if this AppInfo is not of type ApplicationList.
   */
  AppInfo makeAppContextRequest(const std::string& aAppPackageSource) const;

  //! \return the application name.
  std::string appName() const;

  //! \return the application provider.
  std::string appProvider() const;

  //! \return the application software version, or empty string if not present.
  std::string appSoftVersion() const;

  //! \return the application description, or empty string if not present.
  std::string appDescription() const;

  //! \return the application package source, or empty string if not present.
  std::string appPackageSource() const;

  /**
   * \return the reference URI.
   *
   * \throw std::runtime_error if this structure is not intended to be used in a
   * message AppContextResponse.
   */
  std::string referenceUri() const;

  //! \return the message type that this structure is intended for.
  Message message() const;

 private:
  void commonSetup(const std::string& aAppName,
                   const std::string& aAppProvider,
                   const std::string& aAppSoftVersion,
                   const std::string& aAppDescription,
                   support::Thrower&  aThrower);
};

std::string toString(const AppInfo::Message aMessage);

} // namespace etsimec
} // namespace uiiit

std::ostream& operator<<(std::ostream&                  aStream,
                         const uiiit::etsimec::AppInfo& aAppInfo);
