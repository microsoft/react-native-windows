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

function translateField(
  type: Nullable<NativeModuleBaseTypeAnnotation>,
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
      // TODO: type.elementType
      return 'React::JSValueArray';
    case 'GenericObjectTypeAnnotation':
      return 'React::JSValue';
    case 'ObjectTypeAnnotation':
      // TODO: we have more information here, and could create a more specific type
      return 'React::JSValueObject';
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
      return `std::optional<${translateField(type.typeAnnotation)}>`;
    default:
      throw new Error(`Unhandled type in translateReturnType: ${returnType}`);
  }
}

export function translateObjectBody(
  type: NativeModuleObjectTypeAnnotation,
  prefix: string,
) {
  return type.properties
    .map((prop: NamedShape<Nullable<NativeModuleBaseTypeAnnotation>>) => {
      let propType = prop.typeAnnotation;
      if (prop.optional && propType.type !== 'NullableTypeAnnotation') {
        propType = {type: 'NullableTypeAnnotation', typeAnnotation: propType};
      }
      const first = `${prefix}REACT_FIELD(${prop.name})`;
      const second = `${prefix}${translateField(propType)} ${prop.name};`;
      return `${first}\n${second}`;
    })
    .join('\n');
}
