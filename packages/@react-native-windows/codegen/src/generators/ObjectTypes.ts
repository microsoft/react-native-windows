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

function translateField(
  type: Nullable<NativeModuleBaseTypeAnnotation>,
  aliases: AliasMap,
  baseAliasName: string,
): string {
  // avoid: Property 'type' does not exist on type 'never'
  const returnType = type.type;
  switch (type.type) {
    case 'StringTypeAnnotation':
      return 'std::string';
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
    case 'DoubleTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'ArrayTypeAnnotation':
      if (type.elementType) {
        return `std::vector<${translateField(
          type.elementType,
          aliases,
          `${baseAliasName}_element`,
        )}>`;
      } else {
        return `React::JSValueArray`;
      }
    case 'GenericObjectTypeAnnotation':
      return 'React::JSValue';
    case 'ObjectTypeAnnotation':
      return getAnonymousAliasCppName(aliases, baseAliasName, type);
    case 'ReservedTypeAnnotation': {
      // avoid: Property 'name' does not exist on type 'never'
      const name = type.name;
      // (#6597)
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (name !== 'RootTag')
        throw new Error(
          `Unknown reserved function: ${name} in translateReturnType`,
        );
      return 'double';
    }
    case 'TypeAliasTypeAnnotation':
      return getAliasCppName(type.name);
    case 'NullableTypeAnnotation':
      return `std::optional<${translateField(
        type.typeAnnotation,
        aliases,
        baseAliasName,
      )}>`;
    default:
      throw new Error(`Unhandled type in translateReturnType: ${returnType}`);
  }
}

export function translateObjectBody(
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
