import type {PropTypeAnnotation, ObjectTypeAnnotation, EventTypeAnnotation} from '@react-native/codegen/lib/CodegenSchema';
import type {CppCodegenOptions} from './ObjectTypes';
import {
    AliasMap,
    getAnonymousAliasCppName,
  } from './AliasManaging';
 

export function translateComponentPropsFieldType(type: PropTypeAnnotation,
    aliases: AliasMap<ObjectTypeAnnotation<PropTypeAnnotation>>,
    baseAliasName: string,
    options: CppCodegenOptions) : {type: string, initializer: string, alreadySupportsOptionalOrHasDefault?: boolean} {
      switch (type.type) {
        case 'StringTypeAnnotation':
          return {type: options.cppStringType, initializer:type.default ? `{${type.default}}` : '', alreadySupportsOptionalOrHasDefault: !!type.default};
        case 'FloatTypeAnnotation':
          return {type: 'float', initializer:type.default ? `{${type.default}}` : '{}', alreadySupportsOptionalOrHasDefault: !!type.default};
        case 'DoubleTypeAnnotation':
          return {type: 'double', initializer:type.default ? `{${type.default}}` : '{}', alreadySupportsOptionalOrHasDefault: !!type.default};
        case 'Int32TypeAnnotation':
          return {type: 'int32_t', initializer:type.default ? `{${type.default}}` : '{}', alreadySupportsOptionalOrHasDefault: !!type.default};
        case 'BooleanTypeAnnotation':
          return {type: 'bool', initializer:type.default ? `{${type.default}}` : '{}', alreadySupportsOptionalOrHasDefault: !!type.default};
        case 'ArrayTypeAnnotation':
    
          let arrayTemplateArg = '';
          switch(type.elementType.type) {
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
            // TODO
            // case 'ObjectTypeAnnotation':
            // case 'ReservedPropTypeAnnotation':
            // case 'StringEnumTypeAnnotation':
            default:
              throw new Error(`Unhandled type: ${type.type}`);
          }
    
          return {type:`std::vector<${arrayTemplateArg}>`, initializer: ''};
        case 'ReservedPropTypeAnnotation': {
           
          // TODO
          /*
          | 'ImageSourcePrimitive'
          | 'PointPrimitive'
          | 'EdgeInsetsPrimitive'
          | 'ImageRequestPrimitive'
          | 'DimensionPrimitive';
          */
    
          if (type.name === 'ColorPrimitive')
            return {type: 'winrt::Microsoft::ReactNative::Color', initializer:'{nullptr}', alreadySupportsOptionalOrHasDefault: true};
          else
            throw new Error(`Unknown reserved function: ${name}`);
        }
        case 'ObjectTypeAnnotation': {
            return {type: getAnonymousAliasCppName<ObjectTypeAnnotation<PropTypeAnnotation>>(aliases, baseAliasName, type), initializer:''};
        }
        // TODO
        // StringEnumTypeAnnotation
        // Int32EnumTypeAnnotation
        // MixedTypeAnnotation
        default:
          throw new Error(`Unhandled type: ${type.type}`);
      }
    }


    
export function translateComponentEventType(type: EventTypeAnnotation,
    aliases: AliasMap<ObjectTypeAnnotation<EventTypeAnnotation>>,
    baseAliasName: string,
    options: CppCodegenOptions) : {type: string, initializer: string, alreadySupportsOptionalOrHasDefault?: boolean} {
      switch (type.type) {
        case 'StringTypeAnnotation':
          return {type: options.cppStringType, initializer:''};
        case 'FloatTypeAnnotation':
          return {type: 'float', initializer:'{}'};
        case 'DoubleTypeAnnotation':
          return {type: 'double', initializer:'{}'};
        case 'Int32TypeAnnotation':
          return {type: 'int32_t', initializer:'{}'};
        case 'BooleanTypeAnnotation':
          return {type: 'bool', initializer:'{}'};
        case 'ArrayTypeAnnotation':
        {
          let arrayTemplateArg = '';
          switch(type.elementType.type) {
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
            // TODO
            // case 'ObjectTypeAnnotation':
            // case 'ReservedPropTypeAnnotation':
            // case 'StringEnumTypeAnnotation':
            default:
              throw new Error(`Unhandled type: ${type.type}`);
          }
    
          return {type:`std::vector<${arrayTemplateArg}>`, initializer: '{}'};
        }
        case 'ObjectTypeAnnotation': {
            return {type: getAnonymousAliasCppName<ObjectTypeAnnotation<EventTypeAnnotation>>(aliases, baseAliasName, type), initializer:''};
        }
        // TODO
        // StringEnumTypeAnnotation
        // Int32EnumTypeAnnotation
        // MixedTypeAnnotation
        default:
          throw new Error(`Unhandled type: ${type.type}`);
      }
    }