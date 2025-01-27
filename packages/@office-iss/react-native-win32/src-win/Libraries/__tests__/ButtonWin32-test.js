
'use strict';

const React = require('react');
const renderer = require('react-native/jest/renderer');

const {ButtonWin32} = require('@office-iss/react-native-win32');

describe('ButtonWin32', () => {
  it('renders native component', async () => {
    expect(
      await renderer.create(
        <ButtonWin32/>,
      ),
    ).toMatchSnapshot();
  });
});