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
import type {CppStringTypes} from './ObjectTypes';

export type {CppStringTypes} from './ObjectTypes';

type FilesOutput = Map<string, string>;

const headerTemplate = `/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once`;

const specTemplate = `::_MODULE_CUSTOM_TYPES_REFLECTION_::
struct ::_MODULE_NAME_::Spec : winrt::Microsoft::ReactNative::TurboModuleSpec {
::_MODULE_MEMBERS_TUPLES_::

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
::_MODULE_MEMBERS_CHECKS_::

::_MODULE_MEMBERS_ERRORS_::
  }`;

const typeOnlyTemplate = `
${headerTemplate}

#include <string>
#include <optional>
#include <functional>
#include <vector>

namespace ::_NAMESPACE_:: {
::_MODULE_CUSTOM_TYPES_::
} // namespace ::_NAMESPACE_::
`;

const moduleOnlyTemplate = `
${headerTemplate}

::_TYPE_DEFINITION_INCLUDE_::
#include <NativeModules.h>
#include <tuple>

namespace ::_NAMESPACE_:: {
${specTemplate}
};

} // namespace ::_NAMESPACE_::
`;

const allInOneTemplate = `
${headerTemplate}

#include <NativeModules.h>
#include <tuple>

namespace ::_NAMESPACE_:: {
::_MODULE_CUSTOM_TYPES_::
${specTemplate}
};

} // namespace ::_NAMESPACE_::
`;

export function createNM2Generator({
  methodOnly,
  namespace,
  cppStringType,
  separateDataTypes,
}: {
  methodOnly: boolean;
  namespace: string;
  cppStringType: CppStringTypes;
  separateDataTypes: boolean;
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
        const methods = generateValidateMethods(nativeModule, aliases, {
          cppStringType,
        });
        let tuples = `
  static constexpr auto methods = std::tuple{
${methods.traversedPropertyTuples}${
          methods.traversedEventEmitterTuples ? '\n' : ''
        }${methods.traversedEventEmitterTuples}
  };`;
        let checks = `
    constexpr auto methodCheckResults = CheckMethods<TModule, ::_MODULE_NAME_::Spec>();`;
        let errors =
          methods.traversedProperties +
          (methods.traversedEventEmitters ? '\n' : '') +
          methods.traversedEventEmitters;

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
        const [customTypes, customReflection] = generateAliases(aliases, {
          cppStringType,
        });

        const customTypesExist = customTypes !== '';

        const replaceContent = function (template: string): string {
          return template
            .replace(/::_MODULE_CUSTOM_TYPES_::/g, customTypes)
            .replace(/::_MODULE_CUSTOM_TYPES_REFLECTION_::/g, customReflection)
            .replace(/::_MODULE_MEMBERS_TUPLES_::/g, tuples.substring(1))
            .replace(/::_MODULE_MEMBERS_CHECKS_::/g, checks.substring(1))
            .replace(/::_MODULE_MEMBERS_ERRORS_::/g, errors)
            .replace(/::_MODULE_NAME_::/g, preferredModuleName)
            .replace(
              /::_TYPE_DEFINITION_INCLUDE_::/g,
              customTypesExist
                ? `// #include "Native${preferredModuleName}DataTypes.g.h" before this file to use the generated type definition`
                : '',
            )
            .replace(/::_NAMESPACE_::/g, namespace);
        };

        if (separateDataTypes) {
          if (customTypesExist) {
            files.set(
              `Native${preferredModuleName}DataTypes.g.h`,
              replaceContent(typeOnlyTemplate),
            );
          }
          files.set(
            `Native${preferredModuleName}Spec.g.h`,
            replaceContent(moduleOnlyTemplate),
          );
        } else {
          files.set(
            `Native${preferredModuleName}Spec.g.h`,
            replaceContent(allInOneTemplate),
          );
        }
      }
    }

    return files;
  };
}
