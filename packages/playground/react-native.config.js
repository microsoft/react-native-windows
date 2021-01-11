// Change the below to true for autolink to target playground-win32 instead of playground.
// Then run `npx react-native autolink-windows` to actually run the autolink.
const targetWin32 = false;

module.exports = {
  project: {
    windows: {
      sourceDir: 'windows',
      solutionFile: targetWin32 ? 'playground-win32.sln' : 'playground.sln',
      project: {
        projectFile: targetWin32 ? 'playground-win32\\playground-win32.vcxproj' : 'playground\\playground.vcxproj',
      },
    },
  },
};
