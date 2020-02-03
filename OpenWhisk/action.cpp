/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ ETSI MEC library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/wsk/
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

#include "action.h"

namespace uiiit {
namespace wsk {

ActionKey Action::key() const {
  return ActionKey{theSpace, theName};
}

Action::Action(const std::string& aSpace,
               const std::string& aName,
               const uint64_t     aUpdated,
               const std::string& aVersion)
    : theSpace(aSpace)
    , theName(aName)
    , theUpdated(aUpdated)
    , theVersion(aVersion) {
  // noop
}

bool Action::operator==(const Action& aOther) const {
  return theSpace == aOther.theSpace and theName == aOther.theName and
         theUpdated == aOther.theUpdated and theVersion == aOther.theVersion;
}

std::string Action::toString() const {
  return key().toString() + ", updated " + std::to_string(theUpdated) +
         ", version " + theVersion;
}

bool sameKeys(const std::map<ActionKey, Action>& aLhs,
              const std::map<ActionKey, Action>& aRhs) {
  // short-circuit
  if (aLhs.size() != aRhs.size()) {
    return false;
  }

  // long way
  std::set<std::string> myLhs;
  std::set<std::string> myRhs;
  for (const auto& elem : aLhs) {
    myLhs.emplace(elem.first.toString());
  }
  for (const auto& elem : aRhs) {
    myRhs.emplace(elem.first.toString());
  }
  return myLhs == myRhs;
}

} // namespace wsk
} // namespace uiiit
