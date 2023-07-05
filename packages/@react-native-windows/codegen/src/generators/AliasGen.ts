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
} from 'react-native-tscodegen';
import {AliasMap, getAliasCppName} from './AliasManaging';
import {CppCodegenOptions, translateField} from './ObjectTypes';

function translateObjectBody(
  type: NativeModuleObjectTypeAnnotation,
  aliases: AliasMap,
  baseAliasName: string,
  prefix: string,
  options: CppCodegenOptions,
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
        options,
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
  options: CppCodegenOptions,
): void {
  const aliasType = <NativeModuleObjectTypeAnnotation>aliases.types[aliasName];
  aliasCode[aliasName] = `
REACT_STRUCT(${getAliasCppName(aliasName)})
struct ${getAliasCppName(aliasName)} {
${translateObjectBody(aliasType, aliases, aliasName, '    ', options)}
};
`;
}

function generateNestedAliasesInCorrectOrder(
  aliases: AliasMap,
  aliasCode: AliasCodeMap,
  aliasOrder: string[],
  options: CppCodegenOptions,
): void {
  // retrieve and clean all ungenerated aliases
  const jobs = aliases.jobs;
  aliases.jobs = [];

  // generate each one in its found order
  for (const aliasName of jobs) {
    // generate a new struct and all fields will be examined
    // new anonymous objects could be found
    // they will be stored in aliases.jobs
    generateSingleAlias(aliases, aliasName, aliasCode, options);
    // nested C++ structs must be put before the current C++ struct
    // as they will be used in the current C++ struct
    // the order will be perfectly and easily ensured by doing this recursively
    generateNestedAliasesInCorrectOrder(
      aliases,
      aliasCode,
      aliasOrder,
      options,
    );
    // all referenced C++ structs are generated
    // put the current one following them
    aliasOrder.push(aliasName);
  }
}

export function generateAliases(
  aliases: AliasMap,
  options: CppCodegenOptions,
): string {
  const aliasCode: AliasCodeMap = {};
  const aliasOrder: string[] = [];
  generateNestedAliasesInCorrectOrder(aliases, aliasCode, aliasOrder, options);

  // aliasOrder now has the correct order of C++ struct code
  let traversedAliasedStructs = '';
  for (const aliasName of aliasOrder) {
    traversedAliasedStructs = `${traversedAliasedStructs}${aliasCode[aliasName]}`;
  }
  return traversedAliasedStructs;
}
