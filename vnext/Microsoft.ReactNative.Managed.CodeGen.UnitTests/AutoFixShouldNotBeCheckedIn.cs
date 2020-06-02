using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests
{
  [TestClass]
  public class AutoFixShouldNotBeCheckedIn : CodeGenTestBase
  {
    [TestMethod]
    public void EnsureAutoFixIsFalse()
    {
      Assert.IsFalse(AutoFixLkgs, "Do Not check AutoFixLkgs in as true, since it will not catch any failures otherwise.");
    }
  }
}
