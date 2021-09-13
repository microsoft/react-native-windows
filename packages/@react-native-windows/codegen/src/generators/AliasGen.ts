/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {
  NativeModuleBaseTypeAnnotation,
  NativeModuleObjectTypeAnnotation,
  NamedShape,
  Nullable,
} from 'react-native-tscodegen';
import {AliasMap, getAliasCppName} from './AliasManaging';
import {translateField} from './ObjectTypes';

function translateObjectBody(
  type: NativeModuleObjectTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  prefix: string,
) {
  return type.properties
    .map((prop: NamedShape<Nullable<NativeModuleBaseTypeAnnotation>>) => {
      let propType = prop.typeAnnotation;
      if (prop.optional && propType.type !== 'NullableTypeAnnotation') {
        propType = {type: 'NullableTypeAnnotation', typeAnnotation: propType};
      }
      const first = `${prefix}REACT_FIELD(${prop.name})`;
      const second = `${prefix}${translateField(
        propType,
        aliases,
        `${baseAliasName}_${prop.name}`,
      )} ${prop.name};`;
      return `${first}\n${second}`;
    })
    .join('\n');
}

export function createAliasMap(nativeModuleAliases: {
  [name: string]: NativeModuleObjectTypeAnnotation;
}): AliasMap {
  const aliases: AliasMap = {types: {}, jobs: Object.keys(nativeModuleAliases)};
  for (const aliasName of aliases.jobs) {
    aliases.types[aliasName] = nativeModuleAliases[aliasName];
  }
  return aliases;
}

interface AliasCodeMap {
  [name: string]: string;
}

function generateSingleAlias(
  aliases: AliasMap,
  aliasName: string,
  aliasCode: AliasCodeMap,
): void {
  const aliasType = <NativeModuleObjectTypeAnnotation>aliases.types[aliasName];
  aliasCode[aliasName] = `
REACT_STRUCT(${getAliasCppName(aliasName)})
struct ${getAliasCppName(aliasName)} {
${translateObjectBody(aliasType, aliases, aliasName, '    ')}
};
`;
}

function generateNestedAliasesInCorrectOrder(
  aliases: AliasMap,
  aliasCode: AliasCodeMap,
  aliasOrder: string[],
): void {
  const jobs = aliases.jobs;
  aliases.jobs = [];
  for (const aliasName of jobs) {
    generateSingleAlias(aliases, aliasName, aliasCode);
    generateNestedAliasesInCorrectOrder(aliases, aliasCode, aliasOrder);
    aliasOrder.push(aliasName);
  }
}

export function generateAliases(aliases: AliasMap): string {
  const aliasCode: AliasCodeMap = {};
  const aliasOrder: string[] = [];
  generateNestedAliasesInCorrectOrder(aliases, aliasCode, aliasOrder);

  let traversedAliasedStructs = '';
  for (const aliasName of aliasOrder) {
    traversedAliasedStructs = `${traversedAliasedStructs}${aliasCode[aliasName]}`;
  }
  return traversedAliasedStructs;
}
