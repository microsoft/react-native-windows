// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactUWPTestApp
{
  public class TestDeclaration
  {
    public TestDeclaration()
    {
    }

    public TestDeclaration(string automationId, Type pageType, string jsFileName = "ReactUWPTestApp/index", string jsComponentName = "ReactUWPTestApp")
    {
      AutomationId = automationId;
      PageType = pageType;
      JSFileName = jsFileName;
      JSComponentName = jsComponentName;
    }

    public string AutomationId { get; set; }

    public Type PageType { get; set; }

    public string JSFileName { get; set; }

    public string JSComponentName { get; set; }

    public override string ToString()
    {
      return String.Format("{0}({1}:{2})", AutomationId, JSComponentName, JSFileName);
    }
  }
}
