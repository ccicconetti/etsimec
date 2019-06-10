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

#include "EtsiMec/ueapplcmproxy.h"

#include <list>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

namespace uiiit {
namespace etsimec {
struct AddressAppKey {
  explicit AddressAppKey(const std::string& aAddress,
                         const std::string& aAppName)
      : theAddress(aAddress)
      , theAppName(aAppName) {
  }
  bool operator==(const AddressAppKey& aOther) const noexcept {
    return theAddress == aOther.theAddress and theAppName == aOther.theAppName;
  }

  std::string theAddress;
  std::string theAppName;
};
} // namespace etsimec
} // namespace uiiit

namespace std {
template <>
struct hash<uiiit::etsimec::AddressAppKey> {
  size_t operator()(const uiiit::etsimec::AddressAppKey& aKey) const {
    return std::hash<std::string>()(aKey.theAddress + aKey.theAppName);
  }
};
} // namespace std

namespace uiiit {
namespace etsimec {

/**
 * A simple static UE application LCM proxy.
 *
 * A context can be created only for applications that have been added
 * beforehand. If an application is deleted after a context has been created,
 * then no action is taken on the existing context.
 *
 * Upon creation of a UE application it looks for the address of the client and
 * the name of the UE application in an in-memory table: if found it assigns the
 * corresponding referenceURI.
 *
 * A default referenceURI for a given UE application name can be set if the
 * client's address does not match any entry in the table. If the default
 * referenceURI is not found and there is no default, the creation is not
 * authorized. Otherwise, any application is immediately authorized without
 * limitations.
 */
class StaticUeAppLcmProxy : public UeAppLcmProxy
{
  struct Desc {
    std::string theAssociateUeAppId;
    std::string theCallbackReference;
    std::string theClientAddress;
    std::string theAppName;
    std::string theEdgeRouterEndpoint;
  };
  using ApplicationsByContextId = std::unordered_map<std::string, Desc>;

  // 0 edge client address   (never empty)
  // 1 application name      (never empty)
  // 2 edge router end-point (can be empty)
  using ClientTuple = std::tuple<std::string, std::string, std::string>;

  // 0 edge client address   (can be empty)
  // 1 application name      (never empty)
  // 2 edge router end-point (never empty)
  using TableTuple = std::tuple<std::string, std::string, std::string>;

  using Table = std::unordered_map<AddressAppKey, std::string>;

  // 0 callbackReference
  // 1 referenceURI
  // 2 contextId
  using NotificationList =
      std::list<std::tuple<std::string, std::string, std::string>>;

 public:
  explicit StaticUeAppLcmProxy(const std::string& aApiRoot);

  /**
   * Change the association between a pair of edge client address and
   * application name and the end-point of an edge router.
   *
   * \param aAddress the edge client address; if empty means any
   * \param aAppName the application name; cannot be empty
   * \param aEdgeRouter the edge router end-point; cannot be empty
   *
   * \throw std::runtime_error if aAppName or aEdgeRouter is empty.
   */
  void associateAddress(const std::string& aAddress,
                        const std::string& aAppName,
                        const std::string& aEdgeRouter);

  /**
   * Remove the association of a given pair of edge client address and
   * application name.
   *
   * \param aAddress the edge client address; if empty means any
   * \param aAppName the application name; cannot be empty
   *
   * \throw std::runtime_error if aAddress is empty.
   */
  void removeAddress(const std::string& aAddress, const std::string& aAppName);

  //! \return the number of static addresses registered.
  size_t numAddresses() const;

  //! \return the number of active application contexts.
  size_t numContexts() const;

  /**
   * \return the edge router client associated to this pair, or empty if none.
   *
   * \throw std::runtime_error if aAddress or aAppName are empty.
   */
  std::string edgeRouter(const std::string& aAddress,
                         const std::string  aAppName) const;

  //! \return the address associations table.
  std::list<TableTuple> addressAssociations() const;

  //! \return the clients with an active contexts.
  std::list<ClientTuple> contexts() const;

 protected:
  AppContext createContext(const std::string& aClientAddress,
                           const AppContext&  aRequest) override;
  bool       updateContext(const AppContext& aRequest) override;
  bool       deleteContext(const std::string& aContextId) override;

 private:
  /**
   * Issue a POST NotificationEvent on all the servers in the list.
   *
   * Remove context if any error occurs.
   */
  void notifyClients(const NotificationList& aNotificationList);

  /**
   * Remove the given context.
   *
   * \return true if actually removed, false otherwise.
   */
  bool purge(const std::string& aContextId);

  /**
   * \return the entry in the table matching a key.
   *
   * If the given edge client address and application name match both the
   * wildcard (ie. empty edge client address) and the specific entry, the latter
   * is returned.
   */
  Table::const_iterator find(const std::string& aAddress,
                             const std::string& aAppName) const;

 private:
  mutable std::mutex theMutex;

  // key:   <edge client address, app name>
  // value: edge router end-point
  Table theTable;

  // key:   context ID
  // value: application context descriptor
  ApplicationsByContextId theApplicationsByContextId;
  // key:   UE application ID
  // value: iterator to theApplicationsByContextId
  std::unordered_map<std::string, ApplicationsByContextId::iterator>
      theApplicationsByUeAppId;
};

} // namespace etsimec
} // namespace uiiit
