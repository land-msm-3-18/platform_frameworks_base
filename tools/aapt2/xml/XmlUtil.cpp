/*
 * Copyright (C) 2015 The Android Open Source Project
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

#include "util/Maybe.h"
#include "util/Util.h"
#include "xml/XmlUtil.h"

#include <string>

namespace aapt {
namespace xml {

std::string buildPackageNamespace(const StringPiece& package) {
    std::string result = kSchemaPublicPrefix;
    result.append(package.data(), package.size());
    return result;
}

Maybe<ExtractedPackage> extractPackageFromNamespace(const std::string& namespaceUri) {
    if (util::stringStartsWith(namespaceUri, kSchemaPublicPrefix)) {
        StringPiece schemaPrefix = kSchemaPublicPrefix;
        StringPiece package = namespaceUri;
        package = package.substr(schemaPrefix.size(), package.size() - schemaPrefix.size());
        if (package.empty()) {
            return {};
        }
        return ExtractedPackage{ package.toString(), false /* isPrivate */ };

    } else if (util::stringStartsWith(namespaceUri, kSchemaPrivatePrefix)) {
        StringPiece schemaPrefix = kSchemaPrivatePrefix;
        StringPiece package = namespaceUri;
        package = package.substr(schemaPrefix.size(), package.size() - schemaPrefix.size());
        if (package.empty()) {
            return {};
        }
        return ExtractedPackage{ package.toString(), true /* isPrivate */ };

    } else if (namespaceUri == kSchemaAuto) {
        return ExtractedPackage{ std::string(), true /* isPrivate */ };
    }
    return {};
}

void transformReferenceFromNamespace(IPackageDeclStack* declStack,
                                     const StringPiece& localPackage, Reference* inRef) {
    if (inRef->name) {
        if (Maybe<ExtractedPackage> transformedPackage =
                   declStack->transformPackageAlias(inRef->name.value().package, localPackage)) {
            ExtractedPackage& extractedPackage = transformedPackage.value();
            inRef->name.value().package = std::move(extractedPackage.package);

            // If the reference was already private (with a * prefix) and the namespace is public,
            // we keep the reference private.
            inRef->privateReference |= extractedPackage.privateNamespace;
        }
    }
}

} // namespace xml
} // namespace aapt
