// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace TreeDumpLibrary
{
    public sealed  class VisualTreeDumper
    {
        class Visitor
        {
            private DefaultVisualTreeLogger _logger;
            private int _indent;
            private DefaultFilter _filter;
            private DefaultPropertyValueTranslator _translator;
            public Visitor(DefaultFilter filter, DefaultPropertyValueTranslator translator, DefaultVisualTreeLogger logger)
            {
                _indent = 0;
                _filter = filter;
                _translator = translator;
                _logger = logger;
            }
            public void EndVisitNode(DependencyObject obj)
            {
                _indent--;
                _logger.EndNode(_indent, obj.GetType().FullName, obj);
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

            public void VisitProperty(string propertyName, object value)
            {
                var v = _translator.PropertyValueToString(propertyName, value);
                if (_filter.ShouldVisitPropertyValue(v))
                {
                    _logger.LogProperty(_indent + 1, propertyName, v);
                }
            }
        }

        public static string DumpTree(DependencyObject root, DependencyObject excludedNode, IList<string> additionalProperties)
        {
            var propertyFilter = new DefaultFilter();
            ((List<string>)propertyFilter.PropertyNameAllowList).AddRange(additionalProperties);

            Visitor visitor = new Visitor(propertyFilter,
                new DefaultPropertyValueTranslator(),
                new DefaultVisualTreeLogger());
            WalkThroughTree(root, excludedNode, visitor);

            return visitor.ToString();
        }

        private static void WalkThroughProperties(DependencyObject node, Visitor visitor)
        {
            if (visitor.ShouldVisitPropertiesForNode(node))
            {
                var properties = node.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance).OrderBy(x => x.Name);
                foreach (var property in properties)
                {
                    if (visitor.ShouldVisitProperty(property))
                    {
                        Object value = null;

                        try
                        {
                            value = property.GetValue(obj: node, index: null);
                        }
                        catch (Exception e)
                        {
                            value = "Exception when read " + property.Name + e.ToString();
                        }
                        visitor.VisitProperty(property.Name, value);
                    }
                }
            }
        }
        private static void WalkThroughTree(DependencyObject node, DependencyObject excludedNode, Visitor visitor)
        {
            if (node != null)
            {
                visitor.BeginVisitNode(node);

                WalkThroughProperties(node, visitor);
                for (int i = 0; i < VisualTreeHelper.GetChildrenCount(node); i++)
                {
                    var child = VisualTreeHelper.GetChild(node, i);
                    if (child != excludedNode)
                    {
                        WalkThroughTree(child, excludedNode, visitor);
                    }
                }

                visitor.EndVisitNode(node);
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
    public sealed class DefaultPropertyValueTranslator
    {
        public string PropertyValueToString(string propertyName, object propertyObject)
        {
            if (propertyObject == null)
            {
                return "[NULL]";
            }

            var brush = propertyObject as SolidColorBrush;
            if (brush != null)
            {
                return brush.Color.ToString();
            }
            return propertyObject.ToString();
        }
    }
    public sealed class DefaultVisualTreeLogger
    {
        public void BeginNode(int indent, string nodeName, DependencyObject obj)
        {
            AppendLogger(indent, string.Format("[{0}]", nodeName));
        }

        public void EndNode(int indent, string nodeName, DependencyObject obj)
        {
        }

        public void LogProperty(int indent, string propertyName, object propertyValue)
        {
            AppendLogger(indent, string.Format("{0}={1}", propertyName, propertyValue));
        }

        public override string ToString()
        {
            return _logger.ToString();
        }

        private StringBuilder _logger = new StringBuilder();
        private void AppendLogger(int indent, string s)
        {
            _logger.AppendLine(s.PadLeft(2 * indent + s.Length));
        }
    }
}
