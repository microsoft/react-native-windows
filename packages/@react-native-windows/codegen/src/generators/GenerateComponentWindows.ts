/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import type {
  SchemaType,
  EventTypeAnnotation,
  PropTypeAnnotation,
  ObjectTypeAnnotation,
  CommandParamTypeAnnotation,
} from '@react-native/codegen/lib/CodegenSchema';
import {getAliasCppName, setPreferredModuleName} from './AliasManaging';
import {
  translateComponentPropsFieldType,
  translateComponentEventType,
  translateCommandParamType,
} from './PropObjectTypes';
import type {CppStringTypes} from './ObjectTypes';
import type {AliasMap} from './AliasManaging';

export type {CppStringTypes} from './ObjectTypes';

type FilesOutput = Map<string, string>;

const headerTemplate = `/*
 * This file is auto-generated from ::_COMPONENT_NAME_::NativeComponent spec file in flow / TypeScript.
 */
#pragma once

#include <JSValueComposition.h>
#include <NativeModules.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.UI.Composition.h>`;

const propsTemplate = `REACT_STRUCT(::_PROPS_NAME_::)
struct ::_PROPS_NAME_:: : winrt::implements<::_PROPS_NAME_::, winrt::Microsoft::ReactNative::IComponentProps> {
  ::_PROPS_NAME_::(winrt::Microsoft::ReactNative::ViewProps props) : ViewProps(props) {}

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

::_PROPS_FIELDS_::
  const winrt::Microsoft::ReactNative::ViewProps ViewProps;
};`;

const propsObjectTemplate = `REACT_STRUCT(::_OBJECT_NAME_::)
struct ::_OBJECT_NAME_:: {
::_OBJECT_FIELDS_::};
`;
const eventsObjectTemplate = `REACT_STRUCT(::_OBJECT_NAME_::)
struct ::_OBJECT_NAME_:: {
::_OBJECT_FIELDS_::};
`;

const eventEmitterMethodTemplate = `  void ::_EVENT_NAME_::(::_EVENT_OBJECT_TYPE_:: &value) const {
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
};`;

const baseStructTemplate = `
template<typename TUserData>
struct Base::_COMPONENT_NAME_:: {

  virtual void UpdateProps(
    const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
    const winrt::com_ptr<::_COMPONENT_NAME_::Props> &newProps,
    const winrt::com_ptr<::_COMPONENT_NAME_::Props> &/*oldProps*/) noexcept {
    m_props = newProps;
  }

  // UpdateState will only be called if this method is overridden
  virtual void UpdateState(
    const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
    const winrt::Microsoft::ReactNative::IComponentState &/*newState*/) noexcept {
  }

  virtual void UpdateEventEmitter(const std::shared_ptr<::_COMPONENT_NAME_::EventEmitter> &eventEmitter) noexcept {
    m_eventEmitter = eventEmitter;
  }

  // MountChildComponentView will only be called if this method is overridden
  virtual void MountChildComponentView(const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
           const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &/*args*/) noexcept {
  }

  // UnmountChildComponentView will only be called if this method is overridden
  virtual void UnmountChildComponentView(const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
           const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &/*args*/) noexcept {
  }

  // Initialize will only be called if this method is overridden
  virtual void Initialize(const winrt::Microsoft::ReactNative::ComponentView &/*view*/) noexcept {
  }

  // CreateVisual will only be called if this method is overridden
  virtual winrt::Microsoft::UI::Composition::Visual CreateVisual(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
    return view.as<winrt::Microsoft::ReactNative::Composition::ComponentView>().Compositor().CreateSpriteVisual();
  }

  // FinalizeUpdate will only be called if this method is overridden
  virtual void FinalizeUpdate(const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
                                        winrt::Microsoft::ReactNative::ComponentViewUpdateMask /*mask*/) noexcept {
  }

::_COMPONENT_VIEW_COMMAND_HANDLERS_::

  ::_COMPONENT_VIEW_COMMAND_HANDLER_::

  const std::shared_ptr<::_COMPONENT_NAME_::EventEmitter>& EventEmitter() const { return m_eventEmitter; }
  const winrt::com_ptr<::_COMPONENT_NAME_::Props>& Props() const { return m_props; }

private:
  winrt::com_ptr<::_COMPONENT_NAME_::Props> m_props;
  std::shared_ptr<::_COMPONENT_NAME_::EventEmitter> m_eventEmitter;
};
`;

const registerTemplate = `
template <typename TUserData>
void Register::_COMPONENT_NAME_::NativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder,
    std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"::_COMPONENT_NAME_::", [builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps(
            [](winrt::Microsoft::ReactNative::ViewProps props) noexcept { return winrt::make<::_COMPONENT_NAME_::Props>(props); });

        builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateProps(view, newProps ? newProps.as<::_COMPONENT_NAME_::Props>() : nullptr, oldProps ? oldProps.as<::_COMPONENT_NAME_::Props>() : nullptr);
        });

        builder.SetUpdateEventEmitterHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter) noexcept {
          auto userData = view.UserData().as<TUserData>();
          userData->UpdateEventEmitter(std::make_shared<::_COMPONENT_NAME_::EventEmitter>(eventEmitter));
        });

        if constexpr (&TUserData::FinalizeUpdate != &Base::_COMPONENT_NAME_::<TUserData>::FinalizeUpdate) {
            builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->FinalizeUpdate(view, mask);
          });
        } 

        if constexpr (&TUserData::UpdateState != &Base::_COMPONENT_NAME_::<TUserData>::UpdateState) {
          builder.SetUpdateStateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->member(view, newState);
          });
        }

::_REGISTER_CUSTOM_COMMAND_HANDLER_::

        if constexpr (&TUserData::MountChildComponentView != &Base::_COMPONENT_NAME_::<TUserData>::MountChildComponentView) {
          builder.SetMountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->MountChildComponentView(view, args);
          });
        }

        if constexpr (&TUserData::UnmountChildComponentView != &Base::_COMPONENT_NAME_::<TUserData>::UnmountChildComponentView) {
          builder.SetUnmountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->UnmountChildComponentView(view, args);
          });
        }

        compBuilder.SetViewComponentViewInitializer([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
          auto userData = winrt::make_self<TUserData>();
          if constexpr (&TUserData::Initialize != &Base::_COMPONENT_NAME_::<TUserData>::Initialize) {
            userData->Initialize(view);
          }
          view.UserData(*userData);
        });

        if constexpr (&TUserData::CreateVisual != &Base::_COMPONENT_NAME_::<TUserData>::CreateVisual) {
          compBuilder.SetCreateVisualHandler([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->CreateVisual(view);
          });
        }

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

::_BASE_COMPONENT_STRUCT_::

::_COMPONENT_REGISTRATION_::
} // namespace ::_NAMESPACE_::
`;

function capitalizeFirstLetter(s: string) {
  return s.charAt(0).toUpperCase() + s.slice(1);
}

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
          if (
            // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
            propsBaseType.type !== 'ReactNativeBuiltInType' ||
            // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
            propsBaseType.knownTypeName !== 'ReactNativeCoreViewProps'
          ) {
            throw new Error(
              'Currently only supports props extending from ViewProps',
            );
          }
        });

        // Props
        const propObjectAliases: AliasMap<
          ObjectTypeAnnotation<PropTypeAnnotation>
        > = {types: {}, jobs: []};
        const propsName = `${componentName}Props`;
        const propsFields = componentShape.props
          .map(prop => {
            const propType = translateComponentPropsFieldType(
              prop.typeAnnotation,
              propObjectAliases,
              `${propsName}_${prop.name}`,
              cppCodegenOptions,
            );
            return `  REACT_FIELD(${prop.name})\n  ${
              prop.optional && !propType.alreadySupportsOptionalOrHasDefault
                ? `std::optional<${propType.type}>`
                : propType.type
            } ${prop.name}${propType.initializer};\n`;
          })
          .join('\n');

        const propObjectTypes = propObjectAliases.jobs
          .map(propObjectTypeName => {
            const propObjectType = propObjectAliases.types[propObjectTypeName]!;
            const propsObjectFields = propObjectType.properties
              .map(property => {
                const propType = translateComponentPropsFieldType(
                  property.typeAnnotation,
                  propObjectAliases,
                  `${propsName}_${property.name}`,
                  cppCodegenOptions,
                );
                return `  REACT_FIELD(${property.name})\n  ${
                  property.optional &&
                  !propType.alreadySupportsOptionalOrHasDefault
                    ? `std::optional<${propType.type}>`
                    : propType.type
                } ${property.name}${propType.initializer};\n`;
              })
              .join('\n');

            return propsObjectTemplate
              .replace(
                /::_OBJECT_NAME_::/g,
                getAliasCppName(propObjectTypeName),
              )
              .replace(/::_OBJECT_FIELDS_::/g, propsObjectFields);
          })
          .join('\n');

        // Events
        const eventObjectAliases: AliasMap<
          ObjectTypeAnnotation<EventTypeAnnotation>
        > = {types: {}, jobs: []};
        const eventEmitterName = `${componentName}EventEmitter`;
        const eventEmitterMethods = componentShape.events
          .filter(event => event.typeAnnotation.argument)
          .map(event => {
            if (event.typeAnnotation.argument?.baseTypes) {
              throw new Error(
                'Events with base type arguments not currently supported',
              );
            }

            // Called to collect the eventObjectAliases
            translateComponentEventType(
              event.typeAnnotation.argument!,
              eventObjectAliases,
              `${event.name}`,
              cppCodegenOptions,
            );

            // onSomething -> something
            let eventNameLower = event.name.replace('on', '');
            eventNameLower =
              eventNameLower[0].toLowerCase() + eventNameLower.slice(1);

            return eventEmitterMethodTemplate
              .replace(/::_EVENT_NAME_::/g, event.name)
              .replace(/::_EVENT_NAME_NO_ON_::/g, eventNameLower)
              .replace(
                /::_EVENT_OBJECT_TYPE_::/g,
                event.name.replace('on', 'On'),
              );
          })
          .join('\n\n');

        const eventObjects = eventObjectAliases.jobs
          .map(eventObjectTypeName => {
            const eventObjectType =
              eventObjectAliases.types[eventObjectTypeName]!;
            const eventObjectFields = eventObjectType.properties
              .map(property => {
                const eventPropType = translateComponentEventType(
                  property.typeAnnotation,
                  eventObjectAliases,
                  eventObjectTypeName,
                  cppCodegenOptions,
                );
                return `  REACT_FIELD(${property.name})\n  ${
                  property.optional &&
                  !eventPropType.alreadySupportsOptionalOrHasDefault
                    ? `std::optional<${eventPropType.type}>`
                    : eventPropType.type
                } ${property.name}${eventPropType.initializer};\n`;
              })
              .join('\n');
            return eventsObjectTemplate
              .replace(
                /::_OBJECT_NAME_::/g,
                `${componentName}_${eventObjectTypeName.replace('on', 'On')}`,
              )
              .replace(/::_OBJECT_FIELDS_::/g, eventObjectFields);
          })
          .join('\n');

        const eventObjectUsings = eventObjectAliases.jobs
          .map(eventObjectTypeName => {
            return `  using ${eventObjectTypeName.replace(
              'on',
              'On',
            )} = ${componentName}_${eventObjectTypeName.replace('on', 'On')};`;
          })
          .join('\n');

        const eventEmitter = eventEmitterTemplate
          .replace(/::_COMPONENT_EVENT_OBJECT_TYPES_::/g, eventObjects)
          .replace(/::_EVENT_EMITTER_METHODS_::/g, eventEmitterMethods)
          .replace(/::_EVENT_EMITTER_USINGS_::/g, eventObjectUsings);

        // Commands
        const commandAliases: AliasMap<
          ObjectTypeAnnotation<CommandParamTypeAnnotation>
        > = {types: {}, jobs: []};
        const hasAnyCommands = componentShape.commands.length !== 0;
        const commandHandlers = hasAnyCommands
          ? componentShape.commands
              .map(command => {
                const commandArgs = command.typeAnnotation.params
                  .map(param => {
                    const commandArgType = translateCommandParamType(
                      param.typeAnnotation,
                      commandAliases,
                      `${componentName}_${command.name}`,
                      cppCodegenOptions,
                    );
                    return `${
                      param.optional &&
                      !commandArgType.alreadySupportsOptionalOrHasDefault
                        ? `std::optional<${commandArgType.type}>`
                        : commandArgType.type
                    } ${param.name}`;
                  })
                  .join(', ');

                return `  // You must provide an implementation of this method to handle the "${
                  command.name
                }" command
  virtual void Handle${capitalizeFirstLetter(
    command.name,
  )}Command(${commandArgs}) noexcept = 0;`;
              })
              .join('\n\n')
          : '';

        const commandHandler = hasAnyCommands
          ? `void HandleCommand(const winrt::Microsoft::ReactNative::ComponentView &view, const winrt::Microsoft::ReactNative::HandleCommandArgs& args) noexcept {
    auto userData = view.UserData().as<TUserData>();
    auto commandName = args.CommandName();
${componentShape.commands
  .map(command => {
    const commaSeparatedCommandArgs = command.typeAnnotation.params
      .map(param => param.name)
      .join(', ');
    return `    if (commandName == L"${command.name}") {
${
  command.typeAnnotation.params.length !== 0
    ? `      ${command.typeAnnotation.params
        .map(param => {
          const commandArgType = translateCommandParamType(
            param.typeAnnotation,
            commandAliases,
            `${componentName}_${command.name}`,
            cppCodegenOptions,
          );
          return `${
            param.optional &&
            !commandArgType.alreadySupportsOptionalOrHasDefault
              ? `std::optional<${commandArgType.type}>`
              : commandArgType.type
          } ${param.name};`;
        })
        .join('\n')}
      winrt::Microsoft::ReactNative::ReadArgs(args.CommandArgs(), ${commaSeparatedCommandArgs});`
    : ''
}
      userData->Handle${capitalizeFirstLetter(
        command.name,
      )}Command(${commaSeparatedCommandArgs});
      return;
    }`;
  })
  .join('\n\n')}
  }`
          : '';

        const registerCommandHandler = hasAnyCommands
          ? `        builder.SetCustomCommandHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                          const winrt::Microsoft::ReactNative::HandleCommandArgs& args) noexcept {
          auto userData = view.UserData().as<TUserData>();
          userData->HandleCommand(view, args);
        });`
          : '';

        const baseType = baseStructTemplate
          .replace(/::_COMPONENT_VIEW_COMMAND_HANDLERS_::/g, commandHandlers)
          .replace(/::_COMPONENT_VIEW_COMMAND_HANDLER_::/g, commandHandler);

        // Registration
        const componentRegistration = registerTemplate.replace(
          /::_REGISTER_CUSTOM_COMMAND_HANDLER_::/g,
          registerCommandHandler,
        );

        // Final output
        const replaceContent = function (template: string): string {
          return template
            .replace(/::_COMPONENT_PROP_OBJECT_TYPES_::/g, propObjectTypes)
            .replace(/::_COMPONENT_PROP_TYPES_::/g, propsTemplate)
            .replace(/::_COMPONENT_EVENT_EMITTER_::/g, eventEmitter)
            .replace(/::_BASE_COMPONENT_STRUCT_::/g, baseType)
            .replace(/::_COMPONENT_REGISTRATION_::/g, componentRegistration)
            .replace(/::_EVENT_EMITTER_NAME_::/g, eventEmitterName)
            .replace(/::_PROPS_NAME_::/g, propsName)
            .replace(/::_COMPONENT_NAME_::/g, componentName)
            .replace(/::_PROPS_FIELDS_::/g, propsFields)
            .replace(/::_NAMESPACE_::/g, namespace)
            .replace(/\n\n\n+/g, '\n\n');
        };

        files.set(`${componentName}.g.h`, replaceContent(fileTemplate));
      }
    }

    return files;
  };
}
