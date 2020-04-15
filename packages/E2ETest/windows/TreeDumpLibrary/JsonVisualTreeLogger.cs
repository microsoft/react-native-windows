// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Text;
using Windows.UI.Xaml;

namespace TreeDumpLibrary
{
    public sealed class JsonVisualTreeLogger : IVisualTreeLogger
    {
        private readonly StringBuilder _logger = new StringBuilder();
        public void BeginArray(int indent, string propertyName)
        {
            AppendLogger(indent, $"\"{propertyName}\": [");
        }

        public void BeginNode(int indent, string nodeName, DependencyObject obj, bool hasProperties)
        {
            AppendLogger(indent, "{");
            LogProperty(indent + 2, "XamlType", JsonPropertyValueTranslator.Quote(nodeName), !hasProperties);
        }

        public void EndArray(int indent, string propertyName)
        {
            AppendLogger(indent, "]");
        }

        public void EndNode(int indent, string nodeName, DependencyObject obj, bool isLast)
        {
            AppendLogger(indent, $"}}{GetDelimiter(isLast)}");
        }

        private string GetDelimiter(bool isLast)
        {
            return isLast ? "" : ",";
        }

        public void LogProperty(int indent, string propertyName, object propertyValue, bool isLast)
        {
            AppendLogger(indent, $"\"{propertyName}\": {propertyValue}{GetDelimiter(isLast)}");
        }

        public override string ToString()
        {
            return _logger.ToString();
        }

        private void AppendLogger(int indent, string s)
        {
            _logger.AppendLine(s.PadLeft(2 * indent + s.Length));
        }
    }
}
