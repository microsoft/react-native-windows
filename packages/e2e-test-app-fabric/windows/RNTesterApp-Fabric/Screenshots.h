
struct MakeScreenshotParameters {
  std::string testName;
  std::string screenshotsPath;

  RECT *pRect{nullptr};
};

std::string MakeScreenshot(const MakeScreenshotParameters &parameters) noexcept;
