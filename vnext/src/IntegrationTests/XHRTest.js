/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */
'use strict';

const React = require('react');
const ReactNative = require('react-native');

const {AppRegistry, View} = ReactNative;

const {TestModule} = ReactNative.NativeModules;

type State = {
  done: ?number,
  xhr:  ?XMLHttpRequest,
};

class XHRTest extends React.Component<{...}, State> {
  //state: Object = {
  //  downloading: false,
  //  // set by onreadystatechange
  //  contentLength: 1,
  //  responseLength: 0,
  //  // set by onprogress
  //  progressTotal: 1,
  //  progressLoaded: 0,

  //  readystateHandler: false,
  //  progressHandler: true,
  //  arraybuffer: false,
  //};

  state: State = {
    done: 0,
    xhr: null,
  };

  //xhr: ?XMLHttpRequest = null;

  //_download = () => {
  //  return new Promise((resolve, reject) => {
  //    let xhr;
  //    if (this.xhr) {
  //      xhr = this.xhr;
  //      xhr.abort();
  //    } else {
  //      xhr = this.xhr = new XMLHttpRequest();
  //    }

  //    const onreadystatechange = () => {
  //      if (xhr.readyState === xhr.HEADERS_RECEIVED) {
  //        const contentLength = parseInt(
  //          xhr.getResponseHeader('Content-Length'),
  //          10,
  //        );
  //        this.setState({
  //          contentLength,
  //          responseLength: 0,
  //        });
  //      } else if (xhr.readyState === xhr.LOADING && xhr.response) {
  //        this.setState({
  //          responseLength: xhr.response.length,
  //        });
  //      }
  //    };

  //    if (this.state.readystateHandler) {
  //      xhr.onreadystatechange = onreadystatechange;
  //    }
  //    //ISS:2306365 - Uncomment when native module is complete.
  //    //if (this.state.progressHandler) {
  //    //  xhr.onprogress = onprogress;
  //    //}
  //    if (this.state.arraybuffer) {
  //      xhr.responseType = 'arraybuffer';
  //    }
  //    xhr.onload = () => {
  //      this.setState({downloading: false});
  //      if (xhr.status === 200) {
  //        let responseType = `Response is a string, ${xhr.response.length} characters long.`;
  //        if (xhr.response instanceof ArrayBuffer) {
  //          responseType = `Response is an ArrayBuffer, ${xhr.response.byteLength} bytes long.`;
  //        }
  //        console.log('Download complete!', responseType);
  //      } else if (xhr.status !== 0) {
  //        console.error(
  //          'Error',
  //          `Server returned HTTP status of ${xhr.status}: ${xhr.responseText}`,
  //        );
  //      } else {
  //        console.error('Error', xhr.responseText);
  //      }
  //    };
  //    xhr.open('GET', 'https://en.wikipedia.org/favicon.ico');
  //    // Avoid gzip so we can actually show progress
  //    xhr.setRequestHeader('Accept-Encoding', '');
  //    xhr.send();

  //    this.setState({downloading: true});
  //  });
  //};

  _do = () => {
    this.xhr.onload = () => {
      console.log('onload');

      this.setState({
        done: 1
      });
    };
    this.xhr.open('GET', 'https://raw.githubusercontent.com/microsoft/react-native-windows/react-native-windows_v0.67.1/NuGet.Config');
    this.xhr.setRequestHeader('Accept-Encoding', '');
    this.xhr.send();
  };

  _fetch = () => {
    fetch('https://raw.githubusercontent.com/microsoft/react-native-windows/react-native-windows_v0.67.1/NuGet.Config').then(res => {
      console.log('fetched');
      return res.text();
    }).then(text => {
      console.log('textd');
    });
  };

  _done = () => {
    console.log(`_done ${this.state.done}`);
    return this.state.done === 1;
  };

  _waitFor = (condition: any, timeout: any, callback: any) => {
    let remaining = timeout;
    const timeoutFunction = function () {
      if (condition()) {
        callback(true);
        return;
      }
      remaining--;
      if (remaining === 0) {
        callback(false);
      } else {
        setTimeout(timeoutFunction, 1000);
      }
    };
    setTimeout(timeoutFunction, 1000);
  };

  componentDidMount() {
    this.xhr = new XMLHttpRequest();
    this._do();
    //this._fetch();
    this._waitFor(this._done, 5, doneSucceeded => {
      TestModule.markTestPassed(doneSucceeded);
    });
  }

  render(): React.Node {
    return <View />;
  }
}

AppRegistry.registerComponent('XHRTest', () => XHRTest);

module.exports = XHRTest;
