// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/OriginPolicyHttpFilter.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Microsoft::React::Networking::OriginPolicyHttpFilter;
using winrt::Windows::Foundation::Uri;

namespace Microsoft::React::Test {

TEST_CLASS (OriginPolicyHttpFilterTest) {

//TEMP tests to see if Uri has comparison capabilities
TEST_METHOD(UrlsHaveSameOrigin) {  
  // clang-format off
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com"       }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com/"      }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com//"     }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:80"  }, Uri{L"http://www.microsoft.com:80"    }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://msw"                  }, Uri{L"https://MSW"                    }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com:80"    }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://www.microsoft.com"    }, Uri{L"https://www.microsoft.com:443"  }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"https://www.microsoft.com"      }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com:8080"  }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com:81"    }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:80"  }, Uri{L"http://www.microsoft.com:81"    }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"file://testfolder"            }, Uri{L"http://testfolder"              }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:443" }, Uri{L"https://www.microsoft.com:443"  }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"ftp://www.microsoft.com"      }, Uri{L"file://abc.com"                 }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://"                      }, Uri{L"http://www.microsoft.com"       }));
  // Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                             }, Uri{L"http://www.microsoft.com:81"    }));
  // Assert::IsFalse(Uri{L"bar://abc.com"                }.Equals(Uri{L"abc.com"}));
  // clang-format on
}

};

} //namespace
