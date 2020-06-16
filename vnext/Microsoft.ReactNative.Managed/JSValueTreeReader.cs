// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Collections.Generic;

namespace Microsoft.ReactNative.Managed
{
  public class JSValueTreeReader : IJSValueReader, IJSValueTreeReader
  {
    private bool m_isIterating = false;
    private readonly Stack<StackEntry> m_stack = new Stack<StackEntry>();

    public JSValueTreeReader(JSValue root)
    {
      Root = root;
      Current = root;
    }

    public JSValueType ValueType => Current.Type;

    public JSValue Current { get; private set; }

    public JSValue Root { get; private set; }

    public bool GetNextObjectProperty(out string propertyName)
    {
      if (!m_isIterating)
      {
        if (Current.TryGetObject(out var currentObject))
        {
          var properties = currentObject.GetEnumerator();
          if (properties.MoveNext())
          {
            m_stack.Push(StackEntry.ObjectProperty(Current, properties));
            SetCurrentValue(properties.Current.Value);
            propertyName = properties.Current.Key;
            return true;
          }
          else
          {
            m_isIterating = m_stack.Count != 0;
          }
        }
      }
      else if (m_stack.Count != 0)
      {
        StackEntry entry = m_stack.Peek();
        if (entry.Value.Type == JSValueType.Object)
        {
          if (entry.Property.MoveNext())
          {
            SetCurrentValue(entry.Property.Current.Value);
            propertyName = entry.Property.Current.Key;
            return true;
          }
          else
          {
            Current = entry.Value;
            m_stack.Pop();
            m_isIterating = m_stack.Count != 0;
          }
        }
      }

      propertyName = "";
      return false;
    }

    public bool GetNextArrayItem()
    {
      if (!m_isIterating)
      {
        if (Current.TryGetArray(out var currentArray))
        {
          var items = currentArray.GetEnumerator();
          if (items.MoveNext())
          {
            m_stack.Push(StackEntry.ArrayItem(Current, items));
            SetCurrentValue(items.Current);
            return true;
          }
        }
        else
        {
          m_isIterating = m_stack.Count != 0;
        }

      }
      else if (m_stack.Count != 0)
      {
        var entry = m_stack.Peek();
        if (entry.Value.Type == JSValueType.Array)
        {
          if (entry.Item.MoveNext())
          {
            SetCurrentValue(entry.Item.Current);
            return true;
          }
          else
          {
            Current = entry.Value;
            m_stack.Pop();
            m_isIterating = m_stack.Count != 0;
          }
        }
      }

      return false;
    }

    private void SetCurrentValue(JSValue value)
    {
      Current = value;
      switch (value.Type)
      {
        case JSValueType.Object:
        case JSValueType.Array:
          m_isIterating = false;
          break;
        default:
          m_isIterating = true;
          break;
      }
    }

    public string GetString()
    {
      return Current.TryGetString(out var stringValue) ? stringValue : "";
    }

    public bool GetBoolean()
    {
      return Current.TryGetBoolean(out var booleanValue) ? booleanValue : false;
    }

    public long GetInt64()
    {
      return Current.TryGetInt64(out var int64Value) ? int64Value : 0;
    }

    public double GetDouble()
    {
      return Current.TryGetDouble(out var doubleValue) ? doubleValue : 0;
    }

    private struct StackEntry
    {
      public JSValue Value;
      public IEnumerator<JSValue> Item;
      public IEnumerator<KeyValuePair<string, JSValue>> Property;

      public static StackEntry ObjectProperty(JSValue value, IEnumerator<KeyValuePair<string, JSValue>> property)
      {
        return new StackEntry
        {
          Value = value,
          Item = null,
          Property = property
        };
      }

      public static StackEntry ArrayItem(JSValue value, IEnumerator<JSValue> item)
      {
        return new StackEntry
        {
          Value = value,
          Item = item,
          Property = null
        };
      }
    }
  }
}
