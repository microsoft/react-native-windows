// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System.Collections.Generic;

namespace Microsoft.ReactNative.Managed.UnitTests
{
  class JTokenJSValueReader : IJSValueReader
  {
    private JToken m_current;
    private bool m_isIterating = false;
    private readonly Stack<StackEntry> m_stack = new Stack<StackEntry>();

    public JTokenJSValueReader(JToken root)
    {
      m_current = root;
    }

    public JSValueType ValueType
    {
      get
      {
        switch (m_current.Type)
        {
          case JTokenType.Null:
            return JSValueType.Null;
          case JTokenType.Object:
            return JSValueType.Object;
          case JTokenType.Array:
            return JSValueType.Array;
          case JTokenType.String:
            return JSValueType.String;
          case JTokenType.Boolean:
            return JSValueType.Boolean;
          case JTokenType.Integer:
            return JSValueType.Int64;
          case JTokenType.Float:
            return JSValueType.Double;
          default:
            return JSValueType.Null;
        }
      }
    }

    public bool GetNextObjectProperty(out string propertyName)
    {
      if (!m_isIterating)
      {
        if (m_current.Type == JTokenType.Object)
        {
          var properties = ((JObject)m_current).Properties().GetEnumerator();
          if (properties.MoveNext())
          {
            m_stack.Push(StackEntry.ObjectProperty(m_current, properties));
            SetCurrentValue(properties.Current.Value);
            propertyName = properties.Current.Name;
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
        if (entry.Value.Type == JTokenType.Object)
        {
          if (entry.Property.MoveNext())
          {
            SetCurrentValue(entry.Property.Current.Value);
            propertyName = entry.Property.Current.Name;
            return true;
          }
          else
          {
            m_current = entry.Value;
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
        if (m_current.Type == JTokenType.Array)
        {
          var items = ((JArray)m_current).GetEnumerator();
          if (items.MoveNext())
          {
            m_stack.Push(StackEntry.ArrayItem(m_current, items));
            SetCurrentValue(items.Current);
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
        var entry = m_stack.Peek();
        if (entry.Value.Type == JTokenType.Array)
        {
          if (entry.Item.MoveNext())
          {
            SetCurrentValue(entry.Item.Current);
            return true;
          }
          else
          {
            m_current = entry.Value;
            m_stack.Pop();
            m_isIterating = m_stack.Count != 0;
          }
        }
      }

      return false;
    }

    private void SetCurrentValue(JToken value)
    {
      m_current = value;
      switch (value.Type)
      {
        case JTokenType.Object:
        case JTokenType.Array:
          m_isIterating = false;
          break;
        default:
          m_isIterating = true;
          break;
      }
    }

    public string GetString()
    {
      return (m_current.Type == JTokenType.String) ? (string)(JValue)m_current : "";
    }

    public bool GetBoolean()
    {
      return (m_current.Type == JTokenType.Boolean) ? (bool)(JValue)m_current : false;
    }

    public long GetInt64()
    {
      return (m_current.Type == JTokenType.Integer) ? (long)(JValue)m_current : 0;
    }

    public double GetDouble()
    {
      return (m_current.Type == JTokenType.Float) ? (double)(JValue)m_current : 0;
    }

    private struct StackEntry
    {
      public static StackEntry ObjectProperty(JToken value, IEnumerator<JProperty> property)
      {
        return new StackEntry
        {
          Value = value,
          Item = null,
          Property = property
        };
      }

      public static StackEntry ArrayItem(JToken value, IEnumerator<JToken> item)
      {
        return new StackEntry
        {
          Value = value,
          Item = item,
          Property = null
        };
      }

      public JToken Value;
      public IEnumerator<JToken> Item;
      public IEnumerator<JProperty> Property;
    }
  }
}
