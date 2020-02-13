// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;

namespace Microsoft.ReactNative.Managed
{
  // JSValue represents an immutable JavaScript value passed as a parameter.
  // It is created to simplify working with IJSValueReader.
  //
  // JSValue is designed to minimize number of memory allocations:
  // - It is a struct that avoid extra memory allocations when it is stored in a container.
  // - It avoids boxing simple values by using a union type to store them.
  // The JSValue is an immutable and is safe to be used from multiple threads.
  // It does not implement GetHashCode() and must not be used as a key in a dictionary.
  struct JSValue : IEquatable<JSValue>
  {
    public static readonly JSValue Null = new JSValue();

    public struct Void { };

    private readonly SimpleValue m_simpleValue;
    private readonly object m_objValue;

    public JSValue(IReadOnlyDictionary<string, JSValue> value)
    {
      Type = JSValueType.Object;
      m_simpleValue = new SimpleValue();
      m_objValue = value;
    }

    public JSValue(IReadOnlyList<JSValue> value)
    {
      Type = JSValueType.Array;
      m_simpleValue = new SimpleValue();
      m_objValue = value;
    }

    public JSValue(string value)
    {
      Type = JSValueType.String;
      m_simpleValue = new SimpleValue();
      m_objValue = value;
    }

    public JSValue(bool value)
    {
      Type = JSValueType.Boolean;
      m_simpleValue = new SimpleValue { BooleanValue = value };
      m_objValue = null;
    }

    public JSValue(long value)
    {
      Type = JSValueType.Int64;
      m_simpleValue = new SimpleValue { Int64Value = value };
      m_objValue = null;
    }

    public JSValue(double value)
    {
      Type = JSValueType.Double;
      m_simpleValue = new SimpleValue { DoubleValue = value };
      m_objValue = null;
    }

    public JSValueType Type { get; }

    public bool IsNull => Type == JSValueType.Null;

    public IReadOnlyDictionary<string, JSValue> Object => (IReadOnlyDictionary<string, JSValue>)m_objValue;
    public IReadOnlyList<JSValue> Array => (IReadOnlyList<JSValue>)m_objValue;
    public string String => (string)m_objValue;
    public bool Boolean => m_simpleValue.BooleanValue;
    public long Int64 => m_simpleValue.Int64Value;
    public double Double => m_simpleValue.DoubleValue;

    public T To<T>() => (new JSValueTreeReader(this)).ReadValue<T>();

    public JSValue From<T>(T value)
    {
      var writer = new JSValueTreeWriter();
      writer.WriteValue(value);
      return writer.TakeValue();
    }

    public bool TryGetObjectProperty(string propertyName, out JSValue value)
    {
      if (Type == JSValueType.Object && Object.TryGetValue(propertyName, out value))
      {
        return true;
      }

      value = Null;
      return false;
    }

    public static bool operator ==(JSValue lhs, JSValue rhs)
    {
      return lhs.ValueEquals(rhs);
    }

    public static bool operator !=(JSValue lhs, JSValue rhs)
    {
      return !lhs.ValueEquals(rhs);
    }

    public bool Equals(JSValue other)
    {
      return ValueEquals(other);
    }

    private bool ValueEquals(JSValue other)
    {
      if (Type == other.Type)
      {
        switch (Type)
        {
          case JSValueType.Null: return true;
          case JSValueType.Object: return ObjectEquals(other.Object);
          case JSValueType.Array: return ArrayEquals(other.Array);
          case JSValueType.String: return String == other.String;
          case JSValueType.Boolean: return Boolean == other.Boolean;
          case JSValueType.Int64: return Int64 == other.Int64;
          case JSValueType.Double: return Double == other.Double;
          default: return false;
        }
      }

      return false;
    }

    private bool ObjectEquals(IReadOnlyDictionary<string, JSValue> other)
    {
      var obj = Object;
      if (obj.Count != other.Count)
      {
        return false;
      }

      foreach (var entry in obj)
      {
        if (!other.TryGetValue(entry.Key, out var value) || !value.ValueEquals(entry.Value))
        {
          return false;
        }
      }

      return true;
    }

    private bool ArrayEquals(IReadOnlyList<JSValue> other)
    {
      var arr = Array;
      if (arr.Count != other.Count)
      {
        return false;
      }

      for (int i = 0; i < arr.Count; ++i)
      {
        if (!arr[i].ValueEquals(other[i]))
        {
          return false;
        }
      }

      return true;
    }

    public override bool Equals(object obj)
    {
      return (obj is JSValue) && Equals((JSValue)obj);
    }

    public override int GetHashCode()
    {
      throw new NotImplementedException("JSValue currently does not support hash codes");
    }

    public static JSValue ReadFrom(IJSValueReader reader)
    {
      var treeReader = reader as IJSValueTreeReader;
      if (treeReader != null)
      {
        return treeReader.Current;
      }

      return ReadValue(reader);
    }

    public static Dictionary<string, JSValue> ReadObjectPropertiesFrom(IJSValueReader reader)
    {
      if (reader.ValueType == JSValueType.Object)
      {
        var treeReader = reader as IJSValueTreeReader;
        if (treeReader != null)
        {
          return new Dictionary<string, JSValue>(treeReader.Current.Object as IDictionary<string, JSValue>);
        }

        return ReadObjectProperties(reader);
      }

      return new Dictionary<string, JSValue>();
    }

    public static List<JSValue> ReadArrayItemsFrom(IJSValueReader reader)
    {
      if (reader.ValueType == JSValueType.Array)
      {
        var treeReader = reader as IJSValueTreeReader;
        if (treeReader != null)
        {
          return new List<JSValue>(treeReader.Current.Array as IList<JSValue>);
        }

        return ReadArrayItems(reader);
      }

      return new List<JSValue>();
    }

    private static JSValue ReadValue(IJSValueReader reader)
    {
      switch (reader.ValueType)
      {
        case JSValueType.Null: return Null;
        case JSValueType.Object: return ReadObject(reader);
        case JSValueType.Array: return ReadArray(reader);
        case JSValueType.String: return new JSValue(reader.GetString());
        case JSValueType.Boolean: return new JSValue(reader.GetBoolean());
        case JSValueType.Int64: return new JSValue(reader.GetInt64());
        case JSValueType.Double: return new JSValue(reader.GetDouble());
        default: throw new ReactException("Unexpected JSValueType");
      }
    }

    private static JSValue ReadObject(IJSValueReader reader)
    {
      return new JSValue(new ReadOnlyDictionary<string, JSValue>(ReadObjectProperties(reader)));
    }

    private static JSValue ReadArray(IJSValueReader reader)
    {
      return new JSValue(new ReadOnlyCollection<JSValue>(ReadArrayItems(reader)));
    }

    private static Dictionary<string, JSValue> ReadObjectProperties(IJSValueReader reader)
    {
      var properties = new Dictionary<string, JSValue>();
      while (reader.GetNextObjectProperty(out string propertyName))
      {
        properties.Add(propertyName, ReadValue(reader));
      }

      return properties;
    }

    private static List<JSValue> ReadArrayItems(IJSValueReader reader)
    {
      var items = new List<JSValue>();
      while (reader.GetNextArrayItem())
      {
        items.Add(ReadValue(reader));
      }

      return items;
    }

    public void WriteTo(IJSValueWriter writer)
    {
      switch (Type)
      {
        case JSValueType.Null: writer.WriteNull(); break;
        case JSValueType.Object: WriteObject(writer, Object); break;
        case JSValueType.Array: WriteArray(writer, Array); break;
        case JSValueType.String: writer.WriteString(String); break;
        case JSValueType.Boolean: writer.WriteBoolean(Boolean); break;
        case JSValueType.Int64: writer.WriteInt64(Int64); break;
        case JSValueType.Double: writer.WriteDouble(Double); break;
      }
    }

    public static void WriteObject(IJSValueWriter writer, IEnumerable<KeyValuePair<string, JSValue>> value)
    {
      writer.WriteObjectBegin();
      foreach (var keyValue in value)
      {
        writer.WritePropertyName(keyValue.Key);
        keyValue.Value.WriteTo(writer);
      }
      writer.WriteObjectEnd();
    }

    public static void WriteArray(IJSValueWriter writer, IEnumerable<JSValue> value)
    {
      writer.WriteArrayBegin();
      foreach (var item in value)
      {
        item.WriteTo(writer);
      }
      writer.WriteArrayEnd();
    }

    // This is a 'union' type that may store in the same location bool, long, or double.
    [StructLayout(LayoutKind.Explicit)]
    private struct SimpleValue
    {
      [FieldOffset(0)] public bool BooleanValue;
      [FieldOffset(0)] public long Int64Value;
      [FieldOffset(0)] public double DoubleValue;
    }
  }
}
