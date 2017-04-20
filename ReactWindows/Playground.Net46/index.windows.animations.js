/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  View,
  TouchableOpacity,
  Animated  
} from 'react-native';

class Playground extends Component {
  constructor (props) {
      super(props)
      this.state = {
      }
      this.tx = new Animated.Value(0)
      this.ty = new Animated.Value(0)
      this.p = new Animated.Value(0)
      this.r = new Animated.Value(0)
      this.rx = new Animated.Value(0)
      this.ry = new Animated.Value(0)
      this.rz = new Animated.Value(0)
      this.s = new Animated.Value(0)
      this.sx = new Animated.Value(0)
      this.sy = new Animated.Value(0)
      this.kx = new Animated.Value(0)
      this.ky = new Animated.Value(0)
    }

renderButton (title, action) {
    return (
      <TouchableOpacity style={styles.button} onPress={action}>
        <Text style={styles.buttonText}>{title}</Text>
      </TouchableOpacity>
    )
  }

  translateX = () => {
    Animated.timing(this.tx, { toValue: 1, duration: 1000 }).start()
  }

  translateY = () => {
    Animated.timing(this.ty, { toValue: 1, duration: 1000 }).start()
  }

  // perspective = () => {
  //   Animated.timing(this.p, { toValue: 1, duration: 1000 }).start()
  // }

  perspective = () => {
    Animated.parallel([
    Animated.timing(this.p, { toValue: 1, duration: 1000 }),
    Animated.timing(this.ry, { toValue: 1, duration: 1000 })
    ]).start()
  }

  rotate = () => {
    Animated.timing(this.r, { toValue: 1, duration: 1000 }).start()
  }

  rotateX = () => {
    Animated.timing(this.rx, { toValue: 1, duration: 1000 }).start()
  }

  rotateY = () => {
    Animated.timing(this.ry, { toValue: 1, duration: 1000 }).start()
  }

  rotateZ = () => {
    Animated.timing(this.rz, { toValue: 1, duration: 1000 }).start()
  }

  scale = () => {
    Animated.timing(this.s, { toValue: 1, duration: 1000 }).start()
  }

  scaleX = () => {
    Animated.timing(this.sx, { toValue: 1, duration: 1000 }).start()
  }

  scaleY = () => {
    Animated.timing(this.sy, { toValue: 1, duration: 1000 }).start()
  }

  skewX = () => {
    Animated.timing(this.kx, { toValue: 1, duration: 1000 }).start()
  }

  skewY = () => {
    Animated.timing(this.ky, { toValue: 1, duration: 1000 }).start()
  }

  rotateZandSkewX = () => {
    Animated.parallel([
    Animated.timing(this.rz, { toValue: 1, duration: 1000 }),
    Animated.timing(this.kx, { toValue: 1, duration: 1000 })
    ]).start()
  }

  reset = () => {
    Animated.parallel([
      Animated.timing(this.tx, { toValue: 0, duration: 1000 }),
      Animated.timing(this.ty, { toValue: 0, duration: 1000 }),
      Animated.timing(this.p, { toValue: 0, duration: 1000 }),
      Animated.timing(this.r, { toValue: 0, duration: 1000 }),
      Animated.timing(this.rx, { toValue: 0, duration: 1000 }),
      Animated.timing(this.ry, { toValue: 0, duration: 1000 }),
      Animated.timing(this.rz, { toValue: 0, duration: 1000 }),
      Animated.timing(this.s, { toValue: 0, duration: 1000 }),
      Animated.timing(this.sx, { toValue: 0, duration: 1000 }),
      Animated.timing(this.sy, { toValue: 0, duration: 1000 }),
      Animated.timing(this.kx, { toValue: 0, duration: 1000 }),
      Animated.timing(this.ky, { toValue: 0, duration: 1000 }),
    ]).start()
  }

  renderObjectAll () {
    return (
    <Animated.View
      style={[styles.obj, {
        transform: [
          {translateX: this.tx.interpolate({
            inputRange: [0, 1],
            outputRange: [0, 400]
          })},
          {translateY: this.ty.interpolate({
            inputRange: [0, 1],
            outputRange: [0, 200]
          })},
          {scale: this.s.interpolate({
            inputRange: [0, 1],
            outputRange: [1, 2]
          })},
          {scaleX: this.sx.interpolate({
            inputRange: [0, 1],
            outputRange: [1, 2]
          })},
          {scaleY: this.sy.interpolate({
            inputRange: [0, 1],
            outputRange: [1, 2]
          })},
          {rotate: this.r.interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '45deg']
          })},
          {rotateX: this.rx.interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '45deg']
          })},
          {rotateY: this.ry.interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '45deg']
          })},
          {rotateZ: this.rz.interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '45deg']
          })},      
          {skewX: this.kx.interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '45deg']
          })},
          {skewY: this.ky.interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '45deg']
          })}
        ]}
      ]}>
      <Text style={styles.objText}>All</Text>
    </Animated.View>
    )
  }

  renderObjectRotate () {
    return (
      <Animated.View
        style={[styles.obj, {
          transform: [
            {rotate: this.r.interpolate({
              inputRange: [0, 1],
              outputRange: ['0deg', '45deg']
            })},
            {rotateX: this.rx.interpolate({
              inputRange: [0, 1],
              outputRange: ['0deg', '180deg']
            })},
            {rotateY: this.ry.interpolate({
              inputRange: [0, 1],
              outputRange: ['0deg', '180deg']
            })},
            {rotateZ: this.rz.interpolate({
              inputRange: [0, 1],
              outputRange: ['0deg', '180deg']
            })}
          ]}
        ]}>
        <Text style={styles.objText}>Rotate</Text>
      </Animated.View>
    )
  }

  renderObjectScale () {
    return (
      <Animated.View
        style={[styles.obj, {
          transform: [
            {scale: this.s.interpolate({
              inputRange: [0, 1],
              outputRange: [1, 2]
            })},
            {scaleX: this.sx.interpolate({
              inputRange: [0, 1],
              outputRange: [1, 2]
            })},
            {scaleY: this.sy.interpolate({
              inputRange: [0, 1],
              outputRange: [1, 2]
            })}
          ]}
        ]}>
        <Text style={styles.objText}>Scale</Text>
      </Animated.View>
    )
  }

  renderObjectSkew () {
    return (
      <Animated.View
          style={[styles.obj, {
            transform: [
              {skewX: this.kx.interpolate({
                inputRange: [0, 1],
                outputRange: ['0deg', '45deg']
              })},
              {skewY: this.ky.interpolate({
                inputRange: [0, 1],
                outputRange: ['0deg', '45deg']
              })}
            ]}
        ]}>
        <Text style={styles.objText}>Skew</Text>
      </Animated.View>
    )
  }

  renderObjectTranslate () {
  return (
    <Animated.View
      style={[styles.obj, {
        transform: [
          {translateX: this.tx.interpolate({
            inputRange: [0, 1],
            outputRange: [0, 400]
          })},
          {translateY: this.ty.interpolate({
            inputRange: [0, 1],
            outputRange: [0, 200]
          })}
        ]}
      ]}>
      <Text style={styles.objText}>Translate</Text>
    </Animated.View>
    )
  }

  renderObjectMultiple () {
  return (
    <Animated.View
      style={[styles.obj, {
        transform: [
          {skewX: this.kx.interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '45deg']
          })},
          {rotateZ: this.rz.interpolate({
            inputRange: [0, 1],
            outputRange: ['0deg', '45deg']
          })}
        ]}
      ]}>
      <Text style={styles.objText}>Multiple</Text>
    </Animated.View>
    )
  }

  renderObjectPerspective () {
    return (
    <Animated.View
      style={[styles.obj, {
        transform: [
            {perspective: this.p.interpolate({
              inputRange: [0, 1],
              outputRange: [1, 850]
            })},
            {rotateY: this.ry.interpolate({
              inputRange: [0, 1],
              outputRange: ['0deg', '50deg']
            })}            
          ]}
        ]}>
      <Text style={styles.objText}>Perspective</Text>
    </Animated.View>
    )
  }

  render() {
    return (
      <View style={styles.container}>
        
        <View style={styles.playground}>
          {this.renderObjectAll()}
          {this.renderObjectTranslate()}
          {this.renderObjectScale()}
          {this.renderObjectRotate()}
          {this.renderObjectSkew()}
        </View>
        <View style={styles.buttonPanel}>
            {this.renderButton('translateX', this.translateX)}
            {this.renderButton('translateY', this.translateY)}
            {this.renderButton('perspective', this.perspective)}
            {this.renderButton('rotate', this.rotate)}
            {this.renderButton('rotateX', this.rotateX)}
            {this.renderButton('rotateY', this.rotateY)}
            {this.renderButton('rotateZ', this.rotateZ)}
            {this.renderButton('scale', this.scale)}
            {this.renderButton('scaleX', this.scaleX)}
            {this.renderButton('scaleY', this.scaleY)}
            {this.renderButton('skewX', this.skewX)}
            {this.renderButton('skewY', this.skewY)}
            {this.renderButton('multiple', this.rotateZandSkewX)}
            {this.renderButton('reset', this.reset)}
        </View>
      </View>
    )
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    flexDirection: 'row'
  },
  buttonPanel: {
    width: 250,
    alignItems: 'center',
    justifyContent: 'center'
  },
  playground: {
    flex: 1,
    padding: 50
  },
  button: {
    width: 200,
    height: 40,
    marginBottom: 5,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: 'rgb(33, 33, 33)'
  },
  buttonText: {
    color: 'white'
  },
  obj: {
    width: 80,
    height: 80,
    margin: 10,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: 'navy'
  },
  objText: {
    color: 'white'
  }
})

AppRegistry.registerComponent('Playground.Net46', () => Playground);
