// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Microsoft.ReactNative.Managed
{
  class JSValueTreeWriter : IJSValueWriter
  {
    private State m_state = State.Start;
    private readonly Stack<StackEntry> m_stack = new Stack<StackEntry>();
    private object m_dynamic;
    private string m_propertyName;
    private JSValue m_result;

    public JSValue TakeValue()
    {
      return m_result;
    }

    public void WriteNull()
    {
      WriteValue(new JSValue());
    }

    public void WriteBoolean(bool value)
    {
      WriteValue(new JSValue(value));
    }

    public void WriteInt64(long value)
    {
      WriteValue(new JSValue(value));
    }

    public void WriteDouble(double value)
    {
      WriteValue(new JSValue(value));
    }

    public void WriteString(string value)
    {
      WriteValue(new JSValue(value));
    }

    public void WriteObjectBegin()
    {
      if (m_state == State.PropertyValue)
      {
        m_stack.Push(new StackEntry(m_state, (Dictionary<string, JSValue>)m_dynamic, m_propertyName));
      }
      else if (m_state == State.Array)
      {
        m_stack.Push(new StackEntry(m_state, (List<JSValue>)m_dynamic));
      }
      else if (m_state != State.Start)
      {
        throw new InvalidOperationException();
      }

      m_dynamic = new Dictionary<string, JSValue>();
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
        var obj = new JSValue(new ReadOnlyDictionary<string, JSValue>((Dictionary<string, JSValue>)m_dynamic));
        if (m_stack.Count == 0)
        {
          m_result = obj;
          m_state = State.Finish;
          return;
        }
        else
        {
          StackEntry entry = m_stack.Pop();
          if (entry.State == State.PropertyValue)
          {
            ((Dictionary<string, JSValue>)entry.Dynamic)[entry.PropertyName] = obj;
            m_dynamic = entry.Dynamic;
            m_state = State.PropertyName;
            return;
          }
          else if (entry.State == State.Array)
          {
            ((List<JSValue>)entry.Dynamic).Add(obj);
            m_dynamic = entry.Dynamic;
            m_state = State.Array;
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
        m_stack.Push(new StackEntry(m_state, (Dictionary<string, JSValue>)m_dynamic, m_propertyName));
      }
      else if (m_state == State.Array)
      {
        m_stack.Push(new StackEntry(m_state, (List<JSValue>)m_dynamic));
      }
      else if (m_state != State.Start)
      {
        throw new InvalidOperationException();
      }

      m_dynamic = new List<JSValue>();
      m_state = State.Array;
    }

    public void WriteArrayEnd()
    {
      if (m_state == State.Array)
      {
        var arr = new JSValue(new ReadOnlyCollection<JSValue>((List<JSValue>)m_dynamic));
        if (m_stack.Count == 0)
        {
          m_result = arr;
          m_state = State.Finish;
          return;
        }
        else
        {
          StackEntry entry = m_stack.Pop();
          if (entry.State == State.PropertyValue)
          {
            ((Dictionary<string, JSValue>)entry.Dynamic)[entry.PropertyName] = arr;
            m_dynamic = entry.Dynamic;
            m_state = State.PropertyName;
            return;
          }
          else if (entry.State == State.Array)
          {
            ((List<JSValue>)entry.Dynamic).Add(arr);
            m_dynamic = entry.Dynamic;
            m_state = State.Array;
            return;
          }
        }
      }

      throw new InvalidOperationException();
    }

    private enum State { Start, PropertyName, PropertyValue, Array, Finish };

    class StackEntry
    {
      public State State;
      public object Dynamic;
      public string PropertyName;

      public StackEntry(State state, Dictionary<string, JSValue> obj, string propertyName)
      {
        State = state;
        Dynamic = obj;
        PropertyName = propertyName;
      }

      public StackEntry(State state, List<JSValue> arr)
      {
        State = state;
        Dynamic = arr;
        PropertyName = null;
      }
    }

    private void WriteValue(JSValue value)
    {
      if (m_state == State.PropertyValue)
      {
        ((Dictionary<string, JSValue>)m_dynamic)[m_propertyName] = value;
        m_state = State.PropertyName;
      }
      else if (m_state == State.Array)
      {
        ((List<JSValue>)m_dynamic).Add(value);
      }
      else
      {
        m_result = value;
      }
    }
  }
}
