var selenium = require('selenium-webdriver')

module.exports = function () {
  return new Promise(function (resolve, reject) {
    var driver = new selenium.Builder()
        .usingServer('http://localhost:9999')
        .withCapabilities({
            'app': __dirname + '\\..\\..\\ReactWindows\\Playground.Net46\\bin\\x64\\ReleaseBundle\\Playground.Net46.exe',
            'launchDelay': '1000'
        })
        .forBrowser('desktop')
        .build()

    // Normalize window and run specs
    var spawn = require('child_process').spawn
    var child = spawn("powershell.exe", [__dirname + '\\normalize_window.ps1'])
    child.on('exit', function () {
      resolve(driver)
    })
    child.stderr.on('data', function (data) {
      console.log('Failed to normalize window: ', data.toString('utf8'))
      reject()
    })
    child.stdin.end()
  })
}
