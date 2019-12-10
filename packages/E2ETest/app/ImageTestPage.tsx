/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import {StyleSheet, View, Image, Button, requireNativeComponent} from 'react-native'
import React, { useState } from 'react';
import { TREE_DUMP_RESULT, SHOW_IMAGE_BORDER, IMAGE_ELEMENT } from './Consts';
const TreeDumpControl = requireNativeComponent('TreeDumpControl');

const styles = StyleSheet.create({
  container: {
    height:220,
    width:450,
  },
  imageWithBorder: {
    height: '100%',
    width: '100%',
    borderRadius: 10.0,
    borderWidth:10,
    borderColor: 'green',
    backgroundColor: 'red',
  },
  image: {
    height: '100%',
    width: '100%',
    backgroundColor: 'red',
  },  
  buttonContainer: {
    backgroundColor: '#2980b6',
    paddingVertical: 15
  },
  buttonText: {
    color: '#fff',
    textAlign: 'center',
    fontWeight: '700'
  },  
  treeDumpControl: {
    height: 150,
    width: 450,
    margin: 10,
  },
});

export function ImageTestPage() {
  const [imageWithBorder, setImageBorder] = useState(false);
  const onOressBorder = () => {
    var previousImageBorderState = imageWithBorder;
    setImageBorder(!previousImageBorderState);
 }
  return(
  <View style={styles.container}>
    <Image
      testID={IMAGE_ELEMENT}
      style={imageWithBorder?styles.imageWithBorder:styles.image}
      resizeMode={'center'}
      source={{uri: 'http://facebook.github.io/react-native/img/header_logo.png'}}
    />      
    <Button title= {imageWithBorder?"Hide Border":"Show Border"} 
        onPress={onOressBorder} 
        testID={SHOW_IMAGE_BORDER}/> 
    <TreeDumpControl style={styles.treeDumpControl} dumpID={imageWithBorder?'ImageWithBorder':'ImageWithoutBorder'} uiaID={IMAGE_ELEMENT} testID={TREE_DUMP_RESULT} />
  </View >);
}