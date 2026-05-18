#include "pch.h"
#include "Screenshots.h"
#include <gdiplus.h>
#include <shlwapi.h>
#include <mutex>

#pragma warning(disable : 4995)

std::wstring Utf8ToUtf16(const char *utf8, size_t utf8Len) {
  std::wstring utf16{};

  // A small optimization.
  if (utf8Len == 0) {
    return utf16;
  }

  // Extra parentheses needed here to prevent expanding max as a
  // Windows-specific preprocessor macro.
  if (utf8Len > static_cast<size_t>((std::numeric_limits<int>::max)())) {
    throw std::overflow_error("Length of input string to Utf8ToUtf16() must fit into an int.");
  }

  const int utf8Length = static_cast<int>(utf8Len);

  // We do not specify MB_ERR_INVALID_CHARS here, which means that invalid UTF-8
  // characters are replaced with U+FFFD.
  constexpr DWORD flags = 0;

  const int utf16Length = ::MultiByteToWideChar(
      CP_UTF8, // Source string is in UTF-8.
      flags, // Conversion flags.
      utf8, // Source UTF-8 string pointer.
      utf8Length, // Length of the source UTF-8 string, in chars.
      nullptr, // Do not convert during this step, instead, request the size
      0 //   of the destination buffer, in wchar_ts, excluding the
      //   null termination character.
  );

  if (utf16Length == 0) {
    winrt::throw_hresult(E_FAIL);
  }

  // Note that because the length of the input UTF-8 string was explicitly
  // passed to MultiByteToWideChar (instead of just passing -1 and asking
  // MultiByteToWideChar to scan the whole input string until a null terminator
  // is found), MultiByteToWideChar won't add an additional null terminator to
  // the result string. Therefore, there's no need to invoke
  // std::wstring::resize with a "utf16Length + 1" value.
  utf16.resize(utf16Length);

  // Convert from UTF-8 to UTF-16
  // Note that MultiByteToWideChar converts the UTF-8 BOM into the UTF-16BE BOM.
  // So we do not have to do anything extra here to ensure correct BOM behavior.
  int result = ::MultiByteToWideChar(
      CP_UTF8, // Source string is in UTF-8.
      flags, // Conversion flags.
      utf8, // Source UTF-8 string pointer.
      utf8Length, // Length of source UTF-8 string, in chars.
      &utf16[0], // Pointer to destination buffer. This is fine because the
      //   the C++11 standard specifies that the elements of a
      //   std::basic_string are stored continuously.
      utf16Length // Size of destination buffer, in wchar_ts.
  );

  if (result == 0) {
    winrt::throw_hresult(E_FAIL);
  }

  return utf16;
}

std::wstring Utf8ToUtf16(const char *utf8) {
  return Utf8ToUtf16(utf8, strlen(utf8));
}

constexpr size_t maxPathLength = MAX_PATH;
extern HWND global_hwnd;
static std::once_flag g_initGdiPlus;

class REXBITMAP {
 public:
  struct HandleCloser final {
    auto operator()(HANDLE hFile) const noexcept -> void {
      if (hFile)
        CloseHandle(hFile);
    }
  };

  // Writes bitmap to disk. Returns whether the file was completely written or not. Will overwrite existing file.
  bool WriteToDisk(const char *file) const noexcept {
    if (!m_hbm)
      return false;

    std::call_once(g_initGdiPlus, []() {
      // TODO do once
      Gdiplus::GdiplusStartupInput gdiplusStartupInput;
      ULONG_PTR gdiplusToken;
      Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    });

    Gdiplus::Bitmap bmp(m_hbm.get(), (HPALETTE)0);
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
    bmp.Save(Utf8ToUtf16(file).c_str(), &pngClsid, NULL);

    return true;
  }

  struct DCReleaser final {
    auto operator()(HDC hdc) const noexcept -> void {
      if (hdc)
        ReleaseDC(nullptr, hdc);
    }
  };

  struct HDCDeleter final {
    auto operator()(HDC handle) const noexcept -> void {
      if (handle)
        DeleteObject(handle);
    }
  };

  struct BitmapDeleter final {
    auto operator()(HBITMAP handle) const noexcept -> void {
      if (handle)
        DeleteObject(handle);
    }
  };

  int GetEncoderClsid(const WCHAR *format, CLSID *pClsid) const {
    UINT num = 0; // number of image encoders
    UINT size = 0; // size of the image encoder array in bytes

    Gdiplus::ImageCodecInfo *pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
      return -1; // Failure

    pImageCodecInfo = (Gdiplus::ImageCodecInfo *)(malloc(size));
    if (pImageCodecInfo == NULL)
      return -1; // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
      if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
        *pClsid = pImageCodecInfo[j].Clsid;
        free(pImageCodecInfo);
        return j; // Success
      }
    }

    free(pImageCodecInfo);
    return -1; // Failure
  }

  std::unique_ptr<std::remove_pointer<HBITMAP>::type, BitmapDeleter> m_hbm;

  // Generate screenshot for client portion of provided HWND.
  // Returns:
  // * Empty string on success, with output parameters filled in
  // * Error description on failure.
  std::string GenerateScreenshot(HWND hwnd, RECT *prect = nullptr) noexcept {
    RECT rcClient;

    if (prect == nullptr) {
      GetClientRect(hwnd, &rcClient);
    } else {
      CopyRect(&rcClient, prect);
      POINT pt{0, 0};
      ClientToScreen(hwnd, &pt);
      rcClient.left -= pt.x;
      rcClient.right -= pt.x;
      rcClient.top -= pt.y;
      rcClient.bottom -= pt.y;
    }

    MapWindowPoints(hwnd, NULL, reinterpret_cast<LPPOINT>(&rcClient), 2);

    // We're screenshotting the screen here. Ideally we'd screenshot the window (so it doesn't matter which window is on
    // top) but that seems to work for only the first screenshot...
    std::unique_ptr<std::remove_pointer<HDC>::type, DCReleaser> hdcWnd(GetDC(nullptr));

    std::unique_ptr<std::remove_pointer<HDC>::type, HDCDeleter> hdcMem(CreateCompatibleDC(hdcWnd.get()));
    if (hdcMem.get() == nullptr)
      return std::string(__FUNCTION__) + ": CreateCompatibleDC error";

    m_hbm = std::move(std::unique_ptr<std::remove_pointer<HBITMAP>::type, BitmapDeleter>(
        CreateCompatibleBitmap(hdcWnd.get(), rcClient.right - rcClient.left, rcClient.bottom - rcClient.top)));
    if (m_hbm.get() == nullptr)
      return std::string(__FUNCTION__) + ": CreateCompatibleBitmap error";

    SelectObject(hdcMem.get(), m_hbm.get());

    // We should do something like this, but I can't seem to get it to work as expected :-(
    // BringWindowToTop(hwnd);

    if (!BitBlt(
            hdcMem.get(),
            0,
            0,
            rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top,
            hdcWnd.get(),
            rcClient.left,
            rcClient.top,
            SRCCOPY))
      return std::string(__FUNCTION__) + ": BitBlt error";

    return "";
  }
};

std::string MakeScreenshot(const MakeScreenshotParameters &parameters) noexcept {
  char screenshotFile[maxPathLength];
  PathCombineA(screenshotFile, parameters.screenshotsPath.c_str(), parameters.testName.c_str());
  strcat_s(screenshotFile, ".png"); // PathAddExtensionA won't work if the test name has a period in it

  REXBITMAP screenshot;
  std::string result = screenshot.GenerateScreenshot(global_hwnd, parameters.pRect);
  if (result.length() != 0)
    return result;

  result = "success";

  if (!screenshot.WriteToDisk(screenshotFile))
    return std::string(__FUNCTION__) + ": error writing bitmap to disk";

  return result;
}
