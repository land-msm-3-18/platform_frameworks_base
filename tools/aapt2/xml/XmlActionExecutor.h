/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AAPT_XML_XMLPATTERN_H
#define AAPT_XML_XMLPATTERN_H

#include "Diagnostics.h"
#include "xml/XmlDom.h"

#include <android-base/macros.h>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace aapt {
namespace xml {

enum class XmlActionExecutorPolicy {
    /**
     * Actions on run if elements are matched, errors occur only when actions return false.
     */
    None,

    /**
     * The actions defined must match and run. If an element is found that does not match
     * an action, an error occurs.
     */
    Whitelist,
};

/**
 * Contains the actions to perform at this XML node. This is a recursive data structure that
 * holds XmlNodeActions for child XML nodes.
 */
class XmlNodeAction {
public:
    using ActionFuncWithDiag = std::function<bool(Element*, SourcePathDiagnostics*)>;
    using ActionFunc = std::function<bool(Element*)>;

    /**
     * Find or create a child XmlNodeAction that will be performed for the child element
     * with the name `name`.
     */
    XmlNodeAction& operator[](const std::string& name) {
        return mMap[name];
    }

    /**
     * Add an action to be performed at this XmlNodeAction.
     */
    void action(ActionFunc f);
    void action(ActionFuncWithDiag);

private:
    friend class XmlActionExecutor;

    bool execute(XmlActionExecutorPolicy policy, SourcePathDiagnostics* diag, Element* el) const;

    std::map<std::string, XmlNodeAction> mMap;
    std::vector<ActionFuncWithDiag> mActions;
};

/**
 * Allows the definition of actions to execute at specific XML elements defined by their
 * hierarchy.
 */
class XmlActionExecutor {
public:
    XmlActionExecutor() = default;

    /**
     * Find or create a root XmlNodeAction that will be performed for the root XML element
     * with the name `name`.
     */
    XmlNodeAction& operator[](const std::string& name) {
        return mMap[name];
    }

    /**
     * Execute the defined actions for this XmlResource.
     * Returns true if all actions return true, otherwise returns false.
     */
    bool execute(XmlActionExecutorPolicy policy, IDiagnostics* diag, XmlResource* doc) const;

private:
    std::map<std::string, XmlNodeAction> mMap;

    DISALLOW_COPY_AND_ASSIGN(XmlActionExecutor);
};

} // namespace xml
} // namespace aapt

#endif /* AAPT_XML_XMLPATTERN_H */
