/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ ETSI MEC library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/etsimec/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

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
#include "Rest/datatype.h"

#include <map>
#include <string>
#include <vector>

namespace uiiit {
namespace etsimec {

/**
 * ETSI MEC application context.
 *
 * See ETSI GS MEC 016 v1.1.1 (2017-09).
 */
class AppContext : public rest::DataType
{
 public:
  enum Type {
    Request  = 0,
    Response = 1,
  };

  /**
   * Create an AppContext type (request version).
   *
   * \throw std::runtime_error if aAssociateId is empty.
   * \throw std::runtime_error if the AppInfo for the wrong message is used.
   */
  explicit AppContext(const std::string& aAssociateUeAppId,
                      const std::string& aCallbackReference,
                      const AppInfo&     aAppInfo);

  /**
   * Create an AppContext type (response version).
   *
   * \throw std::runtime_error if aContextId or aAssociateId are empty.
   * \throw std::runtime_error if the AppInfo for the wrong message is used.
   */
  explicit AppContext(const std::string& aContextId,
                      const std::string& aAssociateUeAppId,
                      const std::string& aCallbackReference,
                      const AppInfo&     aAppInfo);

  /**
   * Build from JSON object.
   *
   * \throw std::runtime_error if invalid.
   */
  explicit AppContext(const web::json::value& aJson);

  /**
   * \return an AppContext type of type response from a request by adding the
   * fields contextIt and referenceURI.
   *
   * \throw std::runtime_error if aContextId or aReferenceUri are empty.
   */
  AppContext makeResponse(const std::string& aContextId,
                          const std::string& aReferenceUri) const;

  //! \return the contextId, empty if this is a request message.
  std::string contextId() const;

  //! \return the associateUeAppId.
  std::string associateUeAppId() const;

  //! \return the callbackReference.
  std::string callbackReference() const;

  //! \return the AppInfo message.
  AppInfo appInfo() const;

  //! \return if this context is being used in a request or in a response.
  Type type() const;

 private:
  AppContext(const std::string& aContextId,
             const std::string& aAssociateUeAppId,
             const std::string& aCallbackReference,
             const AppInfo&     aAppInfo,
             const Type         aType);
};

} // namespace etsimec
} // namespace uiiit
