// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;
using System.Text;
using Windows.UI.Notifications;

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

    public static implicit operator JSValue(string value) => new JSValue(value);
    public static implicit operator JSValue(bool value) => new JSValue(value);
    public static implicit operator JSValue(sbyte value) => new JSValue(value);
    public static implicit operator JSValue(short value) => new JSValue(value);
    public static implicit operator JSValue(int value) => new JSValue(value);
    public static implicit operator JSValue(long value) => new JSValue(value);
    public static implicit operator JSValue(byte value) => new JSValue(value);
    public static implicit operator JSValue(ushort value) => new JSValue(value);
    public static implicit operator JSValue(uint value) => new JSValue(value);
    public static implicit operator JSValue(ulong value) => new JSValue((long)value);
    public static implicit operator JSValue(float value) => new JSValue(value);
    public static implicit operator JSValue(double value) => new JSValue(value);

    public JSValueType Type { get; }

    public bool IsNull => Type == JSValueType.Null;

    public IReadOnlyDictionary<string, JSValue> Object => (IReadOnlyDictionary<string, JSValue>)m_objValue;
    public IReadOnlyList<JSValue> Array => (IReadOnlyList<JSValue>)m_objValue;
    public string String => (string)m_objValue;
    public bool Boolean => m_simpleValue.BooleanValue;
    public long Int64 => m_simpleValue.Int64Value;
    public double Double => m_simpleValue.DoubleValue;

    public static explicit operator string(JSValue value)
    {
      switch (value.Type)
      {
        case JSValueType.String: return value.String;
        case JSValueType.Boolean: return value.Boolean ? "true" : "false"; 
        case JSValueType.Int64: return value.Int64.ToString(); 
        case JSValueType.Double: return value.Double.ToString();
        default: return "";
      }
    }

    public static explicit operator bool(JSValue value)
    {
      switch (value.Type)
      {
        case JSValueType.Object: return true;
        case JSValueType.Array: return true;
        case JSValueType.String: return !string.IsNullOrEmpty(value.String);
        case JSValueType.Boolean: return value.Boolean;
        case JSValueType.Int64: return value.Int64 != 0;
        case JSValueType.Double: return value.Double != 0;
        default: return false;
      }
    }

    public static explicit operator sbyte(JSValue value)
    {
      return (sbyte)(long)value;
    }

    public static explicit operator short(JSValue value)
    {
      return (short)(long)value;
    }

    public static explicit operator int(JSValue value)
    {
      return (int)(long)value;
    }

    public static explicit operator long(JSValue value)
    {
      switch (value.Type)
      {
        case JSValueType.String: return long.TryParse(value.String, out long result) ? result : 0;
        case JSValueType.Boolean: return value.Boolean ? 1 : 0;
        case JSValueType.Int64: return value.Int64;
        case JSValueType.Double: return (long)value.Double;
        default: return 0;
      }
    }

    public static explicit operator byte(JSValue value)
    {
      return (byte)(long)value;
    }

    public static explicit operator ushort(JSValue value)
    {
      return (ushort)(long)value;
    }

    public static explicit operator uint(JSValue value)
    {
      return (uint)(long)value;
    }

    public static explicit operator ulong(JSValue value)
    {
      return (ulong)(long)value;
    }

    public static explicit operator float(JSValue value)
    {
      return (float)(double)value;
    }

    public static explicit operator double(JSValue value)
    {
      switch (value.Type)
      {
        case JSValueType.String: return double.TryParse(value.String, out double result) ? result : 0;
        case JSValueType.Boolean: return value.Boolean ? 1 : 0;
        case JSValueType.Int64: return value.Int64;
        case JSValueType.Double: return value.Double;
        default: return 0;
      }
    }

    public T To<T>() => (new JSValueTreeReader(this)).ReadValue<T>();

    public JSValue From<T>(T value)
    {
      var writer = new JSValueTreeWriter();
      writer.WriteValue(value);
      return writer.TakeValue();
    }

    public override string ToString()
    {
      switch (Type)
      {
        case JSValueType.Null: return "null";
        case JSValueType.Object:
          {
            var sb = new StringBuilder();
            sb.Append("{");
            foreach (var prop in Object)
            {
              sb.Append(prop.Key);
              sb.Append(": ");
              sb.Append(prop.Value.ToString());
              sb.Append(", ");
            }
            sb.Length -= 2;
            sb.Append("}");
            return sb.ToString();
          }
        case JSValueType.Array:
          {
            var sb = new StringBuilder();
            sb.Append("[");
            foreach (var item in Array)
            {
              sb.Append(item.ToString());
              sb.Append(", ");
            }
            sb.Length -= 2;
            sb.Append("]");
            return sb.ToString();
          }
        case JSValueType.String: return "\"" + String + "\"";
        case JSValueType.Boolean: return Boolean ? "true": "false";
        case JSValueType.Int64: return Int64.ToString();
        case JSValueType.Double: return Double.ToString();
        default: return "<Unexpected>";
      }
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

    public JSValue GetObjectProperty(string propertyName)
    {
      return (Type == JSValueType.Object && Object.TryGetValue(propertyName, out JSValue value)) ? value : Null;
    }

    public JSValue GetArrayItem(int index)
    {
      return (Type == JSValueType.Array && index < Array.Count) ? Array[index] : Null; 
    }

    public JSValue this[string propertyName]
    {
      get { return GetObjectProperty(propertyName); }
    }

    public JSValue this[int index]
    {
      get { return GetArrayItem(index); }
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

  class JSValueObject : Dictionary<string, JSValue>
  {
    public static implicit operator JSValue(JSValueObject properties)
    {
      return new JSValue(new ReadOnlyDictionary<string, JSValue>(properties));
    }
  }

  class JSValueArray : List<JSValue>
  {
    public static implicit operator JSValue(JSValueArray items)
    {
      return new JSValue(new ReadOnlyCollection<JSValue>(items));
    }
  }
}
