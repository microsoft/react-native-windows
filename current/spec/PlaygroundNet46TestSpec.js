var selenium = require('selenium-webdriver');
var path = require('path');
var By = selenium.By;

jasmine.DEFAULT_TIMEOUT_INTERVAL = 15000;

describe('In PlaygroundNet46 Test', () => {
    beforeAll((done) => {
        this.driver = new selenium.Builder()
          .usingServer('http://localhost:9999')
          .withCapabilities({
              'app': path.join(__dirname, '\\..\\ReactWindows\\Playground.Net46\\bin\\', process.env["PLATFORM"] || "x86",
                process.env["CONFIGURATION"] || "ReleaseBundle", "Playground.Net46.exe"),
              'launchDelay': '1000'
          })
          .forBrowser('desktop')
          .build();
        done();
    });

    it('has WelcomeText text', (done) => {
        var button = this.driver.findElement(By.id('WelcomeText'));
        button.getAttribute('AutomationId').then((id) => {
            expect(id).toEqual('WelcomeText');
            done();
        });
    });

    afterAll((done) => {
        this.driver.quit().then(done);
    });
});
