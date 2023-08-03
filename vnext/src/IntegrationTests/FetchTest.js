/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */
import React, { useEffect, useState } from 'react';
import { Button, Text, View } from 'react-native';

const { TestModule } = ReactNative.NativeModules;

const FetchTest = () => {
  const [content, setContent] = useState('NOTHING');
  const [reqId, setReqId] = useState(0);
  const uri =
    'https://raw.githubusercontent.com/microsoft/react-native-windows/main/.yarnrc.yml';
    //'http://localhost:5555';

  const doFetch = async () => {
    var response = await fetch(uri);
    var text = await response.text();
    setReqId(reqId + 1);
    setContent(text);
  };

  useEffect(() => {
    doFetch().catch(console.error);
    TestModule.markTestPassed(true);
  }, []);

  return (
    <View />
  );

//  return (
//    <View
//      style={{
//        flex: 1,
//        justifyContent: 'flex-start',
//        alignItems: 'stretch',
//      }}>
//      <Text>Response {reqId}:</Text>
//      <Text>[{content}]</Text>
//      <Button onPress={doFetch} title="Reload" />
//    </View>
//  );
};

export default FetchTest;
