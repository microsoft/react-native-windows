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
  const aliases: AliasMap = {};
  for (const aliasName of Object.keys(nativeModuleAliases)) {
    aliases[aliasName] = nativeModuleAliases[aliasName];
  }
  return aliases;
}

export function generateAliases(aliases: AliasMap): string {
  const aliasOrder: string[] = [];
  const aliasCode: {[name: string]: string} = {};
  const aliasJobs: string[] = Object.keys(aliases);

  while (aliasJobs.length > 0) {
    const aliasName = <string>aliasJobs.shift();
    const aliasType = <NativeModuleObjectTypeAnnotation>aliases[aliasName];
    aliasCode[aliasName] = `
REACT_STRUCT(${getAliasCppName(aliasName)})
struct ${getAliasCppName(aliasName)} {
${translateObjectBody(aliasType, aliases, aliasName, '    ')}
};
`;
    aliasOrder.push(aliasName);
  }

  let traversedAliasedStructs = '';
  for (const aliasName of aliasOrder) {
    traversedAliasedStructs = `${traversedAliasedStructs}${aliasCode[aliasName]}`;
  }
  return traversedAliasedStructs;
}
