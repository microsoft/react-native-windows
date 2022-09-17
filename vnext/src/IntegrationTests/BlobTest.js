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

const {TestModule, FileReader} = ReactNative.NativeModules;

type State = {
  statusCode: number,
  xhr: XMLHttpRequest,
  expected: String,
};

class BlobTest extends React.Component<{...}, State> {
  state: State = {
    statusCode: 0,
    xhr: new XMLHttpRequest(),
    // https://www.facebook.com/favicon.ico
    expected: new String(
      'data:application/octet-stream;base64,' +
        'AAABAAIAEBAAAAEAIABoBAAAJgAAACAgAAABACAAqBAAAI4EAAAoAAAAEAAAACAA' +
        'AAABACAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOFl' +
        'BiviZgKP4WYB1f//////////4WUA1eJmAI/hawYrAAAAAAAAAAAAAAAAAAAAAAAA' +
        'AAAAAAAA/4ArBuNpA5TkawP942kC/+NpAv///////////+NpAv/jaQL/5GoD/eNp' +
        'A5T/gCsGAAAAAAAAAAAAAAAA/4ArBuVvBL3lbgT/5W4E/+VuBP/lbgT/////////' +
        '///lbgT/5W4E/+VuBP/lbgT/5W8Evf+AKwYAAAAAAAAAAOlzBZTncwX/53MF/+dz' +
        'Bf/ncwX/53MF////////////53MF/+dzBf/ncwb/53MF/+dzBv/pcweUAAAAAO19' +
        'DCvpeAf96HcH/+l4B//odwf/6XgH/+l4B////////////+h3B//odwf/6XgH/+h3' +
        'B//peAf/6nkH/e19DCvrfQmP630J/+t9Cf/rfAn/630J/+t8Cf/rfAn/////////' +
        '///rfQn/630J/+p8CP/rfQn/6nwI/+p8CP/rfQuP7YEL1e2BCv/tgQr/7IEK/+2B' +
        'Cv/////////////////////////////////uiRj/7IEK/+2CCv/tggr/7YIM1e6G' +
        'DfPvhgz/74YM/++HDP/vhgz/////////////////////////////////8Zw4/++G' +
        'DP/uhgz/7oYM/+6GDPPwiw7z8IsN//CLDf/wiw3/8IsN//CLDf/wiw3/////////' +
        '///wig3/8IoN//CLDf/wig3/8IsN//CLDf/xjA/z85EQ1fOQD//zkA//85AP//OQ' +
        'D//zkA//85AP////////////8o8P//KPD//ykA//8o8P//KQD//ykA//85EQ1fSU' +
        'EI/1lRH/9ZUR//SUEP/1lRH/9JQQ//SUEP/+9uz///////jDev/0mRz/9ZUR//SV' +
        'Ef/1lRH/9ZUR//SUEI/5mhgr95kS/faZEv/2mRL/9pkS//aZEv/2mRL//Nqo////' +
        '//////////////rLhv/3mhL/9pkS//eZEv35mhgrAAAAAPifFZT4nhT/+Z8U//ie' +
        'FP/5nxT/+Z8U//ikI//83a3//vjw//78+f/7yX3/+J4T//ieFP/4nxWUAAAAAAAA' +
        'AAD/qisG+6MWvfqjFf/6oxX/+qMV//qjFf/6oxX/+qMV//qjFf/6oxX/+qIV//qj' +
        'Ff/7oxa9/6orBgAAAAAAAAAAAAAAAP+qKwb9qRiU/agW/fyoF//8qBf//agX//yo' +
        'F//9qBf//agX//2oF/39qRiU/6orBgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP+y' +
        'Hiv/rRmP/6wZ1f+tGPP/rBjz/64Z1f+vGY//sh4rAAAAAAAAAAAAAAAAAAAAAPAP' +
        'AADAAwAAgAEAAIABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAB' +
        'AACAAQAAwAMAAPAPAAAoAAAAIAAAAEAAAAABACAAAAAAAAAQAAAAAAAAAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP+A' +
        'AAbiZQRH4GMAlf//////////////////////////4GQAv+BjAJXiZQBH/4AABgAA' +
        'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAOpqCxjiZgKW4WYB8eJmAf/hZQH/////////' +
        '///////////////////hZgH/4mYB/+FmAf/iZwHx4mYClupqCxgAAAAAAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP9t' +
        'JAfkagSC42kC9ONoAv/jaAL/4mgC/+NoAv///////////////////////////+Jo' +
        'Af/iaAL/4mgB/+JoAv/iaAL/42kC9ORqBIL/bSQHAAAAAAAAAAAAAAAAAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADqagsY5GoEx+NqA//jagP/42oD/+Nq' +
        'A//kawP/42oD////////////////////////////42oD/+RrA//jagP/5GsD/+Rr' +
        'A//kawP/5GsD/+RsBMfqdQsYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA' +
        'AAAAAAAA6HEGLeVuBOPlbQT/5GwD/+VtBP/kbAP/5GwD/+VtBP/kbAP/////////' +
        '///////////////////kbAP/5G0D/+RsA//kbQP/5G0D/+RtA//kbQP/5G0D/+Ru' +
        'A+PocQYtAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOp1CxjmcAbj5W8E/+Vv' +
        'BP/lbwT/5W8E/+VvBP/lbwT/5W8E/+VvBP///////////////////////////+Vv' +
        'BP/mcAX/5W8E/+ZwBf/mcAX/5W8E/+ZwBf/lbwT/5W8E/+ZwBuPqdQsYAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAAD/kiQH53IFx+ZyBf/mcQX/5nEF/+ZxBf/mcQX/5nEF/+Zx' +
        'Bf/ncgX/5nEF////////////////////////////53IG/+ZxBf/ncgb/5nEF/+Zx' +
        'Bf/mcgX/5nEF/+ZyBf/mcgX/5nEF/+dyBcf/kiQHAAAAAAAAAAAAAAAAAAAAAOd2' +
        'CILodAb/53QG/+h0Bv/odAb/6HQG/+h0Bv/odAb/6HQG/+d0Bv/odAb/////////' +
        '///////////////////ndAX/53QG/+d0Bf/ndAb/53QG/+h1Bv/ndAb/6HUG/+h1' +
        'Bv/ndAX/6HUG/+d2BoIAAAAAAAAAAAAAAADqgAsY6HYG9Oh2B//odgb/6HYH/+h2' +
        'B//odgb/6HYH/+h2Bv/odgb/6HYH/+h2Bv///////////////////////////+l3' +
        'B//odgb/6XcH/+h2Bv/odgb/6HYH/+h2Bv/odgf/6HYH/+h2Bv/odgf/6HYG9OqA' +
        'CxgAAAAAAAAAAOt6CZbpeQj/6nkI/+l5CP/qeQj/6nkI/+l5B//qeQj/6XkH/+l5' +
        'B//peQf/6XkH////////////////////////////6XkI/+l5B//peQj/6XkH/+l5' +
        'B//peQj/6XkH/+l5CP/peQj/6XkH/+l5CP/peQf/63oJlgAAAAD/gCsG7H0K8et8' +
        'Cf/qewj/63wJ/+p7CP/qewj/6nsI/+p7CP/qewj/6nsI/+t8Cf/qewj/////////' +
        '///////////////////qewj/6nwJ/+p7CP/qfAn/6nwJ/+p7CP/qfAn/6nsI/+p7' +
        'CP/rfAn/6nsI/+t8Cf/sfQrx/4ArBu2BC0frfQn/630J/+t+Cf/rfQn/634J/+t+' +
        'Cf/rfgn/634J////////////////////////////////////////////////////' +
        '///////////////////zs27/634J/+x+Cf/rfgn/634J/+t+Cf/rfgn/634J/+t+' +
        'Cf/tgQtH7IAKleyACv/sgAr/7IAK/+yACv/sgAr/7IAK/+yACv/sgAr/////////' +
        '//////////////////////////////////////////////////////////////XC' +
        'iv/sgAr/7IAK/+yACv/sgAr/7IAJ/+yACv/sgAn/7IAJ/+yACpXugwu/7YML/+2D' +
        'C//tggr/7YML/+2CCv/tggr/7YIK/+2CCv//////////////////////////////' +
        '////////////////////////////////////////+NKn/+2DC//tggr/7YML/+2D' +
        'C//tgwv/7YML/+2DC//tgwv/7oMLv++GDNnuhQv/7oUL/+6FC//uhQv/7oUL/+6F' +
        'C//vhQz/7oUL////////////////////////////////////////////////////' +
        '///////////////////64cT/7oUL/+6FC//uhQv/7oUL/+6EC//uhQv/7oQL/+6E' +
        'C//vhgzZ74gO8++IDP/viAz/74cM/++IDP/vhwz/74cM/++HDP/vhwz/////////' +
        '//////////////////////////////////////////////////////////////3w' +
        '4f/viA3/74cM/++IDf/viA3/74cM/++IDf/vhwz/74cM/++HDfPwiw7z8IoN//CK' +
        'Df/wig3/8IoN//CKDf/wig3/8IkN//CKDf/wiQ3/8IkN//CKDf/wiQ3/////////' +
        '///////////////////wiQ3/8IoN//CJDf/wig3/8IoN//CJDf/wig3/8IkN//CJ' +
        'Df/wiQ3/8IkN//CJDf/wiQ3/8IsO8/KNDtnxjA7/8YwO//GMDf/xjA7/8YwN//GM' +
        'Df/xjA3/8YwN//GMDf/xjA3/8YwO//GMDf////////////////////////////GM' +
        'Dv/xjA7/8YwO//GMDv/xjA7/8YwO//GMDv/xjA7/8YwO//GMDv/xjA7/8YwO//GM' +
        'Dv/yjQ7Z8o8Pv/KPD//yjw//8o8P//KPD//yjw//8o8P//KPD//yjw//8o8P//KP' +
        'D//yjg7/8o8P////////////////////////////8Y4O//KODv/xjg7/8o4O//KO' +
        'Dv/yjg7/8o4O//KODv/yjg7/8o8P//KODv/yjw//8o8P//OQEL/zkQ+V85EP//OR' +
        'D//zkQ//85EP//ORD//zkQ//85EP//ORD//zkQ//85EP//OREP/zkQ///vr0////' +
        '///////////////////0myb/85EQ//ORD//zkRD/85EQ//ORD//zkRD/85EP//OR' +
        'D//zkQ//85EP//ORD//zkQ//85EPlfSXEkf0kxD/9JMQ//SUEP/0kxD/9JQQ//SU' +
        'EP/zkxD/9JQQ//OTEP/zkxD/9JQQ//OTEP/86tD///////////////////////rV' +
        'ov/1nSb/85MQ//STEP/0kxD/9JMQ//STEP/0kxD/9JMQ//SUEP/0kxD/9JQQ//SU' +
        'EP/0kxJH/6orBvWWEvH1lhH/9ZYR//WWEf/1lhH/9ZYR//WWEf/1lhH/9ZYR//WW' +
        'Ef/1lhH/9ZYR//vZq///////////////////////////////////////////////' +
        '///1lhH/9ZYR//WWEf/1lhH/9ZYR//WWEf/1lhH/9ZYS8f+qKwYAAAAA95kTlvaY' +
        'Ev/2mBH/9pgS//aYEf/2mBH/9ZgR//aYEf/1mBH/9ZgR//aYEv/1mBH/+LFN////' +
        '//////////////////////////////////////////////aYEv/1mBH/9pgS//aY' +
        'Ev/1mBH/9pgS//WYEf/3mRGWAAAAAAAAAAD/nxUY+JwU9PebE//3mxP/95sT//eb' +
        'E//3mxP/95sT//ebE//3mxP/95oS//ebE//3mhL//OK7////////////////////' +
        '////////////////////////95sT//ebE//3mxP/95sT//aaEv/3mxP/95sT9P+f' +
        'FRgAAAAAAAAAAAAAAAD5nxSC+J0T//idE//4nRP/+J0T//ecE//4nRP/95wT//ec' +
        'E//4nRP/95wT//idE//4pSf//efF////////////////////////////////////' +
        '///4nRP/950T//idE//4nRP/+J0T//idE//5nxSCAAAAAAAAAAAAAAAAAAAAAP+2' +
        'JAf6oBXH+aAU//mgFP/5oBT/+J8U//mgFP/4nxT/+J8U//mfFP/4nxT/+Z8U//mf' +
        'FP/5oRf/+86H//7w2v/+/Pj//v36//758f/+8+P//evQ//mgFP/5nxT/+aAU//mg' +
        'FP/4nxT/+qAVx/+2JAcAAAAAAAAAAAAAAAAAAAAAAAAAAP+qFRj7oxXj+aEU//mh' +
        'FP/6ohX/+aEU//qiFf/6ohX/+qIV//qiFf/6ohX/+qIV//mhFP/6ohX/+aEU//mh' +
        'FP/6ohX/+aEU//qiFf/6ohX/+aEU//qiFf/5oRT/+aEU//ujFeP/qhUYAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAP+mFi78pBXj+6QV//ukFv/7pBX/+6QW//uk' +
        'Fv/6pBX/+6QW//qkFf/6pBX/+6QW//qkFf/7pBb/+6QW//ulFv/7pBb/+6UW//ul' +
        'Fv/7pBX/+6UW//ukFf/8pBXj/6QXLQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA' +
        'AAAAAAAAAAAAAP+qIBj8qBfH/KcW//ynF//8pxb//KcW//ynFv/8pxb//KcW//yn' +
        'Fv/7phb//KcW//umFv/7phb/+6YW//umFv/7phb/+6YW//ynFv/7phb//KgXx/+q' +
        'IBgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP+2' +
        'JAf9qxiC/akY9PypF//8qRf//KgX//ypF//8qBf//KgX//2pF//8qBf//akX//2p' +
        'F//9qRf//akX//2pF//9qRf//qkY9P2rGIL/tiQHAAAAAAAAAAAAAAAAAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/tSAY/60alv+s' +
        'GPH+rBj//qwY//6sGP/+rBj//asY//6sGP/9qxj//asY//2rF//9qxj//qsX8f2s' +
        'GJb/tSAYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/9UrBv+wGUf/rxqV/68Zv/+v' +
        'Gtn/rhnz/64Z8/+vGtn/rxm//68alf+wGUf/1SsGAAAAAAAAAAAAAAAAAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAP/AA///AAD//AAAP/gAAB/wAAAP4AAAB8AA' +
        'AAPAAAADgAAAAYAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA' +
        'AAAAAAAAAAAAAAAAAACAAAABgAAAAcAAAAPAAAAD4AAAB/AAAA/4AAAf/AAAP/8A' +
        'AP//wAP/',
    ),
  };

  _get = () => {
    this.state.xhr.onloadend = () => {
      this.setState({
        statusCode: this.state.xhr.status,
      });
    };
    this.state.xhr.open('GET', 'https://www.facebook.com/favicon.ico');
    this.state.xhr.setRequestHeader('Accept-Encoding', 'utf-8');
    this.state.xhr.responseType = 'blob';
    this.state.xhr.send();
  };

  _getSucceeded = () => {
    return this.state.statusCode === 200 && this.state.xhr.response !== null;
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
    this._get();
    this._waitFor(this._getSucceeded, 6, (doneSucceeded) => {
      let reader = new FileReader();
      reader.readAsDataURL(this.state.xhr.response);
      reader.onload = () => {
        TestModule.markTestPassed(
          doneSucceeded && this.state.expected === reader.result,
        );
      };
    });
  }

  render(): React.Node {
    return <View />;
  }
}

AppRegistry.registerComponent('BlobTest', () => BlobTest);

module.exports = BlobTest;
