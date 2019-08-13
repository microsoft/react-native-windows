// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;

namespace ReactUWPTestApp
{
  class TestInventory
  {
    static TestInventory()
    {
      Tests = new List<TestDeclaration>
            {
                new TestDeclaration("TextInputTestPage", typeof(ReactTestPage), "ReactUWPTestApp/TextInputTestPage", "TextInputTestPage"),
            };
    }

    public static List<TestDeclaration> Tests { get; private set; }
  }
}
