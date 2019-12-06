/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import {StyleSheet, View, Image, requireNativeComponent} from 'react-native'
import React from 'react';
import { IMAGE_TREE_DUMP_RESULT } from './Consts';
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
  imageContainer: {
    marginTop: 5,
    height:200,
    width:400,
    backgroundColor: 'orange',
  },
  treeDumpControl: {
    height: 90,
    width: 400,
    margin: 10,
  },
});

export function ImageTestPage() {
  return(
  <View style={styles.container}>
    <View style={styles.imageContainer}>
      <Image
        style={styles.image}
        resizeMode={'center'}
        source={{uri: 'http://facebook.github.io/react-native/img/header_logo.png'}}
      />      
    </View>    
    <TreeDumpControl style={styles.treeDumpControl} dumpID='ImageWithoutBorder' testID={IMAGE_TREE_DUMP_RESULT} />
  </View >);
}