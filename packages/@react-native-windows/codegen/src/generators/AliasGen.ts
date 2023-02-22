/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {
  NativeModuleBaseTypeAnnotation,
  NativeModuleObjectTypeAnnotation,
  NamedShape,
  Nullable,
} from '@react-native/codegen/lib/CodegenSchema';
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
  // retrieve and clean all ungenerated aliases
  const jobs = aliases.jobs;
  aliases.jobs = [];

  // generate each one in its found order
  for (const aliasName of jobs) {
    // generate a new struct and all fields will be examined
    // new anonymous objects could be found
    // they will be stored in aliases.jobs
    generateSingleAlias(aliases, aliasName, aliasCode);
    // nested C++ structs must be put before the current C++ struct
    // as they will be used in the current C++ struct
    // the order will be perfectly and easily ensured by doing this recursively
    generateNestedAliasesInCorrectOrder(aliases, aliasCode, aliasOrder);
    // all referenced C++ structs are generated
    // put the current one following them
    aliasOrder.push(aliasName);
  }
}

export function generateAliases(aliases: AliasMap): string {
  const aliasCode: AliasCodeMap = {};
  const aliasOrder: string[] = [];
  generateNestedAliasesInCorrectOrder(aliases, aliasCode, aliasOrder);

  // aliasOrder now has the correct order of C++ struct code
  let traversedAliasedStructs = '';
  for (const aliasName of aliasOrder) {
    traversedAliasedStructs = `${traversedAliasedStructs}${aliasCode[aliasName]}`;
  }
  return traversedAliasedStructs;
}
