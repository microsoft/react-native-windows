/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

'use strict';

import {
  SchemaType,
  MethodTypeShape,
  //FunctionTypeAnnotationParam,
  FunctionTypeAnnotationParamTypeAnnotation,
  FunctionTypeAnnotationReturn,
} from 'react-native-tscodegen';

type FilesOutput = Map<string, string>;

const moduleTemplate = `
/*
 * This function registers a class as implementing a TurboModule Spec
 * The class must implement all the methods as required by the Spec.
 * Methods may be implemented using one of the method signatures provided above the RegisterMethod calls
 *
 * Note that methods can be optionally written using a veriety of method signatures which may be preferable
 * depending on your scenario.
 * 
 * An example implementation you could copy/paste is provided here:

class ::_MODULE_NAME_:: {
  void Initialize() {
  }
::_EXAMPLE_MODULE_PROPERTIES_::
};

 */
template <typename TMyModuleImpl>
void Register::_MODULE_NAME_::Module(ReactModuleBuilder<::_MODULE_IMPL_NAME_::> &moduleBuilder) noexcept {
  moduleBuilder.RegisterModuleName(L"::_MODULE_NAME_::");
  moduleBuilder.RegisterInitMethod(&::_MODULE_IMPL_NAME_::::Initialize);
::_MODULE_PROPERTIES_::
}`;

const moduleMethodTemplate = `
  // This method can be implemented using the following method signature:
  // ::_METHOD_RETURN_TYPE_:: ::_MODULE_IMPL_NAME_::::::_METHOD_NAME_::(::_METHOD_ARGS_::) noexcept;
  moduleBuilder.Register::_METHOD_SYNC_::Method/*<::_METHOD_TYPE_::>*/(&::_MODULE_IMPL_NAME_::::::_METHOD_NAME_::, L"::_METHOD_NAME_::");`;

const exampleMethodTemplate = `
  ::_METHOD_RETURN_TYPE_:: ::_MODULE_NAME_::::::_METHOD_NAME_::(::_METHOD_ARGS_::) noexcept {
    throw std::logic_error("::_MODULE_NAME_::::::_METHOD_NAME_:: not yet implemented.");
  }`;

const template = `
/**
 * This file is auto-generated from a TurboModule Spec file
 */

#pragma once

#include "NativeModules.h"

namespace ::_NAMESPACE_:: {
::_MODULES_::

} // namespace ::_NAMESPACE_::
`;

function translatePrimitiveJSTypeToCpp(
  type:    FunctionTypeAnnotationParamTypeAnnotation    | FunctionTypeAnnotationReturn,
  error: string,
) {
  switch (type.type) {
    case 'VoidTypeAnnotation':
      return 'void';
    case 'StringTypeAnnotation':
      return 'std::string';
    case 'NumberTypeAnnotation':
    case 'FloatTypeAnnotation':
      return 'double';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'GenericObjectTypeAnnotation':
    case 'ObjectTypeAnnotation':
      return 'jsi::Object';
/*
    case 'ArrayTypeAnnotation':
      return 'jsi::Array';
    case 'FunctionTypeAnnotation':
      return 'jsi::Function';
    case 'GenericPromiseTypeAnnotation':
      return 'jsi::Value';
*/
    default:
      throw new Error(error);
  }
}

function renderProperties(properties : ReadonlyArray<MethodTypeShape>, example : boolean) : string {
  return properties
  .map(prop => {

    const isMethodSync = prop.typeAnnotation.returnTypeAnnotation.type !== 'VoidTypeAnnotation';

    let params = prop.typeAnnotation.params;

    /*
    let hasCallback = false;
    // If last arg is a callback
    if (prop.typeAnnotation.returnTypeAnnotation.type === 'VoidTypeAnnotation' &&
      params.length && params[params.length - 1].typeAnnotation.type === 'FunctionTypeAnnotation') {
      hasCallback = true;
    }
    */

    const traversedArgs = params
      .map(param => {
        const translatedParam = translatePrimitiveJSTypeToCpp(
          param.typeAnnotation,
          `Unspopported type for param "${param.name}" in ${
            prop.name
          }. Found: ${param.typeAnnotation.type}`,
        );
        return `${translatedParam}`;
        /*
        const isObject = translatedParam.startsWith('jsi::');
        return (
          (isObject
            ? 'const ' + translatedParam + ' &'
            : translatedParam + ' ') + param.name
        );
        */
      })
      .join(', ');

    const translatedReturnParam = translatePrimitiveJSTypeToCpp(
      prop.typeAnnotation.returnTypeAnnotation,
      `Unsupported return type for ${prop.name}. Found: ${
        prop.typeAnnotation.returnTypeAnnotation.type
      }`);

    return (example ? exampleMethodTemplate : moduleMethodTemplate)
      .replace('::_METHOD_SYNC_::', isMethodSync ? 'Sync' : '')
      .replace(/::_METHOD_NAME_::/g, prop.name)
      .replace('::_METHOD_RETURN_TYPE_::', translatedReturnParam)
      .replace('::_METHOD_ARGS_::', traversedArgs)
      .replace('::_METHOD_TYPE_::', `${translatedReturnParam}(${traversedArgs})`);
  })
  .join('\n');
}

export function createNM2Generator({namespace} : {namespace: string}) {
  return (  _libraryName: string,
    schema: SchemaType,
    _moduleSpecName: string) : FilesOutput => {

  const nativeModules = Object.keys(schema.modules)
    .map(moduleName => {
      const modules = schema.modules[moduleName].nativeModules;
      if (modules == null) {
        return null;
      }

      return modules;
    })
    .filter(Boolean)
    .reduce((acc, components) => Object.assign(acc, components), {});


  const modules = Object.keys(nativeModules)
    .map(name => {
      const {properties} = nativeModules[name];
      const traversedProperties = renderProperties(properties, false);
      const traversedExampleProperties = renderProperties(properties, true);


      return moduleTemplate
        .replace(/::_EXAMPLE_MODULE_PROPERTIES_::/g, traversedExampleProperties)
        .replace(/::_MODULE_PROPERTIES_::/g, traversedProperties)
        .replace(/::_MODULE_IMPL_NAME_::/g, `T${name}Impl`)
        .replace(/::_MODULE_NAME_::/g, name);
    })
    .join('\n');

  const fileName = 'NativeModules.g.h';
  const replacedTemplate = template.replace(/::_NAMESPACE_::/g, namespace).replace(/::_MODULES_::/g, modules);

  return new Map([[fileName, replacedTemplate]]);
};
}
