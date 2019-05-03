// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Tracing;
using ReactNative.UIManager.Events;
using System.Collections.Generic;
using System.Linq;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Native module to allow JavaScript to create and update native views.
    /// </summary>
    public partial class UIManagerModule
    {
        private const string BUBBLING_EVENTS_KEY = "bubblingEventTypes";
        private const string DIRECT_EVENTS_KEY = "directEventTypes";

        private const string ACTION_DISMISSED = "dismissed";
        private const string ACTION_ITEM_SELECTED = "itemSelected";

        private static JObject CreateConstants(
            IReadOnlyList<IViewManager> viewManagers,
            JObject allBubblingEventTypes,
            JObject allDirectEventTypes,
            bool lazyViewManagersEnabled)
        {
            var constants = GetConstants();

            if (lazyViewManagersEnabled)
            {
                constants.Add("ViewManagerNames", new JArray(viewManagers.Select(viewManager => viewManager.Name)));
                return constants;
            }

            // Generic/default event types:
            // All view managers are capable of dispatching these events.
            // They will be automatically registered with React Fiber.
            var genericBubblingEventTypes = GetBubblingEventTypeConstants();
            var genericDirectEventTypes = GetDirectEventTypeConstants();

            // Cumulative event types:
            // View manager specific event types are collected as views are loaded.
            // This information is used later when events are dispatched.
            if (allBubblingEventTypes != null)
            {
                allBubblingEventTypes.Merge(genericBubblingEventTypes);
            }

            if (allDirectEventTypes != null)
            {
                allDirectEventTypes.Merge(genericDirectEventTypes);
            }

            foreach (var viewManager in viewManagers)
            {
                var viewManagerName = viewManager.Name;
                using (Tracer.Trace(Tracer.TRACE_TAG_REACT_BRIDGE, "UIManagerModuleConstantsHelper.createConstants")
                    .With("ViewManager", viewManagerName)
                    .Start())
                {
                    var viewManagerConstants = CreateConstantsForViewManager(
                        viewManager,
                        null,
                        null,
                        allBubblingEventTypes,
                        allDirectEventTypes);

                    if (viewManagerConstants.Count > 0)
                    {
                        constants.Add(viewManagerName, viewManagerConstants);
                    }
                }
            }

            constants.Add("genericBubblingEventTypes", genericBubblingEventTypes);
            constants.Add("genericDirectEventTypes", genericDirectEventTypes);
            return constants;
        }

        private static JObject GetDefaultExportableEventTypes()
        {
            return new JObject
            {
                { BUBBLING_EVENTS_KEY, GetBubblingEventTypeConstants() },
                { DIRECT_EVENTS_KEY, GetDirectEventTypeConstants() },
            };
        }

        private static JObject CreateConstantsForViewManager(
            IViewManager viewManager,
            JObject defaultBubblingEvents,
            JObject defaultDirectEvents,
            JObject cumulativeBubblingEventTypes,
            JObject cumulativeDirectEventTypes)
        {
            var viewManagerConstants = new JObject();

            var viewManagerBubblingEvents = viewManager.ExportedCustomBubblingEventTypeConstants;
            if (viewManagerBubblingEvents != null)
            {
                var mergedViewManagerBubblingEvents = new JObject();
                cumulativeBubblingEventTypes?.Merge(viewManagerBubblingEvents);
                mergedViewManagerBubblingEvents.Merge(defaultBubblingEvents);
                mergedViewManagerBubblingEvents.Merge(viewManagerBubblingEvents);
                viewManagerConstants.Add("bubblingEventTypes", mergedViewManagerBubblingEvents);
            }
            else if (defaultBubblingEvents != null)
            {
                viewManagerConstants.Add("bubblingEventTypes", defaultBubblingEvents);
            }

            var viewManagerDirectEvents = viewManager.ExportedCustomDirectEventTypeConstants;
            if (viewManagerDirectEvents != null)
            {
                var mergedViewManagerDirectEvents = new JObject();
                cumulativeDirectEventTypes?.Merge(viewManagerDirectEvents);
                mergedViewManagerDirectEvents.Merge(defaultDirectEvents);
                mergedViewManagerDirectEvents.Merge(viewManagerDirectEvents);
                viewManagerConstants.Add("directEventTypes", mergedViewManagerDirectEvents);
            }
            else if (defaultDirectEvents != null)
            {
                viewManagerConstants.Add("directEventTypes", defaultDirectEvents);
            }

            var customViewConstants = viewManager.ExportedViewConstants;
            if (customViewConstants != null)
            {
                viewManagerConstants.Add("Constants", customViewConstants);
            }

            var viewManagerCommands = viewManager.CommandsMap;
            if (viewManagerCommands != null)
            {
                viewManagerConstants.Add("Commands", viewManagerCommands);
            }

            var viewManagerNativeProps = viewManager.NativeProps;
            if (viewManagerNativeProps != null && viewManagerNativeProps.Count > 0)
            {
                viewManagerConstants.Add("NativeProps", viewManagerNativeProps);
            }

            return viewManagerConstants;
        }

        private static JObject GetBubblingEventTypeConstants()
        {
            return new JObject
            {
                {
                    "topChange",
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled", "onChange" },
                                { "captured", "onChangeCapture" },
                            }
                        }
                    }
                },
                {
                    "topSelect",
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled", "onSelect" },
                                { "captured", "onSelectCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Start.GetJavaScriptEventName(),
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled", "onTouchStart" },
                                { "captured", "onTouchStartCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Move.GetJavaScriptEventName(),
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled", "onTouchMove" },
                                { "captured", "onTouchMoveCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.End.GetJavaScriptEventName(),
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled", "onTouchEnd" },
                                { "captured", "onTouchEndCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Cancel.GetJavaScriptEventName(),
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled", "onTouchCancel" },
                                { "captured", "onTouchCancelCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Entered.GetJavaScriptEventName(),
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled", "onMouseOver" },
                                { "captured", "onMouseOverCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Exited.GetJavaScriptEventName(),
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled", "onMouseOut" },
                                { "captured", "onMouseOutCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.PointerMove.GetJavaScriptEventName(),
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled", "onMouseMove" },
                                { "captured", "onMouseMoveCapture" },
                            }
                        }
                    }
                },
                {
                    "topFocus",
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled" , "onFocus" },
                                { "captured" , "onFocusCapture" }
                            }
                        }
                    }
                },
                {
                    "topBlur",
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled" , "onBlur" },
                                { "captured" , "onBlurCapture" }
                            }
                        }
                    }
                },
                {
                    "topKeyDown",
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled" , "onKeyDown" },
                                { "captured" , "onKeyDownCapture" }
                            }
                        }
                    }
                },
                {
                    "topKeyUp",
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled" , "onKeyUp" },
                                { "captured" , "onKeyUpCapture" }
                            }
                        }
                    }
                },
                {
                    "topKeyPress",
                    new JObject
                    {
                        {
                            "phasedRegistrationNames",
                            new JObject
                            {
                                { "bubbled" , "onKeyPress" },
                                { "captured" , "onKeyPressCapture" }
                            }
                        }
                    }
                },
            };
        }

        private static JObject GetDirectEventTypeConstants()
        {
            return new JObject
            {
                {
                    "topAccessibilityTap",
                    new JObject
                    {
                        { "registrationName", "onAccessibilityTap" },
                    }
                },
                {
                    "topSelectionChange",
                    new JObject
                    {
                        { "registrationName", "onSelectionChange" },
                    }
                },
                {
                    "topLoadingStart",
                    new JObject
                    {
                        { "registrationName", "onLoadingStart" },
                    }
                },
                {
                    "topLoadingFinish",
                    new JObject
                    {
                        { "registrationName", "onLoadingFinish" },
                    }
                },
                {
                    "topLoadingError",
                    new JObject
                    {
                        { "registrationName", "onLoadingError" },
                    }
                },
                {
                    "topContentSizeChange",
                    new JObject
                    {
                        { "registrationName", "onContentSizeChange" },
                    }
                },
                {
                    "topLayout",
                    new JObject
                    {
                        { "registrationName", "onLayout" },
                    }
                },
                {
                    "topMouseEnter",
                    new JObject
                    {
                        { "registrationName", "onMouseEnter" },
                    }
                },
                {
                    "topMouseLeave",
                    new JObject
                    {
                        { "registrationName", "onMouseLeave" },
                    }
                },
                {
                    "topMessage",
                    new JObject
                    {
                        { "registrationName", "onMessage" },
                    }
                },
            };
        }

        private static JObject GetConstants()
        {
            return new JObject
            {
                {
                    "UIView",
                    new JObject
                    {
                        {
                            "ContentMode",
                            new JObject
                            {
                                /* TODO: declare content mode properties */
                            }
                        },
                    }
                },
                {
                    "UIText",
                    new JObject
                    {
                        {
                            "AutocapitalizationType",
                            new JObject
                            {
                                /* TODO: declare capitalization types */
                            }
                        },
                    }
                },
                {
                    "StyleConstants",
                    new JObject
                    {
                        {
                            "PointerEventsValues",
                            new JObject
                            {
                                { "none", PointerEvents.None.ToString() },
                                { "boxNone", PointerEvents.BoxNone.ToString() },
                                { "boxOnly", PointerEvents.BoxOnly.ToString() },
                                { "unspecified", PointerEvents.Auto.ToString() },
                            }
                        },
                    }
                },
                {
                    "PopupMenu",
                    new JObject
                    {
                        { ACTION_DISMISSED, ACTION_DISMISSED },
                        { ACTION_ITEM_SELECTED, ACTION_ITEM_SELECTED },
                    }
                },
                {
                    "AccessibilityEventTypes",
                    new JObject
                    {
                        /* TODO: declare accessibility event types */
                    }
                },
            };
        }
    }
}
