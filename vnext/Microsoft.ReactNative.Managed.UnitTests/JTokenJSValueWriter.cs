// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Microsoft.ReactNative.Managed.UnitTests
{
  class JTokenJSValueWriter : IJSValueWriter
  {
    private State m_state = State.Start;
    private List<StackEntry> m_stack = new List<StackEntry>();
    private JToken m_dynamic;
    private string m_propertyName;
    private JToken m_result;

    public JToken TakeValue()
    {
      return m_result;
    }

    public void WriteNull()
    {
      WriteValue(new JValue((object)null));
    }

    public void WriteBoolean(bool value)
    {
      WriteValue(new JValue(value));
    }

    public void WriteInt64(long value)
    {
      WriteValue(new JValue(value));
    }

    public void WriteDouble(double value)
    {
      WriteValue(new JValue(value));
    }

    public void WriteString(string value)
    {
      WriteValue(new JValue(value));
    }

    public void WriteObjectBegin()
    {
      if (m_state == State.PropertyValue)
      {
        m_stack.Add(new StackEntry(m_state, (JObject)m_dynamic, m_propertyName));
      }
      else if (m_state == State.Array)
      {
        m_stack.Add(new StackEntry(m_state, (JArray)m_dynamic));
      }
      else if (m_state != State.Start)
      {
        throw new InvalidOperationException();
      }

      m_dynamic = new JObject();
      m_state = State.PropertyName;
    }

    public void WritePropertyName(string name)
    {
      if (m_state == State.PropertyName)
      {
        m_propertyName = name;
        m_state = State.PropertyValue;
      }
      else
      {
        throw new InvalidOperationException();
      }
    }

    public void WriteObjectEnd()
    {
      if (m_state == State.PropertyName)
      {
        if (m_stack.Count == 0)
        {
          m_result = m_dynamic;
          m_state = State.Finish;
          return;
        }
        else
        {
          StackEntry entry = m_stack.Last();
          if (entry.State == State.PropertyValue)
          {
            ((JObject)entry.Dynamic)[entry.PropertyName] = m_dynamic;
            m_dynamic = entry.Dynamic;
            m_state = State.PropertyName;
            m_stack.RemoveAt(m_stack.Count - 1);
            return;
          }
          else if (entry.State == State.Array)
          {
            entry.Dynamic.Add(m_dynamic);
            m_dynamic = entry.Dynamic;
            m_state = State.Array;
            m_stack.RemoveAt(m_stack.Count - 1);
            return;
          }
        }
      }

      throw new InvalidOperationException();
    }

    public void WriteArrayBegin()
    {
      if (m_state == State.PropertyValue)
      {
        m_stack.Add(new StackEntry(m_state, (JObject)m_dynamic, m_propertyName));
      }
      else if (m_state == State.Array)
      {
        m_stack.Add(new StackEntry(m_state, (JArray)m_dynamic));
      }
      else if (m_state != State.Start)
      {
        throw new InvalidOperationException();
      }

      m_dynamic = new JArray();
      m_state = State.Array;
    }

    public void WriteArrayEnd()
    {
      if (m_state == State.Array)
      {
        if (m_stack.Count == 0)
        {
          m_result = m_dynamic;
          m_state = State.Finish;
          return;
        }
        else
        {
          StackEntry entry = m_stack.Last();
          if (entry.State == State.PropertyValue)
          {
            ((JObject)entry.Dynamic)[entry.PropertyName] = m_dynamic;
            m_dynamic = entry.Dynamic;
            m_state = State.PropertyName;
            m_stack.RemoveAt(m_stack.Count - 1);
            return;
          }
          else if (entry.State == State.Array)
          {
            entry.Dynamic.Add(m_dynamic);
            m_dynamic = entry.Dynamic;
            m_state = State.Array;
            m_stack.RemoveAt(m_stack.Count - 1);
            return;
          }
        }
      }

      throw new InvalidOperationException();
    }

    private enum State { Start, PropertyName, PropertyValue, Array, Finish };

    class StackEntry
    {
      public StackEntry(State state, JObject obj, string propertyName)
      {
        State = state;
        Dynamic = obj;
        PropertyName = propertyName;
      }

      public StackEntry(State state, JArray arr)
      {
        State = state;
        Dynamic = arr;
        PropertyName = null;
      }

      public State State;
      public JContainer Dynamic;
      public string PropertyName;
    }

    private void WriteValue(JToken value)
    {
      if (m_state == State.PropertyValue)
      {
        m_dynamic[m_propertyName] = value;
        m_state = State.PropertyName;
      }
      else if (m_state == State.Array)
      {
        ((JArray)m_dynamic).Add(value);
      }
    }
  }
}
