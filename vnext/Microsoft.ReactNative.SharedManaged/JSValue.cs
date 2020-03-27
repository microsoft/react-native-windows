// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;

namespace Microsoft.ReactNative.Managed
{
  // JSValueObject is based on Dictionary<string, JSValue> and can be used to initialize Object value in JSValue.
  // It is possible to write: JSValueObject{{"X", 4}, {"Y", 5}} or JSValueObject{["X"] = 4, ["Y"] = 5} and assign it to JSValue.
  class JSValueObject : Dictionary<string, JSValue>, IEquatable<JSValueObject>
  {
    // Default constructor
    public JSValueObject() { }

    // Create JSValueObject as a shallow copy of 'other'.
    public static JSValueObject CopyFrom(IReadOnlyDictionary<string, JSValue> other)
    {
      JSValueObject obj = new JSValueObject();
      foreach (var keyValue in other)
      {
        obj.Add(keyValue.Key, keyValue.Value);
      }

      return obj;
    }

    // Return true if two JSValueObject interfaces are strictly equal to each other.
    // Both objects must have the same set of equal properties.
    // Property values must be equal.
    public static bool Equals(IReadOnlyDictionary<string, JSValue> left, IReadOnlyDictionary<string, JSValue> right)
    {
      if (left == right) { return true; }
      else if (left == null) { return false; }
      else if (right == null) { return false; }
      else if (left.Count != right.Count) { return false; }

      foreach (var entry in left)
      {
        if (!right.TryGetValue(entry.Key, out var value) || !value.Equals(entry.Value))
        {
          return false;
        }
      }

      return true;
    }

    // Return true if this JSValueObject is strictly equal to 'other'.
    public bool Equals(JSValueObject other)
    {
      return Equals(this as IReadOnlyDictionary<string, JSValue>, other as IReadOnlyDictionary<string, JSValue>);
    }

    // Return true if this JSValueObject is strictly equal to 'other'.
    public bool Equals(IReadOnlyDictionary<string, JSValue> other)
    {
      return Equals(this as IReadOnlyDictionary<string, JSValue>, other);
    }

    // Return true if this JSValueObject is strictly equal to 'obj'.
    public override bool Equals(object obj)
    {
      return Equals(this as IReadOnlyDictionary<string, JSValue>, obj as IReadOnlyDictionary<string, JSValue>);
    }

    // True if Equals(left, right)
    public static bool operator ==(JSValueObject left, IReadOnlyDictionary<string, JSValue> right)
    {
      return Equals(left, right);
    }

    // True if !Equals(left, right)
    public static bool operator !=(JSValueObject left, IReadOnlyDictionary<string, JSValue> right)
    {
      return !Equals(left, right);
    }

    // True if Equals(left, right)
    public static bool operator ==(IReadOnlyDictionary<string, JSValue> left, JSValueObject right)
    {
      return Equals(left, right);
    }

    // True if !Equals(left, right)
    public static bool operator !=(IReadOnlyDictionary<string, JSValue> left, JSValueObject right)
    {
      return !Equals(left, right);
    }

    // Return true if this JSValueObject is strictly equal to other JSValueObject
    // after their property values are converted to the same type.
    // See JSValue::JSEquals for details about the conversion.
    public static bool JSEquals(IReadOnlyDictionary<string, JSValue> left, IReadOnlyDictionary<string, JSValue> right)
    {
      if (left == right) { return true; }
      else if (left == null) { return false; }
      else if (right == null) { return false; }
      else if (left.Count != right.Count) { return false; }

      foreach (var keyValue in left)
      {
        if (!right.TryGetValue(keyValue.Key, out var rightValue) || !keyValue.Value.JSEquals(rightValue))
        {
          return false;
        }
      }

      return true;
    }

    // Calculation of the JSValueObject hash code is quite expensive.
    // It is not implemented.
    public override int GetHashCode()
    {
      throw new NotImplementedException("JSValueObject currently does not support hash codes");
    }

    // Create JSValueObject from IJSValueReader.
    public static JSValueObject ReadFrom(IJSValueReader reader) => JSValue.ReadObjectFrom(reader);

    // Write this JSValueObject to IJSValueWriter.
    public void WriteTo(IJSValueWriter writer) => JSValue.WriteObject(writer, this);
  }

  // JSValueObject is based on List<JSValue> and can be used to initialize Array value in JSValue.
  // It is possible to write: JSValueArray{"X", 42, JSValue.Null, true} and assign it to JSValue.
  class JSValueArray : List<JSValue>, IEquatable<JSValueArray>
  {
    // Default constructor
    public JSValueArray() { }

    public static JSValueArray CopyFrom(IReadOnlyList<JSValue> other)
    {
      JSValueArray arr = new JSValueArray();
      foreach (var item in other)
      {
        arr.Add(item);
      }

      return arr;
    }

    // Return true if two JSValuearray interfaces are strictly equal to each other.
    // Both arrays must have the same set of items. Items must be equal.
    public static bool Equals(IReadOnlyList<JSValue> left, IReadOnlyList<JSValue> right)
    {
      if (left == right) { return true; }
      else if (left == null) { return false; }
      else if (right == null) { return false; }
      else if (left.Count != right.Count) { return false; }

      for (int i = 0; i < left.Count; ++i)
      {
        if (!left[i].Equals(right[i]))
        {
          return false;
        }
      }

      return true;
    }

    // Return true if this JSValueArray is strictly equal to 'other'.
    public bool Equals(JSValueArray other)
    {
      return Equals(this as IReadOnlyList<JSValue>, other as IReadOnlyList<JSValue>);
    }

    // Return true if this JSValueArray is strictly equal to 'other'.
    public bool Equals(IReadOnlyList<JSValue> other)
    {
      return Equals(this as IReadOnlyList<JSValue>, other);
    }

    // Return true if this JSValueArray is strictly equal to 'obj'.
    public override bool Equals(object obj)
    {
      return Equals(this as IReadOnlyList<JSValue>, obj as IReadOnlyList<JSValue>);
    }

    // True if Equals(left, right)
    public static bool operator ==(JSValueArray left, IReadOnlyList<JSValue> right)
    {
      return Equals(left, right);
    }

    // True if !Equals(left, right)
    public static bool operator !=(JSValueArray left, IReadOnlyList<JSValue> right)
    {
      return !Equals(left, right);
    }

    // True if Equals(left, right)
    public static bool operator ==(IReadOnlyList<JSValue> left, JSValueArray right)
    {
      return Equals(left, right);
    }

    // True if !Equals(left, right)
    public static bool operator !=(IReadOnlyList<JSValue> left, JSValueArray right)
    {
      return !Equals(left, right);
    }

    // Return true if this JSValueArray is strictly equal to other JSValueArray
    // after their property values are converted to the same type.
    // See JSValue::JSEquals for details about the conversion.
    public static bool JSEquals(IReadOnlyList<JSValue> left, IReadOnlyList<JSValue> right)
    {
      if (left == right) { return true; }
      else if (left == null) { return false; }
      else if (right == null) { return false; }
      else if (left.Count != right.Count) { return false; }

      for (int i = 0; i < left.Count; ++i)
      {
        if (!left[i].JSEquals(right[i]))
        {
          return false;
        }
      }

      return true;
    }

    // Calculation of the JSValueArray hash code is quite expensive.
    // It is not implemented.
    public override int GetHashCode()
    {
      throw new NotImplementedException("JSValueArray currently does not support hash codes");
    }

    // Create JSValueArray from IJSValueReader.
    public static JSValueArray ReadFrom(IJSValueReader reader) => JSValue.ReadArrayFrom(reader);

    // Write this JSValueArray to IJSValueWriter.
    public void WriteTo(IJSValueWriter writer) => JSValue.WriteArray(writer, this);
  }

  // JSValue represents an immutable JavaScript value passed as a parameter.
  // It is created to simplify working with IJSValueReader in complex situations.
  // For example, when we need to serialize a TypeScript discriminating union and
  // we need to read 'kind' property before we process other properties.
  //
  // JSValue is designed to minimize number of memory allocations:
  // - It is a struct that avoids extra memory allocations when it is stored in a container.
  // - It avoids boxing simple values by using a union type to store them.
  // The JSValue is an immutable and is safe to be used from multiple threads.
  // It does not implement GetHashCode() and must not be used as a key in a dictionary.
  struct JSValue : IEquatable<JSValue>
  {
    public static readonly JSValue Null = new JSValue();
    public static readonly JSValue EmptyObject = new JSValueObject();
    public static readonly JSValue EmptyArray = new JSValueArray();
    public static readonly JSValue EmptyString = "";

    // Used by AsBoolean() conversion from a string to a Boolean.
    private static readonly HashSet<string> StringToBoolean =
      new HashSet<string>(StringComparer.OrdinalIgnoreCase) { "true", "1", "yes", "y", "on" };

    // The Void type could be used instead of void in generic types.
    public struct Void { };

    private readonly SimpleValue m_simpleValue;
    private readonly object m_objValue;

    // Create an Object JSValue.
    public JSValue(IReadOnlyDictionary<string, JSValue> value)
    {
      Type = JSValueType.Object;
      m_simpleValue = new SimpleValue();
      m_objValue = value ?? EmptyObject.ObjectValue;
    }

    // Create an Array JSValue.
    public JSValue(IReadOnlyList<JSValue> value)
    {
      Type = JSValueType.Array;
      m_simpleValue = new SimpleValue();
      m_objValue = value ?? EmptyArray.ArrayValue;
    }

    // Create a String JSValue.
    public JSValue(string value)
    {
      Type = JSValueType.String;
      m_simpleValue = new SimpleValue();
      m_objValue = value ?? "";
    }

    // Create a Boolean JSValue.
    public JSValue(bool value)
    {
      Type = JSValueType.Boolean;
      m_simpleValue = new SimpleValue { BooleanValue = value };
      m_objValue = null;
    }

    // Create an Int64 JSValue.
    public JSValue(long value)
    {
      Type = JSValueType.Int64;
      m_simpleValue = new SimpleValue { Int64Value = value };
      m_objValue = null;
    }

    // Create a Double JSValue.
    public JSValue(double value)
    {
      Type = JSValueType.Double;
      m_simpleValue = new SimpleValue { DoubleValue = value };
      m_objValue = null;
    }

    public static implicit operator JSValue(ReadOnlyDictionary<string, JSValue> value) => new JSValue(value);
    public static implicit operator JSValue(Dictionary<string, JSValue> value) => new JSValue(value);
    public static implicit operator JSValue(JSValueObject value) => new JSValue(value);
    public static implicit operator JSValue(ReadOnlyCollection<JSValue> value) => new JSValue(value);
    public static implicit operator JSValue(List<JSValue> value) => new JSValue(value);
    public static implicit operator JSValue(JSValueArray value) => new JSValue(value);
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

    // Get JSValue type.
    public JSValueType Type { get; }

    // Return true if JSValue type is Null.
    public bool IsNull => Type == JSValueType.Null;

    // Return true and Object value if JSValue type is Object, or false and JSValue.Null otherwise.
    public bool TryGetObject(out IReadOnlyDictionary<string, JSValue> value)
    {
      value = (Type == JSValueType.Object) ? (IReadOnlyDictionary<string, JSValue>)m_objValue : null;
      return value != null;
    }

    // Return true and Array value if JSValue type is Array, or false and JSValue.Null otherwise.
    public bool TryGetArray(out IReadOnlyList<JSValue> value)
    {
      value = (Type == JSValueType.Array) ? (IReadOnlyList<JSValue>)m_objValue : null;
      return value != null;
    }

    // Return true and String value if JSValue type is String, or false and JSValue.Null otherwise.
    public bool TryGetString(out string value)
    {
      value = (Type == JSValueType.String) ? (string)m_objValue : null;
      return value != null;
    }

    // Return true and Boolean value if JSValue type is Boolean, or false and JSValue.Null otherwise.
    public bool TryGetBoolean(out bool value)
    {
      if (Type == JSValueType.Boolean)
      {
        value = m_simpleValue.BooleanValue;
        return true;
      }

      value = false;
      return false;
    }

    // Return true and Int64 value if JSValue type is Int64, or false and JSValue.Null otherwise.
    public bool TryGetInt64(out long value)
    {
      if (Type == JSValueType.Int64)
      {
        value = m_simpleValue.Int64Value;
        return true;
      }

      value = 0;
      return false;
    }

    // Return true and Double value if JSValue type is Double, or false and JSValue.Null otherwise.
    public bool TryGetDouble(out double value)
    {
      if (Type == JSValueType.Double)
      {
        value = m_simpleValue.DoubleValue;
        return true;
      }

      value = 0;
      return false;
    }

    // Return Object representation of JSValue. It is JSValue.EmptyObject if type is not Object.
    public IReadOnlyDictionary<string, JSValue> AsObject() =>
      Type == JSValueType.Object ? ObjectValue : EmptyObject.ObjectValue;

    // Return Array representation of JSValue. It is JSValue.EmptyArray if type is not Array.
    public IReadOnlyList<JSValue> AsArray() =>
      Type == JSValueType.Array ? ArrayValue : EmptyArray.ArrayValue;

    // Return a String representation of JSValue.
    // Null is "null".
    // Object and Array are empty strings "".
    // Boolean is "true" or "false".
    // Int64 is converted to string using integer representation.
    // Double uses AsJSString() conversion which uses "NaN", "Infinity", and "-Infinity" for special values.
    public string AsString()
    {
      switch (Type)
      {
        case JSValueType.Null: return JSConverter.NullString;
        case JSValueType.String: return StringValue;
        case JSValueType.Boolean: return JSConverter.ToJSString(BooleanValue);
        case JSValueType.Int64: return Int64Value.ToString();
        case JSValueType.Double: return JSConverter.ToJSString(DoubleValue);
        default: return "";
      }
    }

    // Return a Boolean representation of JSValue.
    // Object and Array are true if they are not empty.
    // String is true if it case-insensitively matches "true", "1", "yes", "y", or "on" strings.
    // Int64 or Double are false if they are zero or NAN.
    public bool AsBoolean()
    {
      switch (Type)
      {
        case JSValueType.Object: return ObjectValue.Count != 0;
        case JSValueType.Array: return ArrayValue.Count != 0;
        case JSValueType.String: return StringToBoolean.Contains(StringValue);
        case JSValueType.Boolean: return BooleanValue;
        case JSValueType.Int64: return Int64Value != 0;
        case JSValueType.Double: return !double.IsNaN(DoubleValue) && DoubleValue != 0;
        default: return false;
      }
    }

    // Return an Int8 representation of JSValue. It is the same as (Int8)AsInt64().
    public sbyte AsInt8() => (sbyte)AsInt64();

    // Return an Int16 representation of JSValue. It is the same as (Int16)AsInt64().
    public short AsInt16() => (short)AsInt64();

    // Return an Int32 representation of JSValue. It is the same as (Int32)AsInt64().
    public int AsInt32() => (int)AsInt64();

    // Return an Int64 representation of JSValue.
    // String is converted to double first before converting to Int64.
    // Boolean is converted to 0 or 1.
    // Null, Object, and Array are 0.
    public long AsInt64()
    {
      switch (Type)
      {
        case JSValueType.String: return JSConverter.ToInt64(JSConverter.ToJSNumber(StringValue));
        case JSValueType.Boolean: return BooleanValue ? 1 : 0;
        case JSValueType.Int64: return Int64Value;
        case JSValueType.Double: return JSConverter.ToInt64(DoubleValue);
        default: return 0;
      }
    }

    // Return an Uint8 representation of JSValue. It is the same as (UInt8)AsInt64().
    public byte AsUInt8() => (byte)AsInt64();

    // Return an UInt16 representation of JSValue. It is the same as (UInt16)AsInt64().
    public ushort AsUInt16() => (ushort)AsInt64();

    // Return an UInt32 representation of JSValue. It is the same as (UInt32)AsInt64().
    public uint AsUInt32() => (uint)AsInt64();

    // Return an UInt64 representation of JSValue. It is the same as (UInt64)AsInt64().
    public ulong AsUInt64() => (ulong)AsInt64();

    // Return a Single representation of JSValue. It is the same as (Single)AsDouble().
    public float AsSingle() => (float)AsDouble();

    // Return a double representation of JSValue.
    // Boolean is converted to 0.0 or 1.0.
    // Null, Object, and Array are 0.
    public double AsDouble()
    {
      switch (Type)
      {
        case JSValueType.String: return JSConverter.ToJSNumber(StringValue);
        case JSValueType.Boolean: return BooleanValue ? 1.0 : 0.0;
        case JSValueType.Int64: return Int64Value;
        case JSValueType.Double: return DoubleValue;
        default: return 0;
      }
    }

    // Cast JSValue to String using AsString() call.
    public static explicit operator string(JSValue value) => value.AsString();

    // Cast JSValue to Boolean using AsBoolean() call.
    public static explicit operator bool(JSValue value) => value.AsBoolean();

    // Cast JSValue to Int8 using AsInt8() call.
    public static explicit operator sbyte(JSValue value) => value.AsInt8();

    // Cast JSValue to Int16 using AsInt16() call.
    public static explicit operator short(JSValue value) => value.AsInt16();

    // Cast JSValue to Int32 using AsInt32() call.
    public static explicit operator int(JSValue value) => value.AsInt32();

    // Cast JSValue to Int64 using AsInt64() call.
    public static explicit operator long(JSValue value) => value.AsInt64();

    // Cast JSValue to UInt8 using AsUInt8() call.
    public static explicit operator byte(JSValue value) => value.AsUInt8();

    // Cast JSValue to UInt16 using AsUInt16() call.
    public static explicit operator ushort(JSValue value) => value.AsUInt16();

    // Cast JSValue to UInt32 using AsUInt32() call.
    public static explicit operator uint(JSValue value) => value.AsUInt32();

    // Cast JSValue to UInt64 using AsUInt64() call.
    public static explicit operator ulong(JSValue value) => value.AsUInt64();

    // Cast JSValue to Single using AsSingle() call.
    public static explicit operator float(JSValue value) => value.AsSingle();

    // Cast JSValue to Double using AsDouble() call.
    public static explicit operator double(JSValue value) => value.AsDouble();

    // Return a String representation of JSValue. It is equivalent to JavaScript String(value) result.
    public string AsJSString()
    {
      StringBuilder WriteValue(StringBuilder sb, JSValue node)
      {
        switch (node.Type)
        {
          case JSValueType.Null: return sb.Append(JSConverter.NullString);
          case JSValueType.Object: return sb.Append(JSConverter.ObjectString);
          case JSValueType.Array:
            {
              bool start = true;
              foreach (var item in node.ArrayValue)
              {
                if (start)
                {
                  start = false;
                }
                else
                {
                  sb.Append(',');
                }

                WriteValue(sb, item);
              }
              return sb;
            }
          case JSValueType.String: return sb.Append(node.StringValue);
          case JSValueType.Boolean: return sb.Append(JSConverter.ToJSString(node.BooleanValue));
          case JSValueType.Int64: return sb.Append(node.Int64Value);
          case JSValueType.Double: return sb.Append(JSConverter.ToJSString(node.DoubleValue));
          default: return sb;
        }
      }

      switch (Type)
      {
        case JSValueType.Null: return JSConverter.NullString;
        case JSValueType.Object: return JSConverter.ObjectString;
        case JSValueType.Array:
          {
            StringBuilder sb = new StringBuilder();
            WriteValue(sb, this);
            return sb.ToString();
          }
        case JSValueType.String: return StringValue;
        case JSValueType.Boolean: return JSConverter.ToJSString(BooleanValue);
        case JSValueType.Int64: return Int64Value.ToString();
        case JSValueType.Double: return JSConverter.ToJSString(DoubleValue);
        default: return "";
      }
    }

    // Return a Boolean representation of JSValue. It is equivalent to JavaScript Boolean(value) result.
    public bool AsJSBoolean()
    {
      switch (Type)
      {
        case JSValueType.Object:
        case JSValueType.Array: return true;
        case JSValueType.String: return !string.IsNullOrEmpty(StringValue);
        case JSValueType.Boolean: return BooleanValue;
        case JSValueType.Int64: return Int64Value != 0;
        case JSValueType.Double: return !double.IsNaN(DoubleValue) && DoubleValue != 0;
        default: return false;
      }
    }

    // Return a Double representation of JSValue. It is equivalent to JavaScript Number(value) result.
    public double AsJSNumber()
    {
      switch (Type)
      {
        case JSValueType.Object:
        case JSValueType.Array:
        case JSValueType.String: return JSConverter.ToJSNumber(AsJSString());
        case JSValueType.Boolean: return BooleanValue ? 1 : 0;
        case JSValueType.Int64: return Int64Value;
        case JSValueType.Double: return DoubleValue;
        default: return 0;
      }
    }

    // Convert JSValue to a readable string that can be used for logging and debugging.
    public override string ToString()
    {
      switch (Type)
      {
        case JSValueType.Null: return JSConverter.NullString;
        case JSValueType.Object:
          {
            var sb = new StringBuilder();
            sb.Append("{");
            bool start = true;
            foreach (var prop in ObjectValue)
            {
              if (start)
              {
                start = false;
              }
              else
              {
                sb.Append(", ");
              }

              sb.Append(prop.Key);
              sb.Append(": ");
              sb.Append(prop.Value.ToString());
            }

            sb.Append("}");
            return sb.ToString();
          }
        case JSValueType.Array:
          {
            var sb = new StringBuilder();
            sb.Append("[");
            bool start = true;
            foreach (var item in ArrayValue)
            {
              if (start)
              {
                start = false;
              }
              else
              {
                sb.Append(", ");
              }

              sb.Append(item.ToString());
            }

            sb.Append("]");
            return sb.ToString();
          }
        case JSValueType.String: return "\"" + StringValue + "\"";
        case JSValueType.Boolean: return JSConverter.ToJSString(BooleanValue);
        case JSValueType.Int64: return Int64Value.ToString();
        case JSValueType.Double: return JSConverter.ToJSString(DoubleValue);
        default: return "<Unexpected>";
      }
    }

    // Return value T that is created from JSValue using a ReadValue extension function.
    // Default T is constructed by using default constructor.
    public T To<T>() => new JSValueTreeReader(this).ReadValue<T>();

    // Create JSValue from a type that has WriteValue extension method defined to write to IJSValueWriter.
    public JSValue From<T>(T value)
    {
      var writer = new JSValueTreeWriter();
      writer.WriteValue(value);
      return writer.TakeValue();
    }

    // Return property count if JSValue is Object, or 0 otherwise.
    public int PropertyCount => Type == JSValueType.Object ? ObjectValue.Count : 0;

    // Try to get an object property value if JSValue type is Object and the property is found.
    // It returns false and outputs JSValue.Null otherwise.
    public bool TryGetObjectProperty(string propertyName, out JSValue value)
    {
      if (TryGetObject(out var objectValue) && objectValue.TryGetValue(propertyName, out value))
      {
        return true;
      }

      value = Null;
      return false;
    }

    // Get an object property value if JSValue type is Object and the property is found,
    // or JSValue.Null otherwise.
    public JSValue GetObjectProperty(string propertyName)
    {
      TryGetObjectProperty(propertyName, out JSValue result);
      return result;
    }

    // Get an object property value if JSValue type is Object and the property is found,
    // or JSValue.Null otherwise.
    public JSValue this[string propertyName] => GetObjectProperty(propertyName);

    // Return item count if JSValue is Array, or 0 otherwise.
    public int ItemCount => Type == JSValueType.Array ? ArrayValue.Count : 0;

    // Try to get an array item if JSValue type is Array and the index is in bounds.
    // It returns false and outputs JSValue.Null otherwise.
    public bool TryGetArrayItem(int index, out JSValue value)
    {
      if (index >= 0 && TryGetArray(out var arrayValue) && index < arrayValue.Count)
      {
        value = arrayValue[index];
        return true;
      }

      value = Null;
      return false;
    }

    // Get an array item if JSValue type is Array and the index is in bounds,
    // or JSValue.Null otherwise.
    public JSValue GetArrayItem(int index)
    {
      TryGetArrayItem(index, out JSValue result);
      return result;
    }

    // Get an array item if JSValue type is Array and the index is in bounds,
    // or JSValue.Null otherwise.
    public JSValue this[int index] => GetArrayItem(index);

    // Return true if this JSValue is strictly equal to JSValue.
    // Compared values must have the same type and value.
    //
    // The behavior is similar to JavaScript === operator except for Object and Array where
    // this functions does a deep structured comparison instead of pointer equality.
    public bool Equals(JSValue other)
    {
      if (Type != other.Type)
      {
        return false;
      }

      switch (Type)
      {
        case JSValueType.Null: return true;
        case JSValueType.Object: return JSValueObject.Equals(ObjectValue, other.ObjectValue);
        case JSValueType.Array: return JSValueArray.Equals(ArrayValue, other.ArrayValue);
        case JSValueType.String: return StringValue == other.StringValue;
        case JSValueType.Boolean: return BooleanValue == other.BooleanValue;
        case JSValueType.Int64: return Int64Value == other.Int64Value;
        case JSValueType.Double: return DoubleValue == other.DoubleValue;
        default: return false;
      }
    }

    // Return true if obj is a JSValue it is strictly equal to this JSValue. 
    public override bool Equals(object obj)
    {
      return (obj is JSValue) && Equals((JSValue)obj);
    }

    // True if left.Equals(right)
    public static bool operator ==(JSValue left, JSValue right)
    {
      return left.Equals(right);
    }

    //! True if !left.Equals(right)
    public static bool operator !=(JSValue left, JSValue right)
    {
      return !left.Equals(right);
    }

    // Return true if this JSValue is strictly equal to JSValue after they are converted to the same type.
    //
    // Null is not converted to any other type before comparison.
    // Object and Array types are converted first to a String type using AsString() before comparing
    // with other types, and then we apply the same rules as for the String type.
    // String is converted to Double before comparing with Boolean, Int64, or Double.
    // Boolean is converted to 1.0 and +0.0 when comparing with String, Int64, or Double.
    // Int64 is converted to Double before comparing with other types.
    //
    // The behavior is similar to JavaScript == operator except for Object and Array for which
    // this functions does a deep structured comparison using JSEquals instead
    // of pointer equality.
    public bool JSEquals(JSValue other)
    {
      if (Type == other.Type)
      {
        switch (Type)
        {
          case JSValueType.Object:
            return JSValueObject.JSEquals(ObjectValue, other.ObjectValue);
          case JSValueType.Array:
            return JSValueArray.JSEquals(ArrayValue, other.ArrayValue);
          default:
            return Equals(other);
        }
      }
      else if (Type == JSValueType.Null || other.Type == JSValueType.Null)
      {
        return false;
      }

      // If one of the types Boolean, Int64, or Double, then compare as Numbers,
      // otherwise compare as strings.
      JSValueType greatestType = Type > other.Type ? Type : other.Type;
      if (greatestType >= JSValueType.Boolean)
      {
        return AsJSNumber() == other.AsJSNumber();
      }
      else
      {
        return AsJSString() == other.AsJSString();
      }
    }

    // Calculation of the JSValue hash code is quite expensive.
    // It is not implemented.
    public override int GetHashCode()
    {
      throw new NotImplementedException("JSValue currently does not support hash codes");
    }

    // Create JSValue from IJSValueReader.
    public static JSValue ReadFrom(IJSValueReader reader)
    {
      if (reader is IJSValueTreeReader treeReader)
      {
        return treeReader.Current;
      }

      return InternalReadFrom(reader);
    }

    // Create JSValueObject from IJSValueReader.
    public static JSValueObject ReadObjectFrom(IJSValueReader reader)
    {
      if (reader.ValueType == JSValueType.Object)
      {
        if (reader is IJSValueTreeReader treeReader)
        {
          return JSValueObject.CopyFrom(treeReader.Current.ObjectValue);
        }

        return InternalReadObjectFrom(reader);
      }

      return new JSValueObject();
    }

    // Create JSValueArray from IJSValueReader.
    public static JSValueArray ReadArrayFrom(IJSValueReader reader)
    {
      if (reader.ValueType == JSValueType.Array)
      {
        if (reader is IJSValueTreeReader treeReader)
        {
          return JSValueArray.CopyFrom(treeReader.Current.ArrayValue);
        }

        return InternalReadArrayFrom(reader);
      }

      return new JSValueArray();
    }

    // Write this JSValue to IJSValueWriter.
    public void WriteTo(IJSValueWriter writer)
    {
      switch (Type)
      {
        case JSValueType.Null: writer.WriteNull(); break;
        case JSValueType.Object: WriteObject(writer, ObjectValue); break;
        case JSValueType.Array: WriteArray(writer, ArrayValue); break;
        case JSValueType.String: writer.WriteString(StringValue); break;
        case JSValueType.Boolean: writer.WriteBoolean(BooleanValue); break;
        case JSValueType.Int64: writer.WriteInt64(Int64Value); break;
        case JSValueType.Double: writer.WriteDouble(DoubleValue); break;
      }
    }

    // Write JSValueObject key-value pairs to IJSValueWriter.
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

    // Write JSValueArray items to IJSValueWriter.
    public static void WriteArray(IJSValueWriter writer, IEnumerable<JSValue> value)
    {
      writer.WriteArrayBegin();
      foreach (var item in value)
      {
        item.WriteTo(writer);
      }

      writer.WriteArrayEnd();
    }

    private static JSValue InternalReadFrom(IJSValueReader reader)
    {
      switch (reader.ValueType)
      {
        case JSValueType.Null: return Null;
        case JSValueType.Object: return new JSValue(InternalReadObjectFrom(reader));
        case JSValueType.Array: return new JSValue(InternalReadArrayFrom(reader));
        case JSValueType.String: return new JSValue(reader.GetString());
        case JSValueType.Boolean: return new JSValue(reader.GetBoolean());
        case JSValueType.Int64: return new JSValue(reader.GetInt64());
        case JSValueType.Double: return new JSValue(reader.GetDouble());
        default: throw new ReactException("Unexpected JSValueType");
      }
    }

    private static JSValueObject InternalReadObjectFrom(IJSValueReader reader)
    {
      var jsObject = new JSValueObject();
      while (reader.GetNextObjectProperty(out string propertyName))
      {
        jsObject.Add(propertyName, InternalReadFrom(reader));
      }

      return jsObject;
    }

    private static JSValueArray InternalReadArrayFrom(IJSValueReader reader)
    {
      var jsArray = new JSValueArray();
      while (reader.GetNextArrayItem())
      {
        jsArray.Add(InternalReadFrom(reader));
      }

      return jsArray;
    }

    #region Obsolete members

    [Obsolete("Use TryGetObject or AsObject")] public IReadOnlyDictionary<string, JSValue> Object => (IReadOnlyDictionary<string, JSValue>)m_objValue;
    [Obsolete("Use TryGetArray or AsArray")] public IReadOnlyList<JSValue> Array => (IReadOnlyList<JSValue>)m_objValue;
    [Obsolete("Use TryGetString, AsString, or AsJSString")] public string String => (string)m_objValue;
    [Obsolete("Use TryGetBoolean, AsBoolean, or AsJSBoolean")] public bool Boolean => m_simpleValue.BooleanValue;
    [Obsolete("Use TryGetInt64 or AsInt64")] public long Int64 => m_simpleValue.Int64Value;
    [Obsolete("Use TryGetDouble, AsDouble, or AsJSNumber")] public double Double => m_simpleValue.DoubleValue;

    [Obsolete("Use ReadObjectFrom")]
    public static Dictionary<string, JSValue> ReadObjectPropertiesFrom(IJSValueReader reader)
    {
      return ReadObjectFrom(reader);
    }

    [Obsolete("Use ReadArrayFrom")]
    public static List<JSValue> ReadArrayItemsFrom(IJSValueReader reader)
    {
      return ReadArrayFrom(reader);
    }

    #endregion

    private IReadOnlyDictionary<string, JSValue> ObjectValue => (IReadOnlyDictionary<string, JSValue>)m_objValue;
    private IReadOnlyList<JSValue> ArrayValue => (IReadOnlyList<JSValue>)m_objValue;
    private string StringValue => (string)m_objValue;
    private bool BooleanValue => m_simpleValue.BooleanValue;
    private long Int64Value => m_simpleValue.Int64Value;
    private double DoubleValue => m_simpleValue.DoubleValue;

    // This is a 'union' type that may store in the same location bool, long, or double.
    [StructLayout(LayoutKind.Explicit)]
    private struct SimpleValue
    {
      [FieldOffset(0)] public bool BooleanValue;
      [FieldOffset(0)] public long Int64Value;
      [FieldOffset(0)] public double DoubleValue;
    }

    private static class JSConverter
    {
      public static readonly string NullString = "null";
      public static readonly string ObjectString = "[object Object]";

      public static string ToJSString(bool value)
      {
        return value ? "true" : "false";
      }

      public static string ToJSString(double value)
      {
        if (double.IsNaN(value))
        {
          return "NaN";
        }
        else if (double.IsPositiveInfinity(value))
        {
          return "Infinity";
        }
        else if (double.IsNegativeInfinity(value))
        {
          return "-Infinity";
        }
        else
        {
          return value.ToString();
        }
      }

      public static double ToJSNumber(string value)
      {
        var trimmed = value.Trim();
        if (trimmed.Length == 0)
        {
          return 0;
        }

        // NumberStyles.Float does not allow numbers to have thousand's commas insider. 
        return double.TryParse(trimmed, NumberStyles.Float, CultureInfo.InvariantCulture, out double doubleValue)
          ? doubleValue : double.NaN;
      }

      public static long ToInt64(double value) =>
        (long.MinValue <= value && value <= long.MaxValue) ? unchecked((long)value) : 0;
    }
  }
}

