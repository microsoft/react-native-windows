/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {SchemaType} from '@react-native/codegen/lib/CodegenSchema';
import {AliasMap, setPreferredModuleName} from './AliasManaging';
import {createAliasMap, generateAliases} from './AliasGen';
import {generateValidateConstants} from './ValidateConstants';
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
::_MODULE_MEMBERS_TUPLES_::

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
::_MODULE_MEMBERS_CHECKS_::

::_MODULE_MEMBERS_ERRORS_::
  }
};

} // namespace ::_NAMESPACE_::
`;

export function createNM2Generator({
  methodOnly,
  namespace,
}: {
  methodOnly: boolean;
  namespace: string;
}) {
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
        const aliases: AliasMap = createAliasMap(nativeModule.aliasMap);

        // prepare methods
        const methods = generateValidateMethods(nativeModule, aliases);
        let tuples = `
  static constexpr auto methods = std::tuple{
${methods[0]}
  };`;
        let checks = `
    constexpr auto methodCheckResults = CheckMethods<TModule, ::_MODULE_NAME_::Spec>();`;
        let errors = methods[1];

        // prepare constants
        const constants = generateValidateConstants(nativeModule, aliases);
        if (constants !== undefined && !methodOnly) {
          tuples = `
  static constexpr auto constants = std::tuple{
${constants[0]}
  };${tuples}`;
          checks = `
    constexpr auto constantCheckResults = CheckConstants<TModule, ::_MODULE_NAME_::Spec>();${checks}`;
          errors = `${constants[1]}

${errors}`;
        }

        // generate code for structs
        const traversedAliasedStructs = generateAliases(aliases);

        files.set(
          `Native${preferredModuleName}Spec.g.h`,
          moduleTemplate
            .replace(/::_MODULE_ALIASED_STRUCTS_::/g, traversedAliasedStructs)
            .replace(/::_MODULE_MEMBERS_TUPLES_::/g, tuples.substring(1))
            .replace(/::_MODULE_MEMBERS_CHECKS_::/g, checks.substring(1))
            .replace(/::_MODULE_MEMBERS_ERRORS_::/g, errors)
            .replace(/::_MODULE_NAME_::/g, preferredModuleName)
            .replace(/::_NAMESPACE_::/g, namespace),
        );
      }
    }

    return files;
  };
}
