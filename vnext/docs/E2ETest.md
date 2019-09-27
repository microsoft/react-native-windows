# Author and Run E2E Test for React Native Windows

## E2E project structure

E2E test app, test library and test cases are in packages/E2ETest/, and they are organized as below.

 ![](data:image/*;base64,iVBORw0KGgoAAAANSUhEUgAAAIIAAACMCAYAAACwCXGNAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAneSURBVHhe7Z0/bGNZFcZPmJHYBQmQdqQFUYCQQxGFhoLCEaIdJ1qUKpTp7DJupkuDlG6apKBIummmSIEiocTaBSEQisWWC5aFEjML3bDLCg1QoAXFnHPufc6z8+65N4nt+NnfT7nv3D+285L3vXOfn7/cLPUZAgvPF3wECw6EABQIASgQAlAgBKBACECBEIACIQAFQgAKhAAUCAEoEAJQgh86/esPPyYdcl8uZg/N1SU8euvb9NXv/UzboJwEhfDPj96jr3zzJ1yT4St3xCWqCobjZ3/9Bb3zgzOug7ISnhpUHnERuHEOoNQEheCObYoIrqCDOcDOCAkicH3SBmUmnBFSRcB16KD8RN4+xkUAFcwHkakhRQRZDNNqLNHSki+NVtZLjaU1Omgd0JofWzvoJYyBSRCZGrhERNC/+q9W+//7N7eLqR329fX6/TOqH+3R9TFtU3OP6IWMXe4TNbcTx8C4saeGWCbg9pvXH9Gjx2/Rmz/9lNsBWg2fEdbpyHc5qrT/YocqUq3s0G69Td1LHWCsMTBujIygWy5hEUj/5//5B33tG9+nz9/8kfsK6HF6Xyc6kzO7f0n7Vd9/gx5ddHz1BtYYGAfGNUJcBBIl5T96/EWNhVx2qV1doWWp907puK29njYdn/p8r2N12qy5pj0Gxk04I+jGFoFG6c5iEbVntE9NWpapYbtLq0MZoUqr3W03bSw3afXskK6PtTUGxo2REbKNIQKO+rBBLKJCO+cuc/TPD+nw/Jx2dOJ3rDw7d2NcDkeOtDUGxoudESIicOMcBhGUFfMa4dOP36dP//IBx1/SJ69+RZ98/Gv62yspv6HXr35Lr//8Oz7+fY1Lj7/snwjKCP4IFijhjAAWCggBKBACUCAEoMC8ChSYV4Fi3EfINrYIcDNpPggKwR3bFBHAvDoP2BkhQQSuT9qpePeRfLAoH1FndfCghDNCqgi4fisd5Kns0Hl/+EOoYXKiARMl8vYxLoK7qwDMEpGpIUUEWTTQKSAzoV74TiF/xvfoYM09RkqjJWNibWtTc5n7BqZXMAkiUwOXiAji5lU+oGoscb6CF3Q84lv0tJ5Tc/VMHyPlsFajQzG7infxkvtgSJgo9tQQywTcjppXexfUqe7TM38cKzu7fHALWF6h6tE6bOsPhJERdMslLALpj5pXU9ELR8kYzp6GmWC6GNcIcRFIlDRumlcr36XVdpOe+wPbO9grnho8lZ1zutyvUucCmWGahDOCbmwRaJTuLBbCc/1ZnY7W3UXgNm0VTw2Dv31YouXmKu3qe8oabdZxsTgNgp81fPbhOr3zradcM0TAUSxrX//Oj1z84fvcB8qInREiInDjHAYRlJVgRvj772tOAwJXsro+3H352CdO3Gpefbf6c30MKB8wrwLFvo8AFgYIASgQAlAgBKDAvAoUmFeBEp4aVB5xEeBm0nwQFII7tikigHl1HrAzQoIIXJ+0J82s+RdT92fW9ruYcEZIFQHXp6IDMFEibx/jIoAK5oPI1JAigiyG8Knx4NpvMGRHyxlbpQzZDoKmV2boeQ3+LiH898+t4irfo3ew5p87jv0ZTf/GdFC436PGXe2cKpGpgUtEBCkrr+oqqt1Nfc3+WZ3azef+F8C/sJyxVVZY7axnv8DhsWHTq4wd05aYWvU1ifYKf+sZuVVcvUlmm19xfPuTSmC/bxh3/cOniD01xDIBt5NWXhUncuZerW1SnTqkTjQxtnJrsH5ifoVVy/Sqz/POJTmL1o+obS7LmlvFVb9/lbY2/F/VjGN/Ugnt9wwYd42MoFsuYRFI/9jMq7em7ldz9aU0dveC/Z4B465xjRAXgUT5YUzzqoUYWznBnmQ/OM+fe0f+jLRMr/559nRwB+66P7RMK9XcWtGtk9xYjsh+P6RxN5wRdGOLQKN0Z/HW1OhQ52GfKnX+zFZYtUyv7nmcY93zpIzlNLrr/lRoZ/d6bOlEzvsiAvtdaNydLjCvAsXOCBERuHEOg/iQ8BV5dpYNivW2EuSBeRUoMK8Cxb6PABYGCAEoEAJQIASgwLwKFJhXgRKeGlQecRHMxs0kcF+CQnDHNkUEMK/OA3ZGSBCB65M2KDPhjJAqAq7Plg7kM4eATQwEibx9jIsA2WA+iEwNKSLIYgh3hjYaYhb1nwYWGjj9mZwzmaaZSvOvL2Ojq7U+vDG0DESmBi4REaSaVzsrYhYVkwcfuKDxNGcyVQPHdqFxdNhUKmSvL+Mjq7XOgDG0DNhTQywTcDvVvDowi5rG05zJdNQ4yoe30FSq5F5/FKzomoSREXTLJSwC6b+beTXFeNqji46v3ocZMIaWAeMaIS4CiXIgb2VeNQ2cbTo+9f29Uzpu54yj/JxCU2kiWNHVJpwRdGOLQKN0ZzEJy3hapdWuO3OX9JogZxwNmkpHGVmtdQaMoWVghsyrcvW/RyuX1n90AZPCzggREbhxDoMIysoMmVeRER4SmFeBYt9HAAsDhAAUCAEoEAJQYF4FCsyrQAlPDSqPuAhwM2k+CArBHdsUEcC8Og/YGSFBBK5P2uPEuY70A0p1JsGDOGnCGSFVBFwfuw7yqJ8At50nTeTtY1wEk1UBmBaRqSFFBFksptW4aTQdpHlJ+9lgzpxqrmoaNLGC+xCZGrhERBAzr9Y263SUWYtaJ9Sp0sCF1Ds9JlpZloHEVU2HH3fTxAruij01xDIBt6PmVTGPdi5IjlXrpENbu1uiBG73SHSgptPUVU2jJlZwV4yMoFsuYRFIf9S8WtmgLT7DT3stOuls0UZN2l26FE8i10LmYzBdjGuEuAgkSoq2zasV2pAksL1Hna0Nbkm7QyfPu5IOvHXdWtU0xxhMrKCYcEbQjS0CjdKdxQAVUULbTwO+3TniaWKQDqxVTfPcxsQKbsMMmVfBQ2JnhIgI3DiHQQRlZYbMq+AhgXkVKPZ9BLAwQAhAgRCAAiEABeZVoMC8CpTw1KDyiIsAN5Pmg6AQ3LFNEQHMq/OAnRESROD6pH0P7mxQHXEvgTsTzgipIuD6fXUAg+rDE3n7GBfB/VUAZoHI1JAigiwWk2Zezff7+sH1IlihFViHTa5MwNiatg+LvUJrZGrgEhHBeMyro7Sp2d3UfbD+rf+wyXV4LG9sTdqHBV+h1Z4aYpmA22Mxr96gSvuZk3X03/qHTK6WsTVlHxZ8hVYjI+iWS1gE0l8K82rKPiz4Cq3GNUJcBBIljd7bvJqKZXI1ja3p+7CoK7SGM4JubBFolO4sBoibV1OxTK62sTW6Dwu+QivMq0CxM0JEBG6cwyCCsgLzKlBgXgWKfR8BLAwQAlAgBKBACEAJXiy+fPmSruTdwpX4Dfp0dSXvHK4GfRK/9Pbb9PTpU3ry5Il/FignRP8HsqNi3mzyPcsAAAAASUVORK5CYII=)

 - app – the RN app folder

 - reports – save the test reports

 - wdio – includes the page object libraries and test cases.

 - windows – the UWP native app

 - wdio.config.js – default parameters used by wdio test runner



# Run E2E test

- Make sure you have installed [dependencies](./GettingStarted.md#dependencies)

## Procedures to setup and run E2E test

1. Install React Native command line interface using NPM:
```
npm install -g react-native-cli
```

2. Download and install WinAppDriver [WinAppDriver v1.1](https://github.com/microsoft/WinAppDriver/releases/download/v1.1/WindowsApplicationDriver.msi)

3. Install node packages, build JS

- C:\repo>`cd react-native-windows`
- C:\repo\react-native-windows>`yarn install`
- C:\repo\react-native-windows>`yarn build`

4. Run the bundle server

- C:\repo\react-native-windows>`cd packages\E2ETest`
- C:\repo\react-native-windows\packages\E2ETest>`yarn run start`
- wait until you see 'Loading dependency graph, done.'

5. Ensure debugger is running

Open Chrome and navigate to `http://localhost:8081/debugger-ui/` in a new tab. Press `F12` or `Ctrl+Shift+I` in Chrome to open its Developer Tools.

6. Open a new command prompt, build native app, deploy and launch e2e testing

- C:\repo\react-native-windows>`cd packages\E2ETest`
- C:\repo\react-native-windows\packages\E2ETest>`yarn run e2e`

## Procedures to only run E2E test

Make sure bundle server is running(see above 'Run the bundle server' step) and chrome windows is open (see above 'Ensure debugger is running' step)

- run all specs

packages\E2ETest>`yarn run test`

-  Run one spec

packages\E2ETest>`yarn run testspec wdio\test\login.spec.ts`

## Commands help with build and test

| Command | Description | Example |
| --- | --- | --- |
| test | Run all specs | `yarn run test` |
| testspec | Run only one spec | `yarn run testspec wdio\test\login.spec.ts` |
| buildapp | build the native app with BUNDLE macro <BR/> `--release` specify if it's a release version <BR/>`--arch [string]` The build architecture (ARM, x86, x64) (default: &quot;x86&quot;) | `yarn run buildapp` <BR/> `yarn run buildapp --release` <BR/>`yarn run buildapp --arch x64` <BR/> `yarn run buildapp --arch x64 –release` |
| deployapp | Deploy the built test app, you can pair it with `--release` and `--arch` | `yarn run deployapp` <BR/> `yarn run deployapp --release` <BR/> `yarn run deployapp --arch x64` <BR/> `yarn run deployapp --arch x64 –release` |
| e2e | Build and deploy the solution (x86, debug), launch metro bundler and run all e2e specs | `yarn run e2e` |
| start | Launch the metro bundler | `yarn run start` |
| react-native run-windows | For details, see: react-native run-windows --help | `react-native run-windows --no-launch --no-packager --no-deploy --bundle` |

# Authoring E2E Test

## Create a new page for the test app

New test page should be in E2E/app/ or its subfolder.

Hooks are recommended to author the test page. (see [https://reactjs.org/docs/hooks-intro.html](https://reactjs.org/docs/hooks-intro.html) to learn more about Hooks)

```
// LoginTestPage.ts
export function LoginTestPage() {
  const [loginState, setLoginState] = useState('');
  …
  return (
    <View>
      <TextInput style={styles.input}
        placeholder='Email or Mobile Num'
        placeholderTextColor='rgba(225,225,225,0.7)'
        testID={USERNAME_ON_LOGIN}
        onChange={(text) => { setUserName(text.nativeEvent.text) }} />
   	…
 </View >);
}

```

## Add the new page to TestPages.ts

```
// TestPages.ts
const TestPages: ITestPage[] = [
…
  {
    testId: LOGIN_TESTPAGE,
    description: 'Login Test Page',
    content: LoginTestPage,
  },

```

## Put new testIDs in Consts.ts

```
//Consts.ts
export const USERNAME_ON_LOGIN = 'UserName';
```

## Create a Page Object to match with the page in test app

Page Objects should be put in E2ETest/wdio/pages and its subfolder.

```
// LoginPage.ts
class LoginPage extends BasePage {
  isPageLoaded() {
    return super.isPageLoaded() && this._userName.isDisplayed();
  }

  setLoginInfo(userName: string, password: string) {
    this._userName.setValue(userName);
    this._password.setValue(password);
  }

  submitForm() {
    this._submit.click();
  }

  private get _userName() {
    return By(USERNAME_ON_LOGIN);
  }

  private get _password() {
    return By(PASSWORD_ON_LOGIN);
  }

  private get _submit() {
    return By(SUBMIT_ON_LOGIN);
  }
}
export default new LoginPage();
```

Locator is defined in a `get` function and just returns By(testID), then you can use all element function like &#39;click&#39; which is defined in WebDriverIO.

Pay attention to the last line of the LoginPage, we always export a new instance of this object. It makes the test case more readable.

## Write a test spec to use the Page Object

```
// login.spec.ts
before(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGotoLoginPage();
});

describe('LoginTest', () => {
  it('Login Success', () => {
    LoginPage.setLoginInfo('username', 'password');
    LoginPage.submitForm();
    assert.equal(LoginPage.getLoginResult(), 'Success');
  });
```

# More
To understand more about the E2E testing, please refer to [More about E2E test](MoreAboutE2ETest.md)

# Restrictions

1. If you made any change to native code, you must rebuild the native app and redeploy it.
2. The same session can&#39;t be shared by multiple specs. The framework always kills the old app and launches a new session.

# Known issue

1. For `yarn run e2e` or `yarn run e2ebundle`, the test continues even if one of steps like build failed. see [bug 3136](https://github.com/microsoft/react-native-windows/issues/3136) for more details
