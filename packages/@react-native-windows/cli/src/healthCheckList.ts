// Store list of health checks here to avoid having to launch PowerShell on every react-native config call
export const HealthCheckList = [
  [false, "FreeSpace", "Free space on current drive > 15 GB"],
  [false, "InstalledMemory", "Installed memory >= 16 GB"],
  [true, "WindowsVersion", "Windows version > 10.0.16299.0"],
  [true, "DeveloperMode", "Developer mode is on"],
  [true, "LongPath", "Long path support is enabled"],
  [true, "Choco", "Choco"],
  [true, "git", "git"],
  [true, "VSUWP", "Compilers, build tools, SDKs and Visual Studio"],
  [true, "Node", "NodeJS LTS"],
  [false, "Chrome", "Chrome"],
  [true, "Yarn", "Yarn"],
  [true, "DotNetCore", ".net core 3.1"]
]
