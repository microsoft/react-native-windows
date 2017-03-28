var fs = require('fs')
var path = require('path')
var jimp = require('jimp')

var imageDoesMatch = function (driver, expected) {
  return new Promise(function (resolve, reject) {
    var basePath = path.join(__dirname, '..', 'screenshots')
    var screenshotName = expected.replace(/\s+/g, '-').toLowerCase() + '-'
    var actualPath = path.join(basePath, screenshotName + 'actual.png')
    var expectedPath = path.join(basePath, screenshotName + 'expected.png')
    var diffPath = path.join(basePath, screenshotName + 'diff.png')

    driver.takeScreenshot().then((screenshot) => {
      jimp.read(Buffer.from(screenshot, 'base64')).then((actualImg) => {
        // crop to normalized window
        actualImg.crop(8, 31, 1152, 648)
        actualImg.write(actualPath)

        // ensure expected (use actual first time through)
        if (!fs.existsSync(expectedPath)) {
          actualImg.write(expectedPath)
        }

        jimp.read(expectedPath).then((expectedImg) => {
          jimp.read(actualPath).then((actualImg) => {
            var diff = jimp.diff(actualImg, expectedImg)
            diff.image.write(diffPath)

            if (diff.percent < 0.01) {
              resolve(diff.percent)          
            } else {
              reject(diff.percent)
            }
          })
        })
      })
    })
  })
}

var retryDiff = function (driver, expected, maxTimes) {
  return imageDoesMatch(driver, expected).catch((errorDiff) => {
    if (maxTimes === 0) {
      return errorDiff
    } else {
      return new Promise(function (resolve, reject) {
        setTimeout(() => {
          resolve(retryDiff(driver, expected, maxTimes - 1))
        }, 3000)
      })
    }
  })
}

module.exports = function (driver, expected) {
  return new Promise(function (resolve, reject) {
    retryDiff(driver, expected, 10).then((diff) => {
      resolve(diff)
    })
  })
}
