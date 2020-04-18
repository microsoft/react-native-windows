// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;

namespace TreeDumpLibrary
{
    public interface IVisualTreeLogger
    {
        void BeginNode(int indent, string nodeName, DependencyObject obj, bool hasProperties);
        void EndNode(int indent, string nodeName, DependencyObject obj, bool isLast);
        void LogProperty(int indent, string propertyName, object propertyValue, bool isLast);
        string ToString();
        void BeginArray(int indent, string propertyName);
        void EndArray(int indent, string propertyName);
    }
}
