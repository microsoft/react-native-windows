import { webDriver2, HomePage, TextInputTestPage, AnyPage, clickAndWaitForHomePage } from '../testlib/index';
import { waitForPage } from 'selenium-appium'

jest.setTimeout(30000);

beforeAll(() => {
  return webDriver2.start();
});

afterAll(() => {
  return webDriver2.stop();
})

describe("TestInputTestPage", () => {
  beforeAll(async () => {
    const homePage = await clickAndWaitForHomePage(webDriver2);
    await homePage.clickAndWaitForTextInputTestPage();
  })

  test('First', async () => {
    const textInputPage = await waitForPage(TextInputTestPage, webDriver2);
    const textInput = textInputPage.getTextInput();
    await textInput.clear();
    await textInput.sendKeys('ABC');
    expect(await textInput.getText()).toBe('ABC');
  });


  test('Second', async () => {
    const textInputPage = await waitForPage(TextInputTestPage, webDriver2);
    const textInput = textInputPage.getTextInput();
    await textInput.clear();
    
    await textInput.sendKeys('EF');
    expect(await textInput.getText()).toBe('EF');
  });

  afterAll(async () => {
    const homePage = await clickAndWaitForHomePage(webDriver2);
  })
})