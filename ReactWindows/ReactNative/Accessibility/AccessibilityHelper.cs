// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// #define PERF_LOG

using Newtonsoft.Json.Linq;
using ReactNative.Reflection;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
#if PERF_LOG
using ReactNative.Common;
using ReactNative.Tracing;
#endif
using System.Linq;
using System.Text;
using System.Threading;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;

namespace ReactNative.Accessibility
{
    /// <summary>
    /// Accessibility helper methods.
    /// </summary>
    public static class AccessibilityHelper
    {
        /// <summary>
        /// A per UIElement context containing accessibility props (as sent from JS), and some additional state
        /// </summary>
        private class ElementAccessibilityContext
        {
            public enum HidingChildren
            {
                NotSure = 0,
                Yes,
                No
            }

            public ImportantForAccessibility ImportantForAccessibilityProp { get; set; }

            public string AccessibilityLabelProp { get; set; }

            public HidingChildren CurrentlyHidingChildren { get; set; }

            public bool Dirty { get; set; }
        }

        /// <summary>
        /// Per dispatcher thread context
        /// </summary>
        private class TreeContext
        {
            /// <summary>
            /// Set of root views
            /// </summary>
            public HashSet<UIElement> RootViews { get; set; }

            /// <summary>
            /// Dictionary replacement of AttachedProperty objects associated with UIElement (it seems attached properties are >10x slower)
            /// A null value is interpreted as {ImportantForAccessibility.Auto, null, HidingChildren.NotSure, false}.
            /// </summary>
            public Dictionary<UIElement, ElementAccessibilityContext> ElementAccessibilityContextMap { get; set; }

            /// <summary>
            /// Set to true if any accessibility processing is needed for the next batch
            /// </summary>
            public bool Dirty { get; set; }

#if PERF_LOG
            public int ElementCount;
            public int ProcessedNodesCount;
            public int DirtyNodesCount;
            public int MarkedDirtyNodesCount;
#endif
        }

        /// <summary>
        /// State for all dispatcher threads
        /// </summary>
        private static ThreadLocal<TreeContext> s_treeContext =
            new ThreadLocal<TreeContext>(() => new TreeContext()
            {
                RootViews = new HashSet<UIElement>(),
                ElementAccessibilityContextMap = new Dictionary<UIElement, ElementAccessibilityContext>()
            });

        private static ElementAccessibilityContext EnsureElementAccessibilityContext(UIElement element)
        {
            if (!s_treeContext.Value.ElementAccessibilityContextMap.TryGetValue(element, out ElementAccessibilityContext property))
            {
                if (!element.HasTag())
                {
                    throw new InvalidOperationException("Can't create an ElementAccessibilityContext for a non-React view");
                }

                property = new ElementAccessibilityContext()
                {
                    ImportantForAccessibilityProp = ImportantForAccessibility.Auto,
                    AccessibilityLabelProp = null,
                    CurrentlyHidingChildren = ElementAccessibilityContext.HidingChildren.NotSure,
                    Dirty = false
                };
                s_treeContext.Value.ElementAccessibilityContextMap.Add(element, property);
            }

            return property;
        }

#region Property Accessors

        private static void SetAccessibilityLabelProp(UIElement element, string value)
        {
            EnsureElementAccessibilityContext(element).AccessibilityLabelProp = value;
        }

        private static string GetAccessibilityLabelProp(UIElement element)
        {
            return EnsureElementAccessibilityContext(element).AccessibilityLabelProp;
        }

        private static void SetImportantForAccessibilityProp(UIElement element, ImportantForAccessibility value)
        {
            EnsureElementAccessibilityContext(element).ImportantForAccessibilityProp = value;
        }

        private static ImportantForAccessibility GetImportantForAccessibilityProp(UIElement element)
        {
            return EnsureElementAccessibilityContext(element).ImportantForAccessibilityProp;
        }

        private static void SetCurrentlyHidingChildren(UIElement element, ElementAccessibilityContext.HidingChildren value)
        {
            EnsureElementAccessibilityContext(element).CurrentlyHidingChildren = value;
        }

        private static ElementAccessibilityContext.HidingChildren GetCurrentlyHidingChildren(UIElement element)
        {
            return EnsureElementAccessibilityContext(element).CurrentlyHidingChildren;
        }

        private static void SetDirty(UIElement element, bool value)
        {
#if PERF_LOG
            if (GetDirty(element) != value)
            {
                s_treeContext.Value.DirtyNodesCount += (value ? 1 : -1);
            }
#endif
            EnsureElementAccessibilityContext(element).Dirty = value;
        }

        private static bool GetDirty(UIElement element)
        {
            return EnsureElementAccessibilityContext(element).Dirty;
        }

        private static void MarkElementDirty(UIElement element)
        {
            // Retrieve automation peer
            var peer = FrameworkElementAutomationPeer.FromElement(element);

            if (peer == null)
            {
                return;
            }

#if PERF_LOG
            if (!GetDirty(element))
            {
                s_treeContext.Value.MarkedDirtyNodesCount++;
            }
#endif
            // Mark element itself
            SetDirty(element, true);

            // Go up the parent chain
            peer = peer.Navigate(AutomationNavigationDirection.Parent) as AutomationPeer;

            while (peer != null)
            {
                element = GetUIElementFromAutomationPeer(peer);
                // Skip elements not created directly by React
                if (element != null && element.HasTag())
                {
                    if (GetDirty(element))
                    {
                        // Found dirty element, no need to continue.
                        break;
                    }

                    SetDirty(element, true);
                }

                // Go up the parent chain
                peer = peer.Navigate(AutomationNavigationDirection.Parent) as AutomationPeer;
            }

            s_treeContext.Value.Dirty = true;
        }

#endregion

#region Public entry points

        /// <summary>
        /// Marks accessibility data in UI tree for update after <paramref name="child"/> is added to <paramref name="parent"/>
        /// Must be called when a child is added to <paramref name="parent"/>.
        /// </summary>
        /// <param name="parent">The element to which <paramref name="child"/> is added.</param>
        /// <param name="child">The added child.</param>
        public static void OnChildAdded(UIElement parent, DependencyObject child)
        {
            // Call UpdateLayout() on parent to make sure parent/child relationship is updated.
            parent.UpdateLayout();

            // No guarantee all children are hidden anymore due to additional child, reset the flag
            SetCurrentlyHidingChildren(parent, ElementAccessibilityContext.HidingChildren.NotSure);

            // Mark the parent dirty (or skip if there is no associated peer available)
            MarkElementDirty(parent);
        }

        /// <summary>
        /// Marks accessibility data in UI tree for update after a child is removed from <paramref name="parent"/>
        /// Must be called when a child is removed from <paramref name="parent"/>.
        /// </summary>
        /// <param name="parent">The element from which a child is removed.</param>
        public static void OnChildRemoved(UIElement parent)
        {
            // Call UpdateLayout() on parent to make sure parent/child relationship is updated.
            parent.UpdateLayout();

            // Mark the parent dirty (or skip if there is no associated peer available)
            MarkElementDirty(parent);
        }

        /// <summary>
        /// Marks accessibility data in UI tree for update after a relevant (to accessibility) property of that element changed.
        /// For example, it is used by <see cref="RichTextBlock"/> to notify when text or structure is changed inside the text block.
        /// </summary>
        /// <param name="uiElement">The <see cref="UIElement"/>.</param>
        /// <param name="dependencyProperty">The dependency property that changed. It's important to provide this parameter if possible.</param>
        public static void OnElementChanged(UIElement uiElement, DependencyProperty dependencyProperty = null)
        {
            if (ReferenceEquals(UIElement.VisibilityProperty, dependencyProperty))
            {
                // add/remove like
                var fe = uiElement as FrameworkElement;
                if (fe == null)
                {
                    return;
                }
                var parent = fe.Parent as UIElement;
                if (parent == null)
                {
                    return;
                }

                if (uiElement.Visibility == Visibility.Visible)
                {
                    OnChildAdded(parent, fe);
                }
                else
                {
                    OnChildRemoved(parent);
                }
                return;
            }
            // Mark the element dirty
            MarkElementDirty(uiElement);
        }

        /// <summary>
        /// Sets the ImportantForAccessibility property for <paramref name="uiElement"/>.
        /// It uses AutomationProperties.AccessibilityView to expose the element and its children
        /// to narrator. ImportantForAccessibility value is stored as an attached-like property.
        /// </summary>
        /// <param name="uiElement">Element which ImportantForAccessibility property is set.</param>
        /// <param name="importantForAccessibility">The new value of ImportantForAccessibility property.</param>
        public static void SetImportantForAccessibility(UIElement uiElement, ImportantForAccessibility importantForAccessibility)
        {
            // Check if property is already set to requested value.
            if (GetImportantForAccessibilityProp(uiElement) == importantForAccessibility)
            {
                return;
            }
            SetImportantForAccessibilityProp(uiElement, importantForAccessibility);

            // Mark element as dirty
            MarkElementDirty(uiElement);
        }

        /// <summary>
        /// Gets the ImportantForAccessibility property for <paramref name="element"/>.
        /// </summary>
        /// <param name="element"></param>
        /// <returns>Null if the <paramref name="element"/> is not a React Native view, value of the property otherwise.</returns>
        public static ImportantForAccessibility? GetImportantForAccessibility(UIElement element)
        {
            return element.HasTag() ? GetImportantForAccessibilityProp(element) : default(ImportantForAccessibility?);
        }

        /// <summary>
        /// Sets the AccessibilityLabel property for <paramref name="element"/>.
        /// It uses <see cref="AutomationProperties.NameProperty"/> to expose the element and its children
        /// to narrator. AccessibilityLabel value is stored as an attached-like property.
        /// </summary>
        /// <param name="element"></param>
        /// <param name="accessibilityLabel"></param>
        public static void SetAccessibilityLabel(UIElement element, string accessibilityLabel)
        {
            // Check if property is already set to requested value.
            if (GetAccessibilityLabelProp(element) == accessibilityLabel)
            {
                return;
            }
            SetAccessibilityLabelProp(element, accessibilityLabel);

            // Mark element as dirty
            MarkElementDirty(element);
        }

        /// <summary>
        /// Gets the AccessibilityLabel property for <paramref name="element"/>.
        /// </summary>
        /// <param name="element"></param>
        /// <returns>Null if the <paramref name="element"/> is not a React Native view, value of the property otherwise.</returns>
        public static string GetAccessibilityLabel(UIElement element)
        {
            return element.HasTag() ? GetAccessibilityLabelProp(element) : null;
        }

        /// <summary>
        /// Parses <paramref name="accessibilityTraitsValue"/> and sets AccessibilityTraits property for <paramref name="view"/>.
        /// </summary>
        /// <param name="view"></param>
        /// <param name="accessibilityTraitsValue"></param>
        public static void SetAccessibilityTraits(IAccessible view, object accessibilityTraitsValue)
        {
            AccessibilityTrait[] result = null;
            if (accessibilityTraitsValue != null)
            {
                if (accessibilityTraitsValue is JArray asJArray)
                {
                    result = asJArray.Values<string>()
                        .Select(ParseTrait)
                        .OfType<AccessibilityTrait>()
                        .ToArray();

                    result = result.Length > 0 ? result : null;
                }
                else if (EnumHelpers.TryParse<AccessibilityTrait>(accessibilityTraitsValue.ToString(), out var accessibilityTrait))
                {
                    result = new[] { accessibilityTrait };
                }
            }

            view.AccessibilityTraits = result;
        }

        /// <summary>
        /// Makes screen reader announce the element if <see cref="AutomationProperties.LiveSettingProperty"/>
        /// is not <see cref="AutomationLiveSetting.Off"/> and <see cref="AutomationProperties.NameProperty"/>
        /// is not null or empty, by rising the <see cref="AutomationEvents.LiveRegionChanged"/> event.
        /// </summary>
        /// <param name="element"></param>
        public static void AnnounceIfNeeded(UIElement element)
        {
            if (AutomationProperties.GetLiveSetting(element) != AutomationLiveSetting.Off
                && !string.IsNullOrEmpty(AutomationProperties.GetName(element)))
            {
                var peer = FrameworkElementAutomationPeer.FromElement(element);
                peer?.RaiseAutomationEvent(AutomationEvents.LiveRegionChanged);
            }
        }

#endregion

#region Internal entry points

        /// <summary>
        /// The framework calls this method on new view instance creation. The method is
        /// intended for performing necessary accessibility setup (if any) for the newly created views.
        /// </summary>
        /// <param name="view"></param>
        internal static void OnViewInstanceCreated(UIElement view)
        {
            var elementPeer = FrameworkElementAutomationPeer.FromElement(view);
            if (elementPeer == null)
            {
                // Not all the UIElements will create AutomationPeer immediately after being created (e.g. Border, Canvas).
                // We need to make sure AutomationPeer is always created since the impementation relies on
                // traversing automation tree. Setting AutomationProperties.Name to some string and then clearing it will guarantee that
                // AutomationPeer is always created.
                AutomationProperties.SetName(view, " ");
                view.ClearValue(AutomationProperties.NameProperty);
            }

            // Create context proactively, there is no room for allocation optimization due to CurrentlyHidingChildren property.
            EnsureElementAccessibilityContext(view);

#if PERF_LOG
            s_treeContext.Value.ElementCount++;
#endif
        }

        /// <summary>
        /// The framework calls this method on view instance destruction. We use it to clean up associated context
        /// </summary>
        /// <param name="view"></param>
        internal static void OnDropViewInstance(UIElement view)
        {
#if PERF_LOG
            if (GetDirty(view))
            {
                s_treeContext.Value.DirtyNodesCount--;
            }

            s_treeContext.Value.ElementCount--;
#endif
            s_treeContext.Value.ElementAccessibilityContextMap.Remove(view);
        }

        /// <summary>
        /// Called on root view creation.
        /// Has to be run on the dispatcher thread corresponding to this root view.
        /// </summary>
        /// <param name="rootView">The root view to be added.</param>
        internal static void OnRootViewAdded(UIElement rootView)
        {
            s_treeContext.Value.RootViews.Add(rootView);

            OnViewInstanceCreated(rootView);
        }

        /// <summary>
        /// Called on root view removal.
        /// Has to be run on the dispatcher thread corresponding to this root view.
        /// </summary>
        /// <param name="rootView">The root view to be removed.</param>
        internal static void OnRootViewRemoved(UIElement rootView)
        {
            s_treeContext.Value.RootViews.Remove(rootView);
        }

        /// <summary>
        /// Called at the end of each batch.
        /// Has to be run on the dispatcher thread corresponding to the <see cref="UIManager.NativeViewHierarchyManager"/> processing the batch.
        /// </summary>
        internal static void OnBatchComplete()
        {
            // Exit fast if no dirty node exists at all
            if (!s_treeContext.Value.Dirty)
            {
                return;
            }

#if PERF_LOG
            s_treeContext.Value.ProcessedNodesCount = 0;
            var savedDirtyNodesCount = s_treeContext.Value.DirtyNodesCount;
#endif

            // Recurse through all known roots
            foreach (var root in s_treeContext.Value.RootViews)
            {
                UpdateAccessibilityPropertiesForTree(root);
            }

            // Not dirty anymore
            s_treeContext.Value.Dirty = false;

#if PERF_LOG
            RnLog.Info(ReactConstants.RNW, $"Stats: ElementCount: {s_treeContext.Value.ElementCount}, " +
                                           $"MarkedDirtyNodesCount: {s_treeContext.Value.MarkedDirtyNodesCount}, " +
                                           $"DirtyNodesCount(before): {savedDirtyNodesCount}, " +
                                           $"DirtyNodesCount(after): {s_treeContext.Value.DirtyNodesCount}, " +
                                           $"ProcessedNodesCount: {s_treeContext.Value.ProcessedNodesCount}");
            s_treeContext.Value.MarkedDirtyNodesCount = 0;
#endif
        }

#endregion

        /// <summary>
        /// Recursively updates all accessibility properties in the tree rooted by <paramref name="rootElement"/>.
        /// </summary>
        /// <param name="rootElement"></param>
        private static void UpdateAccessibilityPropertiesForTree(UIElement rootElement)
        {
            // Bootstrap the recursion
            var elementPeer = FrameworkElementAutomationPeer.FromElement(rootElement);
 
            UpdateAccessibilityViewAndNameForUIElement(rootElement, elementPeer, false);

            SetDirty(rootElement, false);
        }

        /// <summary>
        /// Sets AccessibilityView and Name property for <paramref name="element"/> and its children according to
        /// the element's "importantForAccessibility" property  (if <paramref name="hideNodes"/>  is false) / "Raw" (if <paramref name="hideNodes"/>  is true) ,
        /// and "accessibilityLabel".
        /// </summary>
        /// <param name="element"></param>
        /// <param name="elementPeer"></param>
        /// <param name="hideNodes"></param>
        private static void UpdateAccessibilityViewAndNameForUIElement(UIElement element, AutomationPeer elementPeer, bool hideNodes)
        {
            var importantForAccessibilityProp = GetImportantForAccessibilityProp(element);
            var accessibilityLabelProp = GetAccessibilityLabelProp(element);
            var hasLabelSet = !string.IsNullOrEmpty(accessibilityLabelProp);

#if PERF_LOG
            s_treeContext.Value.ProcessedNodesCount++;
#endif

            // Phase 1: set correct AV for the current node
            if (hideNodes)
            {
                AutomationProperties.SetAccessibilityView(element, AccessibilityView.Raw);
            }
            else
            {
                switch (importantForAccessibilityProp)
                {
                    case ImportantForAccessibility.Auto when !hasLabelSet:
                        element.ClearValue(AutomationProperties.AccessibilityViewProperty);
                        break;

                    case ImportantForAccessibility.Auto when hasLabelSet:
                    case ImportantForAccessibility.Yes:
                    case ImportantForAccessibility.YesDontHideDescendants:
                        AutomationProperties.SetAccessibilityView(element, AccessibilityView.Content);
                        break;

                    case ImportantForAccessibility.No:
                    case ImportantForAccessibility.NoHideDescendants:
                        AutomationProperties.SetAccessibilityView(element, AccessibilityView.Raw);
                        break;

                    default:
                        throw new NotImplementedException($"Unknown ImportantForAccessibility value [{importantForAccessibilityProp}]");
                }
            }

            // Phase 2: go down the tree after deciding how
            // We can follow dirty nodes (may be none) or traverse all
            // We can switch to "hiding nodes", to "unhiding nodes", or not switch at all.

            bool willHideChildren = hideNodes ||
                                    importantForAccessibilityProp == ImportantForAccessibility.Yes ||
                                    importantForAccessibilityProp == ImportantForAccessibility.NoHideDescendants ||
                                    importantForAccessibilityProp == ImportantForAccessibility.Auto && hasLabelSet;

            bool traverseAllChildren;
            if (willHideChildren)
            {
                // Will be hiding children
                // Force a full traversal of children if current node hasn't been hiding them (or not sure),
                // else just follow the "dirty" nodes, if any
                traverseAllChildren = GetCurrentlyHidingChildren(element) != ElementAccessibilityContext.HidingChildren.Yes;
                SetChildrenAccessibilityViewAndNameForPeer(elementPeer, true, traverseAllChildren);
            }
            else
            {
                // Will be unhiding children
                // Force a full traversal of children if current node has been hiding them (or not sure),
                // else just follow the "dirty" nodes, if any
                traverseAllChildren = GetCurrentlyHidingChildren(element) != ElementAccessibilityContext.HidingChildren.No;
                SetChildrenAccessibilityViewAndNameForPeer(elementPeer, false, traverseAllChildren);
            }
               
            SetCurrentlyHidingChildren(element, willHideChildren ? ElementAccessibilityContext.HidingChildren.Yes : ElementAccessibilityContext.HidingChildren.No);

            //
            // Phase 3: set name if needed (all children nodes have been updated by this point)
            if (traverseAllChildren || GetDirty(element))
            {
                if (!hasLabelSet
                    && (importantForAccessibilityProp == ImportantForAccessibility.Yes
                        || importantForAccessibilityProp == ImportantForAccessibility.YesDontHideDescendants))
                {
                    // Set generated name
                    SetName(element, GenerateNameFromPeer(elementPeer));
                }
                else
                {
                    // Set name as the specified label (includes null as a special case)
                    SetName(element, accessibilityLabelProp);
                }
            }
        }

        /// <summary>
        /// Recursively sets the value of AccessibilityView and Name for children of <paramref name="elementPeer"/>
        /// based on ImportantForAccessibility and AccesibilityLabel
        /// </summary>
        /// <param name="elementPeer"></param>
        /// <param name="hideNodes"></param>
        /// <param name="traverseAllChildren"></param>
        private static void SetChildrenAccessibilityViewAndNameForPeer(AutomationPeer elementPeer, bool hideNodes, bool traverseAllChildren)
        {
            if (elementPeer?.GetChildren() == null)
            {
                return;
            }

            foreach (AutomationPeer childPeer in elementPeer.GetChildren())
            {
                UIElement child = GetUIElementFromAutomationPeer(childPeer);
                // Take into account just views created by React
                if (child == null || !child.HasTag())
                {
                    SetChildrenAccessibilityViewAndNameForPeer(childPeer, hideNodes, traverseAllChildren);
                }
                else
                {
                    if (traverseAllChildren || GetDirty(child))
                    {
                        UpdateAccessibilityViewAndNameForUIElement(child, childPeer, hideNodes);
                    }

                    SetDirty(child, false);
                }
            }
        }

        private static void SetName(UIElement element, string name)
        {
            if (!string.IsNullOrEmpty(name))
            {
                AutomationProperties.SetName(element, name);
            }
            else
            {
                element.ClearValue(AutomationProperties.NameProperty);
            }
            AnnounceIfNeeded(element);
        }

        /// <summary>
        /// Generates name from peer and list of children.
        /// Warning! It has clearing the Name AP as a side effect!
        /// </summary>
        /// <param name="peer"></param>
        /// <returns>Generated name.</returns>
        private static string GenerateNameFromPeer(AutomationPeer peer)
        {
            // Clear Name attached property to unhide any name generated by peer
            var element = GetUIElementFromAutomationPeer(peer);
            if (element != null)
            {
                element.ClearValue(AutomationProperties.NameProperty);
            }

            var ownName = peer.GetName();
            if (!string.IsNullOrEmpty(ownName))
            {
                // Own name present, we can use
                return ownName;
            }

            // Defer to children
            return GenerateNameFromChildren(peer.GetChildren());
        }

        /// <summary>
        /// Generates name from list of children.
        /// </summary>
        /// <param name="children"></param>
        /// <returns>Generated name.</returns>
        private static string GenerateNameFromChildren(IList<AutomationPeer> children)
        {
            if (children == null)
            {
                return "";
            }

            var result = new StringBuilder();
            foreach (var child in children)
            {
                var childElement = GetUIElementFromAutomationPeer(child);
                var isReactChild = childElement.HasTag();
                var i4a = isReactChild ? GetImportantForAccessibilityProp(childElement) : ImportantForAccessibility.Auto;
                var childResult = default(string);
                string label;
                switch (i4a)
                {
                    case ImportantForAccessibility.NoHideDescendants:
                        // The child is hidden from accessibility along with whole subtree.
                        break;
                    case ImportantForAccessibility.No:
                        // Ignore yourself even if AccessibilityLabel is set, go to children.
                        childResult = GenerateNameFromChildren(child.GetChildren());
                        break;
                    case ImportantForAccessibility.Yes:
                    case ImportantForAccessibility.YesDontHideDescendants:
                    case ImportantForAccessibility.Auto:
                        // Priority order is: AccessiblityLabel (if React element), control-provided name, children.
                        label = isReactChild ? GetAccessibilityLabelProp(childElement) : null;
                        if (!string.IsNullOrEmpty(label))
                        {
                            childResult = label;
                        }
                        else
                        {
                            // We don't use GenerateNameFromPeer because it affects the Name AP.
                            // We actually use the peer name here since it may already by updated by prior steps of the recursive traversal
                            var ownName = child.GetName();
                            if (!string.IsNullOrEmpty(ownName))
                            {
                                childResult = ownName;
                            }
                            else
                            {
                                childResult = GenerateNameFromChildren(child.GetChildren());
                            }
                        }
                        break;
                    default:
                        throw new InvalidOperationException($"Unknown ImportantForAccessibility value [{i4a}]");
                }
                if (!string.IsNullOrWhiteSpace(childResult))
                {
                    if (result.Length > 0)
                    {
                        result.Append(" ");
                    }
                    result.Append(childResult);
                }
            }
            return result.ToString();
        }

        /// <summary>
        /// If an UIElement owning the <paramref name="peer"/> can be determined, it is returned,
        /// otherwise returns null.
        /// </summary>
        /// <param name="peer"></param>
        /// <returns></returns>
        private static UIElement GetUIElementFromAutomationPeer(AutomationPeer peer)
        {
            UIElement ret = null;

            switch (peer)
            {
                case FrameworkElementAutomationPeer feAutomationPeer:
                    ret = feAutomationPeer.Owner;
                    break;
                case ItemAutomationPeer itemAutomationPeer:
                    ret = itemAutomationPeer.Item as UIElement;
                    break;
                default:
                    break;
            }

            return ret;
        }

        /// <summary>
        /// Helper parses the <paramref name="trait"/> into AccessibilityTrait enum.
        /// </summary>
        /// <param name="trait"></param>
        /// <returns>AccessibilityTrait enum</returns>
        private static AccessibilityTrait? ParseTrait(string trait)
        {
            if (EnumHelpers.TryParse<AccessibilityTrait>(trait, out var result))
            {
                return result;
            }

            return null;
        }
    }
}
