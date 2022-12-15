
'use strict';

const React = require('react');
const ReactTestRenderer = require('react-test-renderer');

const {ButtonWin32} = require('../Components/Button/ButtonWin32');

describe('ButtonWin32', () => {
  it('renders native component', () => {
    const component = ReactTestRenderer.create(
      <ButtonWin32/>
    );
    expect(component).toMatchSnapshot();
  });
});