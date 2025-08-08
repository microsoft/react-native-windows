#!/usr/bin/env node

/**
 * Test script to manually verify the architecture prompt functionality
 */

const path = require('path');
const {promptForArchitectureChoice} = require(path.join(__dirname, 'lib-commonjs', 'utils', 'architecturePrompt'));

async function testPrompt() {
  console.log('Testing architecture prompt...\n');
  
  try {
    const result = await promptForArchitectureChoice('old/uwp-cpp-app', 3);
    console.log('\nResult:', result);
  } catch (error) {
    console.error('Error:', error);
  }
}

testPrompt();