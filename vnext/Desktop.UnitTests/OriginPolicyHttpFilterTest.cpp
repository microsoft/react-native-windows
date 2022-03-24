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

  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"                }, Uri{L"HTTP://WWW.MICROSOFT.COM"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://www.microsoft.com:801"           }, Uri{L"https://www.microsoft.com:801"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"                }, Uri{L"http://www.microsoft.com/app/test.html"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:801"            }, Uri{L"http://www.microsoft.com:801/app/test.html"}));
  //Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                                        }, Uri{L"file://sharedfolder"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://msw"                              }, Uri{L"http://msw/app/test.html"}));
  //Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                                        }, Uri{L"xyz"}));
  //Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                                        }, Uri{L"//bad"}));
  //Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                                        }, Uri{L"x.&y.z"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:801"            }, Uri{L"http://www.microsoft.com:801\\test.html"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"                }, Uri{L"http://www.microsoft.com/%35test.html"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://bing.com"                        }, Uri{L"https://foo:bar@bing.com/FakeResponse.ashx"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://[2001:db8:1f70::999:de8:7648:6e8]"}, Uri{L"http://[2001:db8:1f70::999:de8:7648:6e8]/test.html"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://\u65E5\u672C\u8A9E.com"          }, Uri{L"https://\u65E5\u672C\u8A9E.com/FakeResponse.ashx"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://www.microsoft.com"               }, Uri{L"https://www.microsoft.com:443"}));
  // clang-format on
}

};

} //namespace
