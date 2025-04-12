/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React, { useState } from 'react';
import { View, Text, Button, StyleSheet } from 'react-native';

const MultiContainerComponent = () => {
  const [text, setText] = useState("This is a normal text");
  const [nooflines, setNoOfLines] = useState(0);

  const addText = () => {
    setText(text + " Added text");
  };
  
  const sliceFromEnd = () => {
    setText(prevText => prevText.slice(0,Math.max(21,text.length-11)));
  };

  const Reset = () => {  
    setText("This is a normal text");
    setNoOfLines(0);
  };

  return (
    <View style={styles.mainContainer}>
      <View style={styles.container1}>
        <Text numberOfLines={nooflines} adjustsFontSizeToFit style={[styles.text,{maxHeight:130}]}>{text}</Text>
      </View>
      <View style={styles.container2}>
        <Text numberOfLines={nooflines} adjustsFontSizeToFit style={[styles.text,{maxHeight:80}]}>{text}</Text>
      </View>
      <Text>Number of Lines {nooflines}</Text>
      <View style={{flexDirection:'row'}}>
        <View style={styles.buttonContainer}>
          <Button color='black' title="Add Text" onPress={addText} />
        </View>
        <View style={styles.buttonContainer}>
          <Button color='blue' title="Reduce" onPress={sliceFromEnd} />
        </View>
        <View style={styles.buttonContainer}>
          <Button color='green' title="LineCount" onPress={()=>{setNoOfLines(prev=>prev+1)}} />
        </View>
        <View style={styles.buttonContainer}>
          <Button color='red' title="Reset" onPress={Reset} />
        </View>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  mainContainer: {
    flex: 1,
    alignItems: 'center',
  },
  container1: {
    backgroundColor: 'lightblue',
    padding: 10,
    marginBottom: 10,
    width:800,
    height: 150,
  },
  container2: {
    backgroundColor: 'lightcoral',
    padding: 10,
    marginBottom: 10,
    width:500,
    height:100,
  },
  buttonContainer: {
    justifyContent:"center",
    marginTop: 10,
    padding:10
  },
  text: {
    fontSize: 18,
  },
});

export default MultiContainerComponent;
