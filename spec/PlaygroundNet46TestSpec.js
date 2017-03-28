var buildDriver = require('./support/buildDriver')
var diffScreenshots = require('./support/diffScreenshots')
var selenium = require('selenium-webdriver')
var By = selenium.By

jasmine.DEFAULT_TIMEOUT_INTERVAL = 15000

describe('In PlaygroundNet46 Test', () => {
    beforeAll((done) => {
        buildDriver().then((driver) => {
            this.driver = driver
            done()
        })
    })

    it('looks as expected', (done) => {
        diffScreenshots(this.driver, 'playground net46').then((diff) => {
            expect(diff).toBeLessThan(0.01)
            done()
        })
    })

    it('has WelcomeText text', (done) => {
        var button = this.driver.findElement(By.id('WelcomeText'))
        button.getAttribute('AutomationId').then((id) => {
            expect(id).toEqual('WelcomeText')
            done()
        })
    })

    afterAll((done) => {
        this.driver.quit().then(done)
    })
})
