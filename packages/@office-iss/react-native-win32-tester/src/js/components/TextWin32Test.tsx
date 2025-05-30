'use strict';
import React from 'react'
import { Button, Text, View, ViewWin32 } from 'react-native';

// Disabling no-jsx-lambda so functional components are more convenient to use

const TextRunsTest: React.FC<{}> = () => {
  return (
    <View focusable>
      <Text>
        <Text>Text With </Text>
        <Text>Multiple Text </Text>
        <Text>Children [Runs]</Text>
      </Text>
    </View>
  );
};

const FocusableTextTest: React.FC<{}> = () => {
  return (
    <ViewWin32>
      <Text focusable>This Text demonstrates focusable</Text>
    </ViewWin32>
  );
};

const SelectableTextTest: React.FC<{}> = () => {
  return (
    <ViewWin32>
      <Text selectable>This Text demonstrates selectable</Text>
    </ViewWin32>
  );
};

const TextStyleTest: React.FC<{}> = () => {
  return (
    <ViewWin32>
      <Text textStyle={'MediumBold'}>MediumBold TextStyle</Text>
    </ViewWin32>
  );
};

const TextAcessibilityTest: React.FC<{}> = () => {
  return (
    <ViewWin32>
      <Text accessibilityDescription="A11y description" >This Text text with accessibilityDescription</Text>
    </ViewWin32>
  );
};

const TooltipTextTest: React.FC<{}> = () => {
  return (
    <ViewWin32>
      <Text tooltip="Example Tooltip">This Text demonstrates a tooltip</Text>
    </ViewWin32>
  );
};

const TextPromotionTest: React.FC<{}> = () => {
  enum FocusabilityState {
    NoFocus,
    Focusable,
    Selectable,
  }
  const [focusabilityState, setFocusabilityState] = React.useState(FocusabilityState.NoFocus);
  return (
    <ViewWin32>
      <Button
      title="Click to Promote Focusability"
      onPress={() => 
        {
          if (focusabilityState === FocusabilityState.NoFocus)
          {
            setFocusabilityState(FocusabilityState.Focusable);
          }
          else if (focusabilityState === FocusabilityState.Focusable)
          {
            setFocusabilityState(FocusabilityState.Selectable);
          }
        }}  />
      <Text
        focusable={focusabilityState === FocusabilityState.Focusable}
        selectable={focusabilityState === FocusabilityState.Selectable}
      >
        This text is currently...
      </Text>
      <Text>
        {(focusabilityState === FocusabilityState.NoFocus) ? 'No Focusability' : 
         (focusabilityState === FocusabilityState.Focusable) ? 'Focusable' :
         'Selectable'}
      </Text>
    </ViewWin32>
  );
};

const BlurringAndFocusingTextTest: React.FC<{}> = () => {
  const [isFocusableFocused, setIsFocusableFocused] = React.useState(false);
  const [isSelectableFocused, setIsSelectableFocused] = React.useState(false);
  return (
    <View>
      <Text
        focusable
        onBlur={() => setIsFocusableFocused(false)}
        onFocus={() => setIsFocusableFocused(true)}>Focusable text is focused:</Text>
        <Text
        textStyle={isFocusableFocused ? 'MediumBold' : 'MediumStandard'}>{isFocusableFocused ? 'true' : 'false'}</Text>
      <Text
        selectable
        onBlur={() => setIsSelectableFocused(false)}
        onFocus={() => setIsSelectableFocused(true)}>Selectable text is focused:</Text>
        <Text
        textStyle={isSelectableFocused ? 'MediumBold' : 'MediumStandard'}>{isSelectableFocused ? 'true' : 'false'}</Text>
    </View>
  );
};

export const title = 'TextWin32';
export const displayName = 'TextWin32';
export const description = 'TextWin32 Examples and Tests';
export const examples = [
    {
      title: 'Text Runs Example',
      description: 'text runs in action',
      render() {
        return (<TextRunsTest />);
      },
    },
    {
      title: 'Focusable Example',
      description: 'focusable in action',
      render() {
        return (<FocusableTextTest />);
      },
    },
    {
      title: 'Selectable Example',
      description: 'selectable in action',
      render() {
        return (<SelectableTextTest />);
      },
    },
    {
      title: 'TextStyle Example',
      description: 'TextStyles in action',
      render() {
        return (<TextStyleTest />);
      },
    },
    {
      title: 'Acessibility Example',
      description: 'Acessibility on Text in action',
      render() {
        return (<TextAcessibilityTest />);
      },
    },
    {
      title: 'Tooltip Example',
      description: 'tooltips in action',
      render() {
        return (<TooltipTextTest />);
      },
    },
    {
      title: 'TextPromotion Example',
      description: 'dynamic increases in focusability in action',
      render() {
        return (<TextPromotionTest />);
      },
    },
    {
      title: 'Focus and Blur Example',
      description: 'onFocus/onBlur in action',
      render() {
        return (<BlurringAndFocusingTextTest />);
      },
    },
  ];