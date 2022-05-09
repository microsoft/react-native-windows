// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/OriginPolicyHttpFilter.h>

// Windows API
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Web.Http.h>

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

  TEST_METHOD(ExcludeHttpOnlyCookies) {
    std::vector<std::map<std::wstring, std::wstring>> cases{
        {
            {L"Cache-Control", L"controlled"},
            {L"Set-Cookie", L"k=v; HttpOnly  ; k2=v2"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  httponly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT; httpOnly;"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"Set-Cookie", L"HTTPONLY;id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"set-cookie", L"HttpOnly id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"set-cookie2", L"HttpOnly id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },
    };

    std::vector<std::map<std::wstring, std::wstring>> expected{
        {
            {L"Cache-Control", L"controlled"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },
    };

    for (size_t i = 0; i < cases.size(); ++i) {
      winrt::Windows::Web::Http::HttpResponseMessage response;
      for (const auto &header : cases[i]) {
        response.Headers().Insert(header.first, header.second);
      }

      winrt::Windows::Web::Http::HttpResponseMessage expectedResponse;
      auto expectedHeaders = expectedResponse.Headers();
      for (const auto &header : expected[i]) {
        expectedHeaders.Insert(header.first, header.second);
      }

      OriginPolicyHttpFilter::RemoveHttpOnlyCookiesFromResponseHeaders(response, false /*removeAll*/);

      // Set-Cookie field removed.
      Assert::AreEqual(expected[i].size(), static_cast<size_t>(response.Headers().Size()));
      bool equals = true;
      for (const auto &expectedHeader : expected[i]) {
        auto optHeader = response.Headers().TryLookup(expectedHeader.first);
        if (!optHeader.has_value() || optHeader.value() != expectedHeader.second) {
          equals = false;
          break;
        }
      }
      Assert::IsTrue(equals);
    }
  }

  TEST_METHOD(KeepNonHttpOnlyCookies) {
    std::vector<std::map<std::wstring, std::wstring>> cases{
        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT HttpOnly;"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"HttpOnly=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT ;"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookies", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  HttpOnly"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"HttpOnly id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookie2", L"HttpOnly id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT"},
            {L"k3", L"v3"},
        },
    };

    for (size_t i = 0; i < cases.size(); ++i) {
      winrt::Windows::Web::Http::HttpResponseMessage response;
      for (const auto &header : cases[i]) {
        response.Headers().Insert(header.first, header.second);
      }

      OriginPolicyHttpFilter::RemoveHttpOnlyCookiesFromResponseHeaders(response, false /*removeAll*/);

      // No fields removed.
      Assert::AreEqual(cases[i].size(), static_cast<size_t>(response.Headers().Size()));
    }
  }

  TEST_METHOD(RemoveAllCookies) {
    std::vector<std::map<std::wstring, std::wstring>> cases{
        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT HttpOnly;"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"Expires=Wed, 21 Oct 2020 07:28:00 GMT ;"},
            {L"k3", L"v3"},
        },
    };

    for (size_t i = 0; i < cases.size(); ++i) {
      winrt::Windows::Web::Http::HttpResponseMessage response;
      for (const auto &header : cases[i]) {
        response.Headers().Insert(header.first, header.second);
      }

      OriginPolicyHttpFilter::RemoveHttpOnlyCookiesFromResponseHeaders(response, true /*removeAll*/);

      Assert::AreEqual(2, static_cast<int>(response.Headers().Size()));
    }
  }
};

} // namespace Microsoft::React::Test
