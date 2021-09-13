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
  [name: string]: NativeModuleObjectTypeAnnotation | undefined;
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
  aliases[baseAliasName] = extended;
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

  if (aliases[baseAliasName] === undefined) {
    return getAliasCppName(
      recordAnonymouseAlias(aliases, baseAliasName, extended),
    );
  }

  let index = 2;
  while (aliases[`${baseAliasName}${index}`] !== undefined) {
    index++;
  }

  return getAliasCppName(
    recordAnonymouseAlias(aliases, `${baseAliasName}${index}`, extended),
  );
}
