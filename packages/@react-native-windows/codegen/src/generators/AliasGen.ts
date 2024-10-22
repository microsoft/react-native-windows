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
import {CppCodegenOptions, translateField} from './ObjectTypes';

function translateObjectMembersDefinition(
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
      return `${prefix}${translateField(
        propType,
        aliases,
        `${baseAliasName}_${prop.name}`,
        options,
      )} ${prop.name};`;
    })
    .join('\n');
}

function translateObjectMembersReflection(
  type: NativeModuleObjectTypeAnnotation,
  aliasCppName: string,
  prefix: string,
) {
  return type.properties
    .map((prop: NamedShape<Nullable<NativeModuleBaseTypeAnnotation>>) => {
      return `${prefix}{L"${prop.name}", &${aliasCppName}::${prop.name}},`;
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

interface AliasCode {
  definition: string;
  reflection: string;
}

interface AliasCodeMap {
  [name: string]: AliasCode;
}

function getArrayTypeName(
  type: Nullable<NativeModuleBaseTypeAnnotation>,
): string {
  if (
    type.type === 'ArrayTypeAnnotation' &&
    type.elementType.type === 'TypeAliasTypeAnnotation'
  ) {
    return type.elementType.name;
  }

  return '';
}

function checkTypes(
  aliases: AliasMap,
  type: NativeModuleObjectTypeAnnotation,
  aliasOrder: string[],
): void {
  for (const prop of type.properties) {
    const propType = prop.typeAnnotation;
    let propName = '';

    if (propType.type === 'TypeAliasTypeAnnotation') {
      propName = propType.name;
    }

    if (propType.type === 'ArrayTypeAnnotation') {
      propName = getArrayTypeName(propType);
    }

    if (
      propName !== '' &&
      !aliasOrder.includes(prop.name) &&
      !aliases.jobs.includes(propName)
    ) {
      aliases.jobs.push(propName);
    }
  }
}

function generateSingleAlias(
  aliases: AliasMap,
  aliasName: string,
  aliasCode: AliasCodeMap,
  options: CppCodegenOptions,
  aliasOrder: string[],
): void {
  const aliasCppName = getAliasCppName(aliasName);
  const aliasType = <NativeModuleObjectTypeAnnotation>aliases.types[aliasName];

  checkTypes(aliases, aliasType, aliasOrder);

  const definition = `
struct ${aliasCppName} {
${translateObjectMembersDefinition(
  aliasType,
  aliases,
  aliasName,
  '    ',
  options,
)}
};
`;
  const reflection = `
inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(${aliasCppName}*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
${translateObjectMembersReflection(aliasType, aliasCppName, '        ')}
    };
    return fieldMap;
}
`;
  aliasCode[aliasName] = {definition, reflection};
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
    generateSingleAlias(aliases, aliasName, aliasCode, options, aliasOrder);
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
    if (!aliasOrder.includes(aliasName)) {
      aliasOrder.push(aliasName);
    }
  }
}

export function generateAliases(
  aliases: AliasMap,
  options: CppCodegenOptions,
): [string, string] {
  const aliasCode: AliasCodeMap = {};
  const aliasOrder: string[] = [];
  generateNestedAliasesInCorrectOrder(aliases, aliasCode, aliasOrder, options);

  // aliasOrder now has the correct order of C++ struct code
  let customTypes = '';
  let customReflection = '';
  for (const aliasName of aliasOrder) {
    customTypes = `${customTypes}${aliasCode[aliasName].definition}`;
    customReflection = `${customReflection}${aliasCode[aliasName].reflection}`;
  }
  return [customTypes, customReflection];
}
