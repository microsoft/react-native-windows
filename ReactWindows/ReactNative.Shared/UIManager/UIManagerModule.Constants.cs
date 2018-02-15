using ReactNative.Tracing;
using ReactNative.UIManager.Events;
using System.Collections.Generic;
using Map = System.Collections.Generic.Dictionary<string, object>;

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

        private static Dictionary<string, object> CreateConstants(
            IReadOnlyList<IViewManager> viewManagers,
            IDictionary<string, object> allBubblingEventTypes,
            IDictionary<string, object> allDirectEventTypes)
        {
            var constants = GetConstants();

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
                foreach (var entry in genericBubblingEventTypes)
                {
                    allBubblingEventTypes.Add(entry);
                }
            }
            if (allDirectEventTypes != null)
            {
                foreach (var entry in genericDirectEventTypes)
                {
                    allDirectEventTypes.Add(entry);
                }
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
                        genericBubblingEventTypes,
                        genericDirectEventTypes,
                        allBubblingEventTypes,
                        allDirectEventTypes);

                    if (viewManagerConstants.Count > 0)
                    {
                        constants.Add(viewManagerName, viewManagerConstants);
                    }
                }
            }

            return constants;
        }

        private static IDictionary<string, object> CreateConstantsForViewManager(
            IViewManager viewManager,
            IReadOnlyDictionary<string, object> defaultBubblingEvents,
            IReadOnlyDictionary<string, object> defaultDirectEvents,
            IDictionary<string, object> cumulativeBubblingEventTypes,
            IDictionary<string, object> cumulativeDirectEventTypes)
        {
            var viewManagerConstants = new Dictionary<string, object>();

            var viewManagerBubblingEvents = viewManager.ExportedCustomBubblingEventTypeConstants;
            if (viewManagerBubblingEvents != null)
            {
                var mergedViewManagerBubblingEvents = new Dictionary<string, object>();
                RecursiveMerge(cumulativeBubblingEventTypes, viewManagerBubblingEvents);
                RecursiveMerge(mergedViewManagerBubblingEvents, defaultBubblingEvents);
                RecursiveMerge(mergedViewManagerBubblingEvents, viewManagerBubblingEvents);
                viewManagerConstants.Add("bubblingEventTypes", mergedViewManagerBubblingEvents);
            }
            else if (defaultBubblingEvents != null)
            {
                viewManagerConstants.Add("bubblingEventTypes", defaultBubblingEvents);
            }

            var viewManagerDirectEvents = viewManager.ExportedCustomDirectEventTypeConstants;
            if (viewManagerDirectEvents != null)
            {
                var mergedViewManagerDirectEvents = new Dictionary<string, object>();
                RecursiveMerge(cumulativeDirectEventTypes, viewManagerDirectEvents);
                RecursiveMerge(mergedViewManagerDirectEvents, defaultDirectEvents);
                RecursiveMerge(mergedViewManagerDirectEvents, viewManagerDirectEvents);
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

            var viewManagerNativeProps = viewManager.NativeProperties;
            if (viewManagerNativeProps != null && viewManagerNativeProps.Count > 0)
            {
                viewManagerConstants.Add("NativeProps", viewManagerNativeProps);
            }

            return viewManagerConstants;
        }

        private static Dictionary<string, object> GetBubblingEventTypeConstants()
        {
            return new Map
            {
                {
                    "topChange",
                    new Map
                    {
                        {
                            "phasedRegistrationNames",
                            new Map
                            {
                                { "bubbled", "onChange" },
                                { "captured", "onChangeCapture" },
                            }
                        }
                    }
                },
                {
                    "topSelect",
                    new Map
                    {
                        {
                            "phasedRegistrationNames",
                            new Map
                            {
                                { "bubbled", "onSelect" },
                                { "captured", "onSelectCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Start.GetJavaScriptEventName(),
                    new Map
                    {
                        {
                            "phasedRegistrationNames",
                            new Map
                            {
                                { "bubbled", "onTouchStart" },
                                { "captured", "onTouchStartCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Move.GetJavaScriptEventName(),
                    new Map
                    {
                        {
                            "phasedRegistrationNames",
                            new Map
                            {
                                { "bubbled", "onTouchMove" },
                                { "captured", "onTouchMoveCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.End.GetJavaScriptEventName(),
                    new Map
                    {
                        {
                            "phasedRegistrationNames",
                            new Map
                            {
                                { "bubbled", "onTouchEnd" },
                                { "captured", "onTouchEndCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Cancel.GetJavaScriptEventName(),
                    new Map
                    {
                        {
                            "phasedRegistrationNames",
                            new Map
                            {
                                { "bubbled", "onTouchCancel" },
                                { "captured", "onTouchCancelCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Entered.GetJavaScriptEventName(),
                    new Map
                    {
                        {
                            "phasedRegistrationNames",
                            new Map
                            {
                                { "bubbled", "onMouseOver" },
                                { "captured", "onMouseOverCapture" },
                            }
                        }
                    }
                },
                {
                    TouchEventType.Exited.GetJavaScriptEventName(),
                    new Map
                    {
                        {
                            "phasedRegistrationNames",
                            new Map
                            {
                                { "bubbled", "onMouseOut" },
                                { "captured", "onMouseOutCapture" },
                            }
                        }
                    }
                },
                {
                    "topFocus",
                    new Dictionary<string, object>()
                    {
                        {
                            "phasedRegistrationNames",
                            new Dictionary<string, string>()
                            {
                                { "bubbled" , "onFocus" },
                                { "captured" , "onFocusCapture" }
                            }
                        }
                    }
                },
                {
                    "topBlur",
                    new Dictionary<string, object>()
                    {
                        {
                            "phasedRegistrationNames",
                            new Dictionary<string, string>()
                            {
                                { "bubbled" , "onBlur" },
                                { "captured" , "onBlurCapture" }
                            }
                        }
                    }
                },
                {
                    "topKeyDown",
                    new Dictionary<string, object>()
                    {
                        {
                            "phasedRegistrationNames",
                            new Dictionary<string, string>()
                            {
                                { "bubbled" , "onKeyDown" },
                                { "captured" , "onKeyDownCapture" }
                            }
                        }
                    }
                },
                {
                    "topKeyUp",
                    new Dictionary<string, object>()
                    {
                        {
                            "phasedRegistrationNames",
                            new Dictionary<string, string>()
                            {
                                { "bubbled" , "onKeyUp" },
                                { "captured" , "onKeyUpCapture" }
                            }
                        }
                    }
                },
            };
        }

        private static Dictionary<string, object> GetDirectEventTypeConstants()
        {
            return new Map
            {
                {
                    "topAccessibilityTap",
                    new Map
                    {
                        { "registrationName", "onAccessibilityTap" },
                    }
                },
                {
                    "topSelectionChange",
                    new Map
                    {
                        { "registrationName", "onSelectionChange" },
                    }
                },
                {
                    "topLoadingStart",
                    new Map
                    {
                        { "registrationName", "onLoadingStart" },
                    }
                },
                {
                    "topLoadingFinish",
                    new Map
                    {
                        { "registrationName", "onLoadingFinish" },
                    }
                },
                {
                    "topLoadingError",
                    new Map
                    {
                        { "registrationName", "onLoadingError" },
                    }
                },
                {
                    "topContentSizeChange",
                    new Map
                    {
                        { "registrationName", "onContentSizeChange" },
                    }
                },
                {
                    "topLayout",
                    new Map
                    {
                        { "registrationName", "onLayout" },
                    }
                },
                {
                    "topMouseEnter",
                    new Map
                    {
                        { "registrationName", "onMouseEnter" },
                    }
                },
                {
                    "topMouseLeave",
                    new Map
                    {
                        { "registrationName", "onMouseLeave" },
                    }
                },
                {
                    "topMessage",
                    new Map
                    {
                        { "registrationName", "onMessage" },
                    }
                },
            };
        }

        private static Dictionary<string, object> GetConstants()
        {
            return new Map
            {
                {
                    "UIView",
                    new Map
                    {
                        {
                            "ContentMode",
                            new Map
                            {
                                /* TODO: declare content mode properties */
                            }
                        },
                    }
                },
                {
                    "UIText",
                    new Map
                    {
                        {
                            "AutocapitalizationType",
                            new Map
                            {
                                /* TODO: declare capitalization types */
                            }
                        },
                    }
                },
                {
                    "StyleConstants",
                    new Map
                    {
                        {
                            "PointerEventsValues",
                            new Map
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
                    new Map
                    {
                        { ACTION_DISMISSED, ACTION_DISMISSED },
                        { ACTION_ITEM_SELECTED, ACTION_ITEM_SELECTED },
                    }
                },
                {
                    "AccessibilityEventTypes",
                    new Map
                    {
                        /* TODO: declare accessibility event types */
                    }
                },
            };
        }

        private static void RecursiveMerge(IDictionary<string, object> sink, IReadOnlyDictionary<string, object> source)
        {
            if (sink == null || source == null || source.Count == 0)
            {
                return;
            }

            foreach (var pair in source)
            {
                var existing = default(object);
                if (sink.TryGetValue(pair.Key, out existing))
                {
                    var sourceAsMap = pair.Value as IReadOnlyDictionary<string, object>;
                    var sinkAsMap = existing as IDictionary<string, object>;
                    if (sourceAsMap != null && sinkAsMap != null)
                    {
                        RecursiveMerge(sinkAsMap, sourceAsMap);
                    }
                    else
                    {
                        // TODO: confirm that exports should be allowed to override.
                        sink[pair.Key] = pair.Value;
                    }
                }
                else
                {
                    sink.Add(pair.Key, pair.Value);
                }
            }
        }
    }
}
