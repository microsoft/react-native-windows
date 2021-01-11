using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests
{
  [TestClass]
  public class AutoFixShouldNotBeCheckedIn : CodeGenTestBase
  {
    [TestMethod]
    public void EnsureAutoFixIsFalse()
    {
      Assert.IsFalse(AutoFixLkgs, @"Do Not check AutoFixLkgs in as true.
The base test class has a boolean called `AutoFixLkgs` that you can set to regenerate the lkg's.
This is very usefull in case there are a lot of changes in the codegen that touch a lot of tests. 
This test fails if that flag is turned on. This makes sure that nobody accidentally creates a check-in that sets the flag to true and have all the tests pass form here on out because the LKG matches");
    }
  }
}
