/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {NativeModuleObjectTypeAnnotation} from 'react-native-tscodegen';

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
  const extended = <ExtendedObject>objectType;
  const key = extended[ExtendedObjectKey];
  if (key !== undefined) {
    return getAliasCppName(key);
  }

  if (aliases.types[baseAliasName] === undefined) {
    return getAliasCppName(
      recordAnonymouseAlias(aliases, baseAliasName, extended),
    );
  }

  let index = 2;
  while (aliases.types[`${baseAliasName}${index}`] !== undefined) {
    index++;
  }

  return getAliasCppName(
    recordAnonymouseAlias(aliases, `${baseAliasName}${index}`, extended),
  );
}
