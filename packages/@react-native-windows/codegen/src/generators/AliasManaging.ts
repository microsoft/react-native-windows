/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {NativeModuleObjectTypeAnnotation} from '@react-native/codegen/lib/CodegenSchema';

let preferredModuleName: string = '';

export function setPreferredModuleName(moduleName: string): void {
  preferredModuleName = moduleName;
}

export function getAliasCppName(typeName: string): string {
  return `${preferredModuleName}Spec_${typeName}`;
}

export interface AliasMap {
  types: {[name: string]: NativeModuleObjectTypeAnnotation | undefined};
  jobs: string[];
}

const ExtendedObjectKey = '$RNW-TURBOMODULE-ALIAS';
interface ExtendedObject extends NativeModuleObjectTypeAnnotation {
  '$RNW-TURBOMODULE-ALIAS'?: string;
}

function recordAnonymouseAlias(
  aliases: AliasMap,
  baseAliasName: string,
  extended: ExtendedObject,
): string {
  extended[ExtendedObjectKey] = baseAliasName;
  aliases.types[baseAliasName] = extended;
  aliases.jobs.push(baseAliasName);
  return baseAliasName;
}

export function getAnonymousAliasCppName(
  aliases: AliasMap,
  baseAliasName: string,
  objectType: NativeModuleObjectTypeAnnotation,
): string {
  // someone found an anonymous object literal type
  // if the ExtendedObjectKey flag has been set
  // then it is a known one
  // this happens because method signatures are generate twice in spec and error messages
  const extended = <ExtendedObject>objectType;
  const key = extended[ExtendedObjectKey];
  if (key !== undefined) {
    return getAliasCppName(key);
  }

  // if the ExtendedObjectKey flag has not been set
  // it means it is a unknown one
  // associate the name with this object literal type and return
  if (aliases.types[baseAliasName] === undefined) {
    return getAliasCppName(
      recordAnonymouseAlias(aliases, baseAliasName, extended),
    );
  }

  // sometimes names could be anonymous
  let index = 2;
  while (aliases.types[`${baseAliasName}${index}`] !== undefined) {
    index++;
  }

  return getAliasCppName(
    recordAnonymouseAlias(aliases, `${baseAliasName}${index}`, extended),
  );
}
