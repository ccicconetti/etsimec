#pragma once

#include "Edge/edgeclientinterface.h"
#include "Edge/edgemessages.h"

#include <map>
#include <utility>

namespace uiiit {
namespace edge {
class EdgeClient;
}
} // namespace uiiit

namespace uiiit {
namespace etsimec {

class AppContextManager;

/**
 * An edge client that uses an ETSI application context manager to select the
 * edge node to which the lambda requests are sent.
 *
 * This is done by polling the AppContextManager before issuing every lambda
 * request.
 *
 * Upon execution of a lambda an application context is created on the ETSI UE
 * application LCM proxy, if not already present, via the app context manager.
 * The UE application to use is selected from the catalog exposed by the LCM
 * proxy, which is refreshed at every context creation:
 * - if a matching UE application is not found, then the execution of the
 *   lambda fails;
 * - if there are multiple options, then the first occurence is picked, without
 * looking to the appCharcs structure within the AppInfo's.
 */
class EtsiEdgeClient final : public uiiit::edge::EdgeClientInterface
{
  struct Desc {
    Desc(const std::string&                         aUeAppId,
         const std::string&                         aReferenceUri,
         std::unique_ptr<uiiit::edge::EdgeClient>&& aClient)
        : theUeAppId(aUeAppId)
        , theReferenceUri(aReferenceUri)
        , theClient(
              std::forward<std::unique_ptr<uiiit::edge::EdgeClient>>(aClient)) {
    }

    //! Initized upon context creation.
    const std::string theUeAppId;

    //! Updated according to the app context manager.
    std::string theReferenceUri;

    //! Edge client to actually perform execution of lambda functions.
    std::unique_ptr<uiiit::edge::EdgeClient> theClient;
  };

 public:
  /**
   * \param aAppContextManager The ETSI application context manager.
   */
  explicit EtsiEdgeClient(AppContextManager& aAppContextManager);
  ~EtsiEdgeClient();

  /*
   * \param aDry if true do not actually execute the lambda
   */
  uiiit::edge::LambdaResponse RunLambda(const uiiit::edge::LambdaRequest& aReq,
                                        const bool aDry) override;

 private:
  /**
   * If a context exists then the app context manager is queried to check if
   * the reference URI is still the same: if not, then a new edge client is
   * created to contact the given reference URI, otherwise the existing client
   * is used.
   *
   * Otherwise, the method contacts the ETSI UE application LCM proxy
   * to retrieve the list of applications, find the missing details for the
   * AppInfo structure, then use the latter to create a context.
   *
   * \param aLambda The lambda name.
   *
   * \return the edge client to be used.
   *
   * \throw std::runtime_error if there is no suitable application in the UE
   * application LCM proxy or the context creation procedure failed.
   */
  uiiit::edge::EdgeClient& find(const std::string& aLambda);

 private:
  AppContextManager& theAppContextManager;

  // key: lambda name
  // value: descriptor of the lambda
  std::unordered_map<std::string, Desc> theClients;
}; // end class EtsiEdgeClient

} // namespace etsimec
} // namespace uiiit
