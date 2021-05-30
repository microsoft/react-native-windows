// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// OneOf is a class that is similar to C++ std::variant.
// Due to the Winmdexp.exe we cannot name all cases of this class OneOf<...>.
// It gives this error: error WME1065: Type names cannot differ only by case.

namespace Microsoft.ReactNative.Managed
{
  public struct OneOf1<T1>
  {
    public OneOf1(T1 value)
    {
      Value = value;
    }

    public static implicit operator OneOf1<T1>(T1 value) => new OneOf1<T1>(value);

    public object Value { get; }

    public bool TryGet<T>(out T value)
    {
      bool result = (Value != null && typeof(T) == Value.GetType());
      value = result ? (T)Value : default(T);
      return result;
    }
  }

  public struct OneOf2<T1, T2>
  {
    public OneOf2(T1 value)
    {
      Value = value;
    }

    public OneOf2(T2 value)
    {
      Value = value;
    }

    public static implicit operator OneOf2<T1, T2>(T1 value) => new OneOf2<T1, T2>(value);
    public static implicit operator OneOf2<T1, T2>(T2 value) => new OneOf2<T1, T2>(value);

    public object Value { get; }

    public bool TryGet<T>(out T value)
    {
      bool result = (Value != null && typeof(T) == Value.GetType());
      value = result ? (T)Value : default(T);
      return result;
    }
  }

  public struct OneOf3<T1, T2, T3>
  {
    public OneOf3(T1 value)
    {
      Value = value;
    }

    public OneOf3(T2 value)
    {
      Value = value;
    }

    public OneOf3(T3 value)
    {
      Value = value;
    }

    public static implicit operator OneOf3<T1, T2, T3>(T1 value) => new OneOf3<T1, T2, T3>(value);
    public static implicit operator OneOf3<T1, T2, T3>(T2 value) => new OneOf3<T1, T2, T3>(value);
    public static implicit operator OneOf3<T1, T2, T3>(T3 value) => new OneOf3<T1, T2, T3>(value);

    public object Value { get; }

    public bool TryGet<T>(out T value)
    {
      bool result = (Value != null && typeof(T) == Value.GetType());
      value = result ? (T)Value : default(T);
      return result;
    }
  }

  public struct OneOf4<T1, T2, T3, T4>
  {
    public OneOf4(T1 value)
    {
      Value = value;
    }

    public OneOf4(T2 value)
    {
      Value = value;
    }

    public OneOf4(T3 value)
    {
      Value = value;
    }

    public OneOf4(T4 value)
    {
      Value = value;
    }

    public static implicit operator OneOf4<T1, T2, T3, T4>(T1 value) => new OneOf4<T1, T2, T3, T4>(value);
    public static implicit operator OneOf4<T1, T2, T3, T4>(T2 value) => new OneOf4<T1, T2, T3, T4>(value);
    public static implicit operator OneOf4<T1, T2, T3, T4>(T3 value) => new OneOf4<T1, T2, T3, T4>(value);
    public static implicit operator OneOf4<T1, T2, T3, T4>(T4 value) => new OneOf4<T1, T2, T3, T4>(value);

    public object Value { get; }

    public bool TryGet<T>(out T value)
    {
      bool result = (Value != null && typeof(T) == Value.GetType());
      value = result ? (T)Value : default(T);
      return result;
    }
  }

  public struct OneOf5<T1, T2, T3, T4, T5>
  {
    public OneOf5(T1 value)
    {
      Value = value;
    }

    public OneOf5(T2 value)
    {
      Value = value;
    }

    public OneOf5(T3 value)
    {
      Value = value;
    }

    public OneOf5(T4 value)
    {
      Value = value;
    }

    public OneOf5(T5 value)
    {
      Value = value;
    }

    public static implicit operator OneOf5<T1, T2, T3, T4, T5>(T1 value) => new OneOf5<T1, T2, T3, T4, T5>(value);
    public static implicit operator OneOf5<T1, T2, T3, T4, T5>(T2 value) => new OneOf5<T1, T2, T3, T4, T5>(value);
    public static implicit operator OneOf5<T1, T2, T3, T4, T5>(T3 value) => new OneOf5<T1, T2, T3, T4, T5>(value);
    public static implicit operator OneOf5<T1, T2, T3, T4, T5>(T4 value) => new OneOf5<T1, T2, T3, T4, T5>(value);
    public static implicit operator OneOf5<T1, T2, T3, T4, T5>(T5 value) => new OneOf5<T1, T2, T3, T4, T5>(value);

    public object Value { get; }

    public bool TryGet<T>(out T value)
    {
      bool result = (Value != null && typeof(T) == Value.GetType());
      value = result ? (T)Value : default(T);
      return result;
    }
  }

  public struct OneOf6<T1, T2, T3, T4, T5, T6>
  {
    public OneOf6(T1 value)
    {
      Value = value;
    }

    public OneOf6(T2 value)
    {
      Value = value;
    }

    public OneOf6(T3 value)
    {
      Value = value;
    }

    public OneOf6(T4 value)
    {
      Value = value;
    }

    public OneOf6(T5 value)
    {
      Value = value;
    }

    public OneOf6(T6 value)
    {
      Value = value;
    }

    public static implicit operator OneOf6<T1, T2, T3, T4, T5, T6>(T1 value) => new OneOf6<T1, T2, T3, T4, T5, T6>(value);
    public static implicit operator OneOf6<T1, T2, T3, T4, T5, T6>(T2 value) => new OneOf6<T1, T2, T3, T4, T5, T6>(value);
    public static implicit operator OneOf6<T1, T2, T3, T4, T5, T6>(T3 value) => new OneOf6<T1, T2, T3, T4, T5, T6>(value);
    public static implicit operator OneOf6<T1, T2, T3, T4, T5, T6>(T4 value) => new OneOf6<T1, T2, T3, T4, T5, T6>(value);
    public static implicit operator OneOf6<T1, T2, T3, T4, T5, T6>(T5 value) => new OneOf6<T1, T2, T3, T4, T5, T6>(value);
    public static implicit operator OneOf6<T1, T2, T3, T4, T5, T6>(T6 value) => new OneOf6<T1, T2, T3, T4, T5, T6>(value);

    public object Value { get; }

    public bool TryGet<T>(out T value)
    {
      bool result = (Value != null && typeof(T) == Value.GetType());
      value = result ? (T)Value : default(T);
      return result;
    }
  }

  public struct OneOf7<T1, T2, T3, T4, T5, T6, T7>
  {
    public OneOf7(T1 value)
    {
      Value = value;
    }

    public OneOf7(T2 value)
    {
      Value = value;
    }

    public OneOf7(T3 value)
    {
      Value = value;
    }

    public OneOf7(T4 value)
    {
      Value = value;
    }

    public OneOf7(T5 value)
    {
      Value = value;
    }

    public OneOf7(T6 value)
    {
      Value = value;
    }

    public OneOf7(T7 value)
    {
      Value = value;
    }

    public static implicit operator OneOf7<T1, T2, T3, T4, T5, T6, T7>(T1 value) => new OneOf7<T1, T2, T3, T4, T5, T6, T7>(value);
    public static implicit operator OneOf7<T1, T2, T3, T4, T5, T6, T7>(T2 value) => new OneOf7<T1, T2, T3, T4, T5, T6, T7>(value);
    public static implicit operator OneOf7<T1, T2, T3, T4, T5, T6, T7>(T3 value) => new OneOf7<T1, T2, T3, T4, T5, T6, T7>(value);
    public static implicit operator OneOf7<T1, T2, T3, T4, T5, T6, T7>(T4 value) => new OneOf7<T1, T2, T3, T4, T5, T6, T7>(value);
    public static implicit operator OneOf7<T1, T2, T3, T4, T5, T6, T7>(T5 value) => new OneOf7<T1, T2, T3, T4, T5, T6, T7>(value);
    public static implicit operator OneOf7<T1, T2, T3, T4, T5, T6, T7>(T6 value) => new OneOf7<T1, T2, T3, T4, T5, T6, T7>(value);
    public static implicit operator OneOf7<T1, T2, T3, T4, T5, T6, T7>(T7 value) => new OneOf7<T1, T2, T3, T4, T5, T6, T7>(value);

    public object Value { get; }

    public bool TryGet<T>(out T value)
    {
      bool result = (Value != null && typeof(T) == Value.GetType());
      value = result ? (T)Value : default(T);
      return result;
    }
  }
}
