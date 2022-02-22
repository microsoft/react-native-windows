/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {SchemaType} from 'react-native-tscodegen';

type FilesOutput = Map<string, string>;

const moduleTemplate = `
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a TypeScript turbo module definition file.
 */

import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import * as TurboModuleRegistry from 'react-native/Libraries/TurboModule/TurboModuleRegistry';
'use strict';

export interface Spec extends TurboModule {
::_MODULE_MEMBERS_::
}

export default TurboModuleRegistry.getEnforcing<Spec>('::_MODULE_NAME_::');
`;

export function createTypeScriptGenerator() {
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

      if (nativeModule.type === 'NativeModule') {
        console.log(`Generating ${preferredModuleName}Spec.g.ts`);

        files.set(
          `${preferredModuleName}Spec.g.ts`,
          moduleTemplate
            .replace(/::_MODULE_MEMBERS_::/g, '')
            .replace(/::_MODULE_NAME_::/g, preferredModuleName),
        );
      }
    }

    return files;
  };
}
