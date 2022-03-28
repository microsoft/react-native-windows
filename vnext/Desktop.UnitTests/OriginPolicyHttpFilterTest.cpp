// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/OriginPolicyHttpFilter.h>

// TODO: revert
#include <winrt/Windows.Web.Http.h>
#include <winrt/base.h>
#include <regex>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Microsoft::React::Networking::OriginPolicyHttpFilter;
using winrt::Windows::Foundation::Uri;

namespace Microsoft::React::Test {

TEST_CLASS (OriginPolicyHttpFilterTest) {
  // TEMP tests to see if Uri has comparison capabilities
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

  // TODO: Remove
  TEST_METHOD(Dummy) {
    Uri uri{L"http://user:password@domain.com/ab?c=d&e=f"};
    Uri iri{uri.DisplayUri()};

    winrt::hstring::size_type zero = 0;
    Assert::AreEqual(zero, iri.UserName().size());
    Assert::AreEqual(zero, iri.Password().size());
    Assert::AreEqual(uri.DisplayUri().c_str(), iri.ToString().c_str());

    auto cmp = [](std::string a, std::string b) { return _strcmpi(a.c_str(), b.c_str()) < 0; };
    std::regex rgx{"\\s*,\\s*"};
    std::string value = "NO , si, no  ,Yes,yes ,  \tnelson, Yes, nO, SI";

    std::set<std::string> conjcs{
        std::sregex_token_iterator{value.cbegin(), value.cend(), rgx, -1}, std::sregex_token_iterator{}};

    std::set<std::string, decltype(cmp)> conj{
        std::sregex_token_iterator{value.cbegin(), value.cend(), rgx, -1}, std::sregex_token_iterator{}, cmp};

    size_t sz = 4;
    Assert::AreEqual(sz, conj.size());
  }
};

} // namespace Microsoft::React::Test
