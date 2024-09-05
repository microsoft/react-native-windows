import type { PropTypeAnnotation, ObjectTypeAnnotation, EventTypeAnnotation, CommandParamTypeAnnotation } from '@react-native/codegen/lib/CodegenSchema';
import type { CppCodegenOptions } from './ObjectTypes';
import {
  AliasMap,
  getAnonymousAliasCppName,
} from './AliasManaging';


// eslint-disable-next-line complexity
export function translateComponentPropsFieldType(type: PropTypeAnnotation,
  aliases: AliasMap<ObjectTypeAnnotation<PropTypeAnnotation>>,
  baseAliasName: string,
  options: CppCodegenOptions): { type: string, initializer: string, alreadySupportsOptionalOrHasDefault?: boolean } {
  switch (type.type) {
    case 'StringTypeAnnotation':
      return { type: options.cppStringType, initializer: type.default ? `{${type.default}}` : '', alreadySupportsOptionalOrHasDefault: !!type.default };
    case 'FloatTypeAnnotation':
      return { type: 'float', initializer: type.default ? `{${type.default}}` : '{}', alreadySupportsOptionalOrHasDefault: !!type.default };
    case 'DoubleTypeAnnotation':
      return { type: 'double', initializer: type.default ? `{${type.default}}` : '{}', alreadySupportsOptionalOrHasDefault: !!type.default };
    case 'Int32TypeAnnotation':
      return { type: 'int32_t', initializer: type.default ? `{${type.default}}` : '{}', alreadySupportsOptionalOrHasDefault: !!type.default };
    case 'BooleanTypeAnnotation':
      return { type: 'bool', initializer: type.default ? `{${type.default}}` : '{}', alreadySupportsOptionalOrHasDefault: !!type.default };
    case 'ArrayTypeAnnotation':

      let arrayTemplateArg = '';
      switch (type.elementType.type) {
        case 'BooleanTypeAnnotation':
          arrayTemplateArg = 'bool';
          break;
        case 'DoubleTypeAnnotation':
          arrayTemplateArg = 'double';
          break;
        case 'FloatTypeAnnotation':
          arrayTemplateArg = 'float';
          break;
        case 'Int32TypeAnnotation':
          arrayTemplateArg = 'int32_t';
          break;
        case 'StringTypeAnnotation':
          arrayTemplateArg = options.cppStringType;
          break;
        case 'ArrayTypeAnnotation':
          const innerType = translateComponentPropsFieldType(type.elementType, aliases, baseAliasName, options);
          arrayTemplateArg = `std::vector<${innerType.type}>`;
          break;
        case 'ObjectTypeAnnotation':
          arrayTemplateArg = translateComponentPropsFieldType(type.elementType, aliases, baseAliasName, options).type;
          break;
        case 'ReservedPropTypeAnnotation':
          switch (type.elementType.name) {
            case 'ColorPrimitive':
              arrayTemplateArg = 'winrt::Microsoft::ReactNative::Color';
              break;
            case 'DimensionPrimitive':
            case 'EdgeInsetsPrimitive':
            case 'ImageRequestPrimitive':
            case 'ImageSourcePrimitive':
            case 'PointPrimitive':
              arrayTemplateArg = 'winrt::Microsoft::ReactNative::JSValue'; // TODO - better handling for these types than JSValue
              break;
            default:
              throw new Error(`Unhandled ReservedPropTypeAnnotation type: ${type.elementType.name}`);
          }
          break;
        case 'StringEnumTypeAnnotation':
          arrayTemplateArg = options.cppStringType; // TODO - better enum type handling than just passing a string
          break;
        default:
          throw new Error(`Unhandled type: ${type.type}`);
      }

      return { type: `std::vector<${arrayTemplateArg}>`, initializer: '' };
    case 'ReservedPropTypeAnnotation':
      switch (type.name) {
        case 'ColorPrimitive':
          return { type: 'winrt::Microsoft::ReactNative::Color', initializer: '{nullptr}', alreadySupportsOptionalOrHasDefault: true };
        case 'DimensionPrimitive':
        case 'EdgeInsetsPrimitive':
        case 'ImageRequestPrimitive':
        case 'ImageSourcePrimitive':
        case 'PointPrimitive':
          return { type: 'winrt::Microsoft::ReactNative::JSValue', initializer: '{nullptr}', alreadySupportsOptionalOrHasDefault: true }; // TODO - better handling for these types than JSValue
        default:
          throw new Error(`Unhandled ReservedPropTypeAnnotation type: ${type.name}`);
      }
    case 'ObjectTypeAnnotation': {
      return { type: getAnonymousAliasCppName<ObjectTypeAnnotation<PropTypeAnnotation>>(aliases, baseAliasName, type), initializer: '' };
    }
    case 'MixedTypeAnnotation':
      return { type: 'winrt::Microsoft::ReactNative::JSValue', initializer: '{nullptr}', alreadySupportsOptionalOrHasDefault: true };
    case 'Int32EnumTypeAnnotation':
      return { type: 'int32_t', initializer: '' }; // TODO - better enum type handling than just passing a string
    case 'StringEnumTypeAnnotation':
      return { type: options.cppStringType, initializer: '' }; // TODO - better enum type handling than just passing an int
    default:
      throw new Error(`Unhandled type: ${(type as any).type}`);
  }
}

export function translateComponentEventType(type: EventTypeAnnotation,
  aliases: AliasMap<ObjectTypeAnnotation<EventTypeAnnotation>>,
  baseAliasName: string,
  options: CppCodegenOptions): { type: string, initializer: string, alreadySupportsOptionalOrHasDefault?: boolean } {
  switch (type.type) {
    case 'StringTypeAnnotation':
      return { type: options.cppStringType, initializer: '' };
    case 'FloatTypeAnnotation':
      return { type: 'float', initializer: '{}' };
    case 'DoubleTypeAnnotation':
      return { type: 'double', initializer: '{}' };
    case 'Int32TypeAnnotation':
      return { type: 'int32_t', initializer: '{}' };
    case 'BooleanTypeAnnotation':
      return { type: 'bool', initializer: '{}' };
    case 'ArrayTypeAnnotation':
      {
        let arrayTemplateArg = '';
        switch (type.elementType.type) {
          case 'BooleanTypeAnnotation':
            arrayTemplateArg = 'bool';
            break;
          case 'DoubleTypeAnnotation':
            arrayTemplateArg = 'double';
            break;
          case 'FloatTypeAnnotation':
            arrayTemplateArg = 'float';
            break;
          case 'Int32TypeAnnotation':
            arrayTemplateArg = 'int32_t';
            break;
          case 'StringTypeAnnotation':
            arrayTemplateArg = options.cppStringType;
            break;
          case 'ArrayTypeAnnotation':
            const innerType = translateComponentEventType(type.elementType, aliases, baseAliasName, options);
            arrayTemplateArg = `std::vector<${innerType.type}>`;
            break;
          case 'MixedTypeAnnotation':
            arrayTemplateArg = 'winrt::Microsoft::ReactNative::JSValue';
            break;
          case 'ObjectTypeAnnotation':
            arrayTemplateArg = translateComponentEventType(type.elementType, aliases, baseAliasName, options).type;
            break;
          case 'StringEnumTypeAnnotation':
            arrayTemplateArg = options.cppStringType; // TODO - better enum type handling than just passing a string
            break;
          default:
            throw new Error(`Unhandled type: ${type.type}`);
        }

        return { type: `std::vector<${arrayTemplateArg}>`, initializer: '{}' };
      }
    case 'ObjectTypeAnnotation': {
      return { type: getAnonymousAliasCppName<ObjectTypeAnnotation<EventTypeAnnotation>>(aliases, baseAliasName, type), initializer: '' };
    }
    case 'MixedTypeAnnotation': {
      return { type: 'winrt::Microsoft::ReactNative::JSValue', initializer: '{nullptr}', alreadySupportsOptionalOrHasDefault: true };
    }
    case 'StringEnumTypeAnnotation':
      return { type: options.cppStringType, initializer: '' };  // TODO - better enum type handling than just passing a string
    default:
      throw new Error(`Unhandled type: ${(type as any).type}`);
  }
}


export function translateCommandParamType(type: CommandParamTypeAnnotation,
  aliases: AliasMap<ObjectTypeAnnotation<CommandParamTypeAnnotation>>,
  baseAliasName: string,
  options: CppCodegenOptions): { type: string, initializer: string, alreadySupportsOptionalOrHasDefault?: boolean } {
  switch (type.type) {
    case 'StringTypeAnnotation':
      return { type: options.cppStringType, initializer: '' };
    case 'FloatTypeAnnotation':
      return { type: 'float', initializer: '{}' };
    case 'DoubleTypeAnnotation':
      return { type: 'double', initializer: '{}' };
    case 'Int32TypeAnnotation':
      return { type: 'int32_t', initializer: '{}' };
    case 'BooleanTypeAnnotation':
      return { type: 'bool', initializer: '{}' };
    case 'ArrayTypeAnnotation':
      {
        let arrayTemplateArg = '';
        switch (type.elementType.type) {
          case 'BooleanTypeAnnotation':
            arrayTemplateArg = 'bool';
            break;
          case 'DoubleTypeAnnotation':
            arrayTemplateArg = 'double';
            break;
          case 'FloatTypeAnnotation':
            arrayTemplateArg = 'float';
            break;
          case 'Int32TypeAnnotation':
            arrayTemplateArg = 'int32_t';
            break;
          case 'StringTypeAnnotation':
            arrayTemplateArg = options.cppStringType;
            break;
          case 'ArrayTypeAnnotation':
            const innerType = translateCommandParamType(type.elementType, aliases, baseAliasName, options);
            arrayTemplateArg = `std::vector<${innerType.type}>`;
            break;
          case 'ReservedPropTypeAnnotation':
            arrayTemplateArg = 'winrt::Microsoft::ReactNative::JSValue';
            break;
          case 'ObjectTypeAnnotation':
            arrayTemplateArg = 'winrt::Microsoft::ReactNative::JSValueObject'; // TODO - better typing
            break;
          case 'StringEnumTypeAnnotation':
            arrayTemplateArg = options.cppStringType; // TODO - better enum type handling than just passing a string
            break;
          case 'GenericTypeAnnotation' as any: // TODO verify schema - Getting this type when running codegen on all the built in types
            arrayTemplateArg = 'winrt::Microsoft::ReactNative::JSValue';
            break;
          default:
            throw new Error(`Unhandled type: ${(type.elementType as any).type} - ${JSON.stringify(type.elementType, null, 2)}`);
        }

        return { type: `std::vector<${arrayTemplateArg}>`, initializer: '{}' };
      }
    case 'ReservedTypeAnnotation': 
      if ((type.name as any) !== 'RootTag') {
        throw new Error(`Unhandled ReservedTypeAnnotation: ${type.name}`)
      }
      return { type: 'bool', initializer: '{-1}' };
    default:
      throw new Error(`Unhandled type: ${(type as any).type}`);
  }
}