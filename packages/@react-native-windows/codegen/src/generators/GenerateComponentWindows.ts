/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {SchemaType, EventTypeAnnotation, PropTypeAnnotation, ObjectTypeAnnotation} from '@react-native/codegen/lib/CodegenSchema';
import {getAliasCppName, setPreferredModuleName} from './AliasManaging';
import {translateComponentPropsFieldType, translateComponentEventType} from './PropObjectTypes';
import type {CppStringTypes} from './ObjectTypes';
import type {AliasMap} from './AliasManaging';

export type {CppStringTypes} from './ObjectTypes';

type FilesOutput = Map<string, string>;

const headerTemplate = `/*
 * This file is auto-generated from a NativeComponent spec file in js.
 */
#pragma once

#include "NativeComponents.h"`;

// @ts-ignore
 
const propsTemplate = `REACT_STRUCT(::_PROPS_NAME_::)
struct ::_PROPS_NAME_:: : winrt::implements<::_PROPS_NAME_::, winrt::Microsoft::ReactNative::IComponentProps> {
  ::_PROPS_NAME_::(winrt::Microsoft::ReactNative::ViewProps props) : m_props(props) {}
  
  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }
  
::_PROPS_FIELDS_::
  winrt::Microsoft::ReactNative::ViewProps m_props;
};`

const propsObjectTemplate = `REACT_STRUCT(::_OBJECT_NAME_::)
struct ::_OBJECT_NAME_:: {
::_OBJECT_FIELDS_::};
`
const eventsObjectTemplate = `REACT_STRUCT(::_OBJECT_NAME_::)
struct ::_OBJECT_NAME_:: {
::_OBJECT_FIELDS_::};
`

const eventEmitterMethodTemplate = `  void ::_EVENT_NAME_::(OnSomething &value) const {
    m_eventEmitter.DispatchEvent(L"::_EVENT_NAME_NO_ON_::", [value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }`;


const eventEmitterTemplate = `::_COMPONENT_EVENT_OBJECT_TYPES_::

struct ::_EVENT_EMITTER_NAME_:: {
  ::_EVENT_EMITTER_NAME_::(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

::_EVENT_EMITTER_USINGS_::
::_EVENT_EMITTER_METHODS_::

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};`
 
const registerTemplate = `
template <typename TUserData, typename TProps = ::_PROPS_NAME_::, typename TEventEmitter = ::_EVENT_EMITTER_NAME_::>
void Register::_COMPONENT_NAME_::NativeComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder, std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"::_COMPONENT_NAME_::", [builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps(
            [](winrt::Microsoft::ReactNative::ViewProps props) noexcept { return winrt::make<TProps>(props); });

        RegisterUpdatePropsHandler<TUserData, TProps>(builder);
        RegisterUpdateEventEmitterHandler<TUserData, TEventEmitter>(builder);
        RegisterFinalizeUpdateHandler<TUserData>(builder);
        RegisterComponentInitializer<TUserData>(builder);
        RegisterComponentCreateVisual<TUserData>(builder);

        // Allow app to further customize the builder
        if (builderCallback) {
          builderCallback(compBuilder);
        }
      });
}
`;

const fileTemplate = `
${headerTemplate}

namespace ::_NAMESPACE_:: {

::_COMPONENT_PROP_OBJECT_TYPES_::
::_COMPONENT_PROP_TYPES_::

::_COMPONENT_EVENT_EMITTER_::

::_COMPONENT_REGISTRATION_::
} // namespace ::_NAMESPACE_::
`;

export function createComponentGenerator({
  namespace,
  cppStringType,
}: {
  namespace: string;
  cppStringType: CppStringTypes;
}) {
  return (
    _libraryName: string,
    schema: SchemaType,
    _moduleSpecName: string,
  ): FilesOutput => {
    const files = new Map<string, string>();

    const cppCodegenOptions = {cppStringType};

    for (const componentName of Object.keys(schema.modules)) {
      const component = schema.modules[componentName];
      setPreferredModuleName(componentName);

      if (component.type === 'Component') {
        console.log(`Generating ${componentName}.g.h`);

        const componentShape = component.components[componentName];

        componentShape.extendsProps.forEach(propsBaseType => {
          // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
          if (propsBaseType.type !== 'ReactNativeBuiltInType' || propsBaseType.knownTypeName !== 'ReactNativeCoreViewProps') {
            throw new Error('Currently only supports props extending from ViewProps');
          }
        });

        // Props
        const propObjectAliases: AliasMap<ObjectTypeAnnotation<PropTypeAnnotation>> = {types:{}, jobs: []};
        const propsName = `${componentName}Props`;
        const propsFields = componentShape.props.map(prop => {
          const propType = translateComponentPropsFieldType(prop.typeAnnotation, propObjectAliases, `${propsName}_${prop.name}`, cppCodegenOptions);
          return `  REACT_FIELD(${prop.name})\n  ${(prop.optional && !propType.alreadySupportsOptionalOrHasDefault) ? `std::optional<${propType.type}>` : propType.type} ${prop.name}${propType.initializer};\n`;
        }).join('\n');

        const propObjectTypes = propObjectAliases.jobs.map(propObjectTypeName => {
          const propObjectType = propObjectAliases.types[propObjectTypeName]!;
          const propsObjectFields = propObjectType.properties.map(property => {
            const propType = translateComponentPropsFieldType(property.typeAnnotation, propObjectAliases, `${propsName}_${property.name}`, cppCodegenOptions);
            return `  REACT_FIELD(${property.name})\n  ${(property.optional && !propType.alreadySupportsOptionalOrHasDefault) ? `std::optional<${propType.type}>` : propType.type} ${property.name}${propType.initializer};\n`;
          }).join('\n');

          return propsObjectTemplate.replace(/::_OBJECT_NAME_::/g, getAliasCppName(propObjectTypeName)).replace(/::_OBJECT_FIELDS_::/g, propsObjectFields);
        }).join('\n');


        // Events
        const eventObjectAliases: AliasMap<ObjectTypeAnnotation<EventTypeAnnotation>> = {types:{}, jobs: []};
        const eventEmitterName = `${componentName}EventEmitter`;
        const eventEmitterMethods = componentShape.events.filter(event => event.typeAnnotation.argument).map(event => {
          if (event.typeAnnotation.argument?.baseTypes) {
            throw new Error('Events with base type arguments not currently supported');
          }

          // Called to collect the eventObjectAliases
          translateComponentEventType(event.typeAnnotation.argument!, eventObjectAliases, `${event.name}`, cppCodegenOptions);

          // onSomething -> something
          let eventNameLower = event.name.replace('on', '');
          eventNameLower = eventNameLower[0].toLowerCase() + eventNameLower.slice(1);

          return eventEmitterMethodTemplate.replace(/::_EVENT_NAME_::/g,event.name).replace(/::_EVENT_NAME_NO_ON_::/g,eventNameLower);
        }).join('\n');
        
        const eventObjects = eventObjectAliases.jobs.map(eventObjectTypeName => {
          const eventObjectType = eventObjectAliases.types[eventObjectTypeName]!;
          const eventObjectFields = eventObjectType.properties.map(property => {
            const eventPropType = translateComponentEventType(property.typeAnnotation, eventObjectAliases, eventObjectTypeName, cppCodegenOptions);
            return `  REACT_FIELD(${property.name})\n  ${(property.optional && !eventPropType.alreadySupportsOptionalOrHasDefault) ? `std::optional<${eventPropType.type}>` : eventPropType.type} ${property.name}${eventPropType.initializer};\n`;
          }).join('\n');
          return eventsObjectTemplate.replace(/::_OBJECT_NAME_::/g, `${componentName}_${eventObjectTypeName.replace('on', 'On')}`).replace(/::_OBJECT_FIELDS_::/g, eventObjectFields);
        }).join('\n');

        const eventObjectUsings = eventObjectAliases.jobs.map(eventObjectTypeName => {
          return `  using ${eventObjectTypeName.replace('on', 'On')} = ${componentName}_${eventObjectTypeName.replace('on', 'On')};`
        }).join('\n');
        
        const eventEmitter = eventEmitterTemplate
                              .replace(/::_COMPONENT_EVENT_OBJECT_TYPES_::/g, eventObjects)
                              .replace(/::_EVENT_EMITTER_METHODS_::/g, eventEmitterMethods)
                              .replace(/::_EVENT_EMITTER_USINGS_::/g, eventObjectUsings);

        // Registration
        const componentRegistration = registerTemplate;

        // Final output
        const replaceContent = function (template: string): string {
          return template
            .replace(/::_COMPONENT_PROP_OBJECT_TYPES_::/g, propObjectTypes)
            .replace(/::_COMPONENT_PROP_TYPES_::/g, propsTemplate)
            .replace(/::_COMPONENT_EVENT_EMITTER_::/g, eventEmitter)
            .replace(/::_COMPONENT_REGISTRATION_::/g, componentRegistration)
            .replace(/::_EVENT_EMITTER_NAME_::/g, eventEmitterName)
            .replace(/::_PROPS_NAME_::/g, propsName)
            .replace(/::_COMPONENT_NAME_::/g, componentName)
            .replace(/::_PROPS_FIELDS_::/g, propsFields)
            .replace(/::_NAMESPACE_::/g, namespace);
        };

        files.set(
          `${componentName}.g.h`,
          replaceContent(fileTemplate),
        );
      }
    }

    return files;
  };
}
