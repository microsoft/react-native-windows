/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {SchemaType} from 'react-native-tscodegen';
import {AliasMap, setPreferredModuleName} from './AliasManaging';
import {createAliasMap, generateAliases} from './AliasGen';
import {generateValidateMethods} from './ValidateMethods';

type FilesOutput = Map<string, string>;

const moduleTemplate = `
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include "NativeModules.h"
#include <tuple>

namespace ::_NAMESPACE_:: {
::_MODULE_ALIASED_STRUCTS_::
struct ::_MODULE_NAME_::Spec : winrt::Microsoft::ReactNative::TurboModuleSpec {
::_MODULE_MEMBERS_VALIDATION_::
  }
};

} // namespace ::_NAMESPACE_::
`;

export function createNM2Generator({namespace}: {namespace: string}) {
  return (
    _libraryName: string,
    schema: SchemaType,
    _moduleSpecName: string,
  ): FilesOutput => {
    const files = new Map<string, string>();

    for (const moduleName of Object.keys(schema.modules)) {
      const nativeModule = schema.modules[moduleName];
      // from 0.65 facebook's react-native-codegen
      // the module name has the Native prefix comparing to 0.63
      // when reading files we provided
      const preferredModuleName = moduleName.startsWith('Native')
        ? moduleName.substr(6)
        : moduleName;
      setPreferredModuleName(preferredModuleName);

      if (nativeModule.type === 'NativeModule') {
        console.log(`Generating Native${preferredModuleName}Spec.g.h`);

        // copy all explicit to a map
        const aliases: AliasMap = createAliasMap(nativeModule.aliases);

        // prepare methods
        const methods = generateValidateMethods(nativeModule, aliases);

        // generate code for structs
        const traversedAliasedStructs = generateAliases(aliases);

        files.set(
          `Native${preferredModuleName}Spec.g.h`,
          moduleTemplate
            .replace(/::_MODULE_ALIASED_STRUCTS_::/g, traversedAliasedStructs)
            .replace(/::_MODULE_MEMBERS_VALIDATION_::/g, methods)
            .replace(/::_MODULE_NAME_::/g, preferredModuleName)
            .replace(/::_NAMESPACE_::/g, namespace),
        );
      }
    }

    return files;
  };
}
