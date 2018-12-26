#pragma once

#include "EtsiMec/ueapplcmproxy.h"

#include <list>
#include <mutex>
#include <unordered_map>
#include <utility>

namespace uiiit {
namespace etsimec {

/**
 * A simple static UE application LCM proxy.
 *
 * A context can be created only for applications that have been added
 * beforehand. If an application is deleted after a context has been created,
 * then no action is taken on the existing context.
 *
 * Upon creation of a UE application it looks for the address of the client in
 * an in-memory table: if found it assigns the corresponding referenceURI.
 * A default referenceURI can be set and is used if the client's address does
 * not match any entry in the table.
 * If the default referenceURI is not found and there is no default, the
 * creation is not authorized. Otherwise, any application is immediately
 * authorized without limitations.
 */
class StaticUeAppLcmProxy : public UeAppLcmProxy
{
  struct Desc {
    std::string theAssociateUeAppId;
    std::string theCallbackReference;
    std::string theClientAddress;
    std::string theEdgeRouterAddress;
    bool        theDefaultEdgeRouterAddress;
  };
  using ApplicationsByContextId = std::unordered_map<std::string, Desc>;

  // 0 callbackReference
  // 1 referenceURI
  // 2 contextId
  using NotificationList =
      std::list<std::tuple<std::string, std::string, std::string>>;

 public:
  explicit StaticUeAppLcmProxy(const std::string& aApiRoot);

  //! Set the default edge router. Clear if the passed string is empty.
  void defaultEdgeRouter(const std::string& aEdgeRouter);

  //! Change the association between client and edge router addresses.
  void associateAddress(const std::string& aAddress,
                        const std::string& aEdgeRouter);
  //! Remove the association of a client from an edge router.
  void removeAddress(const std::string& aAddress);

  //! \return the number of static addresses registered.
  size_t numAddresses() const;

  //! \return the number of active application contexts.
  size_t numContexts() const;

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

 private:
  mutable std::mutex theMutex;

  std::string                                  theDefaultEdgeRouter;
  std::unordered_map<std::string, std::string> theAddressAssociations;
  ApplicationsByContextId                      theApplicationsByContextId;
  std::unordered_map<std::string, ApplicationsByContextId::iterator>
      theApplicationsByUeAppId;
};

} // namespace etsimec
} // namespace uiiit
