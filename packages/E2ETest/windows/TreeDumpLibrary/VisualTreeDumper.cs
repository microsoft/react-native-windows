// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

namespace TreeDumpLibrary
{
    public enum DumpTreeMode
    {
        Default,
        Json
    }

    public sealed class VisualTreeDumper
    {
        class Visitor
        {
            private readonly IVisualTreeLogger _logger;
            private int _indent;
            private readonly DefaultFilter _filter;
            private readonly IPropertyValueTranslator _translator;
            public Visitor(DefaultFilter filter, IPropertyValueTranslator translator, IVisualTreeLogger logger)
            {
                _indent = 0;
                _filter = filter;
                _translator = translator;
                _logger = logger;
            }
            public void EndVisitNode(DependencyObject obj, bool isLast)
            {
                _indent--;
                _logger.EndNode(_indent, obj.GetType().FullName, obj, isLast);
            }

            public void BeginVisitNode(DependencyObject obj)
            {
                _logger.BeginNode(_indent, obj.GetType().FullName, obj);
                _indent++;
            }

            public override string ToString()
            {
                return _logger.ToString();
            }

            public bool ShouldVisitPropertiesForNode(DependencyObject node)
            {
                return (node as UIElement) != null;
            }

            public bool ShouldVisitProperty(PropertyInfo propertyInfo)
            {
                return _filter.ShouldVisitProperty(propertyInfo.Name);
            }

            public void VisitProperty(string propertyName, object value, bool isLast)
            {
                var v = _translator.PropertyValueToString(propertyName, value);
                _logger.LogProperty(_indent + 1, propertyName, v, isLast);
                if (_filter.ShouldVisitPropertyValue(v))
                {

                }
                else
                {

                }
            }

            public void BeginChildren()
            {
                _logger.BeginArray(++_indent, "children");
            }

            public void EndChildren()
            {
                _logger.EndArray(_indent--, "children");
            }

            public bool ShouldVisitPropertyValue(string propertyName, object value)
            {
                string s = _translator.PropertyValueToString(propertyName, value);
                return _filter.ShouldVisitPropertyValue(s);
            }
        }

        public static string DumpTree(DependencyObject root, DependencyObject excludedNode, IList<string> additionalProperties, DumpTreeMode mode)
        {
            var propertyFilter = new DefaultFilter();
            ((List<string>)propertyFilter.PropertyNameAllowList).AddRange(additionalProperties);

            IPropertyValueTranslator translator = (mode == DumpTreeMode.Json ?
                                                    new JsonPropertyValueTranslator() as IPropertyValueTranslator :
                                                    new DefaultPropertyValueTranslator());
            IVisualTreeLogger logger = (mode == DumpTreeMode.Json ?
                                        new JsonVisualTreeLogger() as IVisualTreeLogger :
                                        new DefaultVisualTreeLogger());
            Visitor visitor = new Visitor(propertyFilter, translator, logger);

            WalkThroughTree(root, excludedNode, visitor);

            return visitor.ToString();
        }

        private static void WalkThroughProperties(DependencyObject node, Visitor visitor, bool hasChildren)
        {
            if (visitor.ShouldVisitPropertiesForNode(node))
            {
                var properties = (from property in node.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance)
                                  where visitor.ShouldVisitProperty(property) &&
                                        visitor.ShouldVisitPropertyValue(property.Name, GetObjectProperty(node, property))
                                  orderby property.Name
                                  select property).ToArray();

                for (int i = 0; i < properties.Length; i++)
                {
                    var property = properties[i];
                    object value = null;
                    value = GetObjectProperty(node, property);
                    bool isLast = (i == properties.Length - 1) && !hasChildren;
                    visitor.VisitProperty(property.Name, value, isLast);
                }
            }
        }

        private static object GetObjectProperty(DependencyObject node, PropertyInfo property)
        {
            object value;
            try
            {
                value = property.GetValue(node);
            }
            catch (Exception e)
            {
                value = "Exception when reading " + property.Name + e.ToString();
            }

            return value;
        }

        private static void WalkThroughTree(DependencyObject node, DependencyObject excludedNode, Visitor visitor, bool isLast = true)
        {
            if (node != null)
            {
                visitor.BeginVisitNode(node);

                var childrenCount = VisualTreeHelper.GetChildrenCount(node);
                WalkThroughProperties(node, visitor, childrenCount != 0);
                if (childrenCount != 0)
                {
                    visitor.BeginChildren();
                    for (int i = 0; i < childrenCount; i++)
                    {
                        var child = VisualTreeHelper.GetChild(node, i);
                        if (child != excludedNode)
                        {
                            bool isLastChild = (i == childrenCount - 1);
                            WalkThroughTree(child, excludedNode, visitor, isLastChild);
                        }
                    }
                    visitor.EndChildren();
                }
                visitor.EndVisitNode(node, isLast);
            }
        }
    }
    public sealed class DefaultFilter
    {
        public IList<string> PropertyNameAllowList { get; set; }

        public DefaultFilter()
        {
            PropertyNameAllowList = new List<string> {"Foreground", "Background", "Padding", "Margin", "RenderSize", "Visibility", "CornerRadius", "BorderThickness",
            "Width", "Height", "BorderBrush", "VerticalAlignment", "HorizontalAlignment", "Clip", /*"ActualOffset" 19h1*/};
        }

        public bool ShouldVisitPropertyValue(string propertyValue)
        {
            return !string.IsNullOrEmpty(propertyValue) && !propertyValue.Equals("NaN") && !propertyValue.StartsWith("Exception");
        }

        public bool ShouldVisitProperty(string propertyName)
        {
            return (PropertyNameAllowList.Contains(propertyName));
        }
    }
    public sealed class DefaultPropertyValueTranslator : IPropertyValueTranslator
    {
        public string PropertyValueToString(string propertyName, object propertyObject)
        {
            if (propertyObject == null)
            {
                return "[NULL]";
            }

            if (propertyObject is SolidColorBrush)
            {
                return (propertyObject as SolidColorBrush).Color.ToString();
            }
            return propertyObject.ToString();
        }
    }

    public sealed class JsonPropertyValueTranslator : IPropertyValueTranslator
    {
        public string PropertyValueToString(string propertyName, object propertyObject)
        {
            if (propertyObject == null)
            {
                return "null";
            }

            if (propertyObject is SolidColorBrush)
            {
                return (propertyObject as SolidColorBrush).Color.ToString();
            }
            return propertyObject.ToString();
        }
    }
}
