// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace Microsoft.ReactNative.Managed
{
  [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
  public class ViewManagerExportedViewConstantAttribute : Attribute
  {
    public string ConstantName { get; set; }

    public ViewManagerExportedViewConstantAttribute() : base() { }

    public ViewManagerExportedViewConstantAttribute(string constantName)
    {
      ConstantName = constantName ?? throw new ArgumentNullException(nameof(constantName));
    }
  }

  [AttributeUsage(AttributeTargets.Method)]
  public class ViewManagerPropertyAttribute : Attribute
  {
    public string PropertyName { get; set; }

    public ViewManagerPropertyType? PropertyType { get; set; } = null;

    public ViewManagerPropertyAttribute() : base() { }

    public ViewManagerPropertyAttribute(string propertyName)
    {
      PropertyName = propertyName ?? throw new ArgumentNullException(nameof(propertyName));
    }

    public ViewManagerPropertyAttribute(string propertyName, ViewManagerPropertyType type) : this(propertyName)
    {
      PropertyType = type;
    }
  }

  [AttributeUsage(AttributeTargets.Method)]
  public class ViewManagerCommandAttribute : Attribute
  {
    public string CommandName { get; set; }

    public ViewManagerCommandAttribute() : base() { }

    public ViewManagerCommandAttribute(string commandName)
    {
      CommandName = commandName ?? throw new ArgumentNullException(nameof(commandName));
    }
  }

  public class ViewManagerExportedDirectEventTypeConstantAttribute : Attribute
  {
    public string EventName { get; private set; }

    public string CallbackName { get; private set; }

    public ViewManagerExportedDirectEventTypeConstantAttribute() : base() { }

    public ViewManagerExportedDirectEventTypeConstantAttribute(string eventName, string callbackName)
    {
      EventName = eventName ?? throw new ArgumentNullException(nameof(eventName));
      CallbackName = callbackName ?? throw new ArgumentNullException(nameof(callbackName));
    }
  }

  [AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
  public class ViewManagerExportedBubblingEventTypeConstantAttribute : Attribute
  {
    public string EventName { get; private set; }

    public string BubbleCallbackName { get; private set; }

    public string CaptureCallbackName { get; private set; }

    public ViewManagerExportedBubblingEventTypeConstantAttribute() : base() { }

    public ViewManagerExportedBubblingEventTypeConstantAttribute(string eventName, string bubbleCallbackName)
    {
      EventName = eventName ?? throw new ArgumentNullException(nameof(eventName));
      BubbleCallbackName = bubbleCallbackName ?? throw new ArgumentNullException(nameof(bubbleCallbackName));
    }

    public ViewManagerExportedBubblingEventTypeConstantAttribute(string eventName, string bubbleCallbackName, string captureCallbackName) : this(eventName, bubbleCallbackName)
    {
      CaptureCallbackName = captureCallbackName ?? throw new ArgumentNullException(nameof(captureCallbackName));
    }
  }
}
