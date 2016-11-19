using Facebook.CSSLayout;
using NUnit.Framework;
using ReactNative.UIManager;
using ReactNative.Views.Text;
using System;
using ReactNative.UIManager.Events;
using System.Threading;
using System.Windows.Controls;
using System.Windows;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Windows.Automation;
using ReactNative.Shared.Tests.Internal;
using System.Windows.Media;
using System.Windows.Threading;
using ReactNative.Views.View;
using System.Windows.Documents;

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    public class ReactTextViewManagerTests
    {
        private List<string> referenceIgnoreDifferencesList;

        [Test]
        public void ReactTextViewManagerTests_ArgumentChecks()
        {
            var viewManager01 = new ReactTextViewManager();

            Assert.AreEqual("RCTText", viewManager01.Name);

            var ex1 = Assert.Throws<ArgumentNullException>(() => new ReactTextViewManager(null));

            var mockEventDispatcher = new MockEventDispatcher();

            Assert.DoesNotThrow(() => new ReactTextViewManager(mockEventDispatcher));

            var viewManager02 = new ReactTextViewManager(mockEventDispatcher);

            Assert.AreEqual("RCTText", viewManager02.Name);
            Assert.NotNull(viewManager02.EventDispatcher);
        }

        [Test]
        public void ReactTextViewManagerTests_NativeProperties_Constants()
        {
            var viewManager = new ReactTextViewManager();

            var constants = viewManager.NativeProperties;

            Assert.IsTrue(constants.ContainsKey("color"));
            Assert.IsTrue(constants.ContainsKey("selectable"));
            Assert.IsTrue(constants.ContainsKey("opacity"));
            Assert.IsTrue(constants.ContainsKey("zIndex"));
            Assert.IsTrue(constants.ContainsKey("accessibilityLabel"));
            Assert.IsTrue(constants.ContainsKey("testID"));
            Assert.IsTrue(constants.ContainsKey("fontSize"));
            Assert.IsTrue(constants.ContainsKey("fontFamily"));
            Assert.IsTrue(constants.ContainsKey("fontWeight"));
            Assert.IsTrue(constants.ContainsKey("fontStyle"));
            Assert.IsTrue(constants.ContainsKey("letterSpacing"));
            Assert.IsTrue(constants.ContainsKey("lineHeight"));
            Assert.IsTrue(constants.ContainsKey("numberOfLines"));
            Assert.IsTrue(constants.ContainsKey("textAlign"));
            Assert.IsTrue(constants.ContainsKey("width"));
            Assert.IsTrue(constants.ContainsKey("minWidth"));
            Assert.IsTrue(constants.ContainsKey("maxWidth"));
            Assert.IsTrue(constants.ContainsKey("height"));
            Assert.IsTrue(constants.ContainsKey("minHeight"));
            Assert.IsTrue(constants.ContainsKey("maxHeight"));
            Assert.IsTrue(constants.ContainsKey("left"));
            Assert.IsTrue(constants.ContainsKey("top"));
            Assert.IsTrue(constants.ContainsKey("bottom"));
            Assert.IsTrue(constants.ContainsKey("right"));
            Assert.IsTrue(constants.ContainsKey("flex"));
            Assert.IsTrue(constants.ContainsKey("flexDirection"));
            Assert.IsTrue(constants.ContainsKey("flexWrap"));
            Assert.IsTrue(constants.ContainsKey("alignSelf"));
            Assert.IsTrue(constants.ContainsKey("alignItems"));
            Assert.IsTrue(constants.ContainsKey("justifyContent"));
            Assert.IsTrue(constants.ContainsKey("margin"));
            Assert.IsTrue(constants.ContainsKey("marginVertical"));
            Assert.IsTrue(constants.ContainsKey("marginHorizontal"));
            Assert.IsTrue(constants.ContainsKey("marginLeft"));
            Assert.IsTrue(constants.ContainsKey("marginRight"));
            Assert.IsTrue(constants.ContainsKey("marginTop"));
            Assert.IsTrue(constants.ContainsKey("marginBottom"));
            Assert.IsTrue(constants.ContainsKey("padding"));
            Assert.IsTrue(constants.ContainsKey("paddingVertical"));
            Assert.IsTrue(constants.ContainsKey("paddingHorizontal"));
            Assert.IsTrue(constants.ContainsKey("paddingLeft"));
            Assert.IsTrue(constants.ContainsKey("paddingRight"));
            Assert.IsTrue(constants.ContainsKey("paddingTop"));
            Assert.IsTrue(constants.ContainsKey("paddingBottom"));
            Assert.IsTrue(constants.ContainsKey("borderWidth"));
            Assert.IsTrue(constants.ContainsKey("borderLeftWidth"));
            Assert.IsTrue(constants.ContainsKey("borderRightWidth"));
            Assert.IsTrue(constants.ContainsKey("borderTopWidth"));
            Assert.IsTrue(constants.ContainsKey("borderBottomWidth"));
            Assert.IsTrue(constants.ContainsKey("position"));
            Assert.IsTrue(constants.ContainsKey("onLayout"));
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void ReactTextViewManagerTests_CreateView()
        {
            var viewManager = new ReactTextViewManager();

            var view = viewManager.CreateView(null);

            Assert.NotNull(view, "CreateViewInstance returned null");
            Assert.IsInstanceOf<TextBlock>(view, "CreateViewInstance returned unexpected type");
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void ReactTextViewManagerTests_CreateShadowNodeInstance()
        {
            var viewManager = new ReactTextViewManager();

            TextBlock textBlock = viewManager.CreateView(null);

            Assert.NotNull(textBlock, "CreateViewInstance returned null");

            var shadowNodeInstance = viewManager.CreateShadowNodeInstance();

            Assert.NotNull(shadowNodeInstance, "CreateShadowNodeInstance returned null");

            Assert.IsTrue(shadowNodeInstance.AlignContent == CSSAlign.FlexStart, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.AlignItems == CSSAlign.Stretch, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.AlignSelf == CSSAlign.Auto, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.ChildCount == 0, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.Direction == CSSDirection.Inherit, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(Math.Abs(shadowNodeInstance.Flex) < 0.1, "Default CSSNode Property has unexpected value"); ;
            Assert.IsTrue(shadowNodeInstance.FlexDirection == CSSFlexDirection.Column, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.HasNewLayout == false, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.HasUpdates	== true, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.Height), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.IsDirty == true, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.IsLayoutOnly == false, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.IsMeasureDefined == true, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.IsVirtual == false, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.IsVirtualAnchor == false, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.JustifyContent == CSSJustify.FlexStart, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.LayoutDirection == CSSDirection.LTR, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.LayoutHeight), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.LayoutWidth), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(Math.Abs(shadowNodeInstance.LayoutX) < 0.1, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(Math.Abs(shadowNodeInstance.LayoutY) < 0.1, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.MaxHeight), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.MaxWidth), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.MinHeight), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.MinWidth), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.NativeChildCount == 0, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.NativeParent == null, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.Overflow == CSSOverflow.Visible, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.Parent == null, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.PositionBottom), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.PositionLeft), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.PositionRight), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.PositionTop), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.PositionType == CSSPositionType.Relative, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.ReactTag == 0, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.ShouldNotifyOnLayout == false, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.TotalNativeChildren == 0, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.ViewClass == null, "Default CSSNode Property has unexpected value");
            Assert.IsTrue(float.IsNaN(shadowNodeInstance.Width), "Default CSSNode Property has unexpected value");
            Assert.IsTrue(shadowNodeInstance.Wrap == CSSWrap.NoWrap, "Default CSSNode Property has unexpected value");
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void ReactTextViewManagerTests_CreateViewInstanceEnsureDefaultValuesSet()
        {
            //Test that ViewManager.CreateViewInstance creates the object of the right type and that any explict default values are set....

            //The ViewManager creates TextBlocks with the following explicitly set default:
            //TextAlignment.Left
            //TextTrimming.CharacterEllipsis

            var viewManager = new ReactTextViewManager();

            var viewTuple = CreateViewInstance(viewManager, 1, null, null);

            var view = viewTuple.Item1;

            Assert.IsInstanceOf<TextBlock>(view);

            var textBlock = (TextBlock) view;

            Assert.AreEqual(TextAlignment.Left, textBlock.TextAlignment, "The default 'TextAlignment' property was overriden with an unexpected value...");
            Assert.AreEqual(TextTrimming.CharacterEllipsis, textBlock.TextTrimming, "The default 'TextTrimming' property was overriden with an unexpected value...");

            var referenceTextBlock = new TextBlock();

            var propertiesToIgnore = this.GetIgnoreDifferenceList();

            var compareResult = ReflectionUtilities.PublicInstancePropertiesEqual<TextBlock>(textBlock, referenceTextBlock, propertiesToIgnore);

            var propertyDifferencesList = compareResult.Item2;

            propertyDifferencesList.Remove("TextAlignment");
            propertyDifferencesList.Remove("TextTrimming");

            CollectionAssert.IsEmpty(propertyDifferencesList, "At least one unexpected property was overriden...");
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void ReactTextViewManagerTests_CreateViewInstanceWithNonShadowNodeProperties() //TODO: better name for test?
        {
            //Test that ViewManager.CreateViewInstance creates the object that has been initialized with 
            //the non Shadow Node settings...

            //color 
            //selectable
            //opacity
            //zIndex
            //accessibilityLabel
            //testID

            var viewManager = new ReactTextViewManager();

            var viewTag = this.GetNextTag();

            //Test 01
            //Test that specifying the following properties are set on the created TextBlock...

            var viewProps01 = new JObject
            {
                {"color", 0XFF424242 },
                {"opacity", 42f},
                {"zIndex", 42},
                {"accessibilityLabel", "42"},
                {"testID", 42}
            };

            var viewTuple = CreateViewInstance(viewManager, viewTag, null, viewProps01);

            var textBlock = (TextBlock)viewTuple.Item1;

            Assert.AreEqual("#FF424242", textBlock.Foreground.ToString(), "The 'Foreground' property was set to an unexpected value...");
            Assert.AreEqual(42f, textBlock.Opacity, 0.1, "The 'Opacity' property was set to an unexpected value...");
            Assert.AreEqual(42, Panel.GetZIndex(textBlock), "The 'ZIndex' property was set to an unexpected value...");
            Assert.AreEqual("42", textBlock.GetValue(AutomationProperties.AutomationIdProperty), "The 'AutomationProperties.AutomationId' property was set to an unexpected value...");
            //TODO: Need test for testID?

            //Test 02
            //Test that attempting to set the selectable property is not supported and should throw a NotImplementedException...

            var viewProps02 = new JObject
            {
                {"selectable", true},
            };

            var ex1 = Assert.Throws<NotImplementedException>(() => CreateViewInstance(viewManager, viewTag, null, viewProps02));
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void ReactTextViewManagerTests_CreateViewWithInitialStyleProperties()
        {
            var viewManager = new ReactTextViewManager();

            var viewTag = this.GetNextTag();

            ///////////////////////////////////////////////////////////////
            //FontWeight
            //
            //UWP FontWeight is a ushort...
            //WPF FontWeights is an enumeration 
            //         System.Windows.FontWeights.Thin (>= 0 && <=149)
            ///////////////////////////////////////////////////////////////

            var viewProps = new JObject
            {
                {ViewProps.FontSize, 42f},
                {ViewProps.FontFamily, "Comic Sans MS"},
                {ViewProps.FontWeight, 42}, //FontWeights.Thin
                {ViewProps.FontStyle, "Oblique"},
                {ViewProps.LetterSpacing, 2}, //FontStretches.ExtraCondensed
                {ViewProps.LineHeight, 42f},
                {ViewProps.NumberOfLines, 42f},
                {ViewProps.TextAlign, "center"},
                {ViewProps.Padding, 21f},
                {ViewProps.PaddingLeft, 42f},
                {ViewProps.PaddingTop, 43f},
                {ViewProps.PaddingRight, 44f},
                {ViewProps.PaddingBottom, 45f},
            };

            var viewTuple = CreateView(viewManager, viewTag, null, viewProps);

            var textBlock = (TextBlock)viewTuple.Item1;

            var referenceTextBlock = new TextBlock();

            var propertiesToIgnore = this.GetIgnoreDifferenceList();

            var compareResult = ReflectionUtilities.PublicInstancePropertiesEqual<TextBlock>(textBlock, referenceTextBlock, propertiesToIgnore);

            var propertyDifferencesList = compareResult.Item2;

            Assert.AreEqual(42f, textBlock.FontSize, 0.1, "The 'FontSize' property was set to an unexpected value...");
            Assert.AreEqual(new FontFamily("Comic Sans MS"), textBlock.FontFamily, "The 'FontFamily' property has an unexpected value...");
            Assert.AreEqual(FontWeights.Thin, textBlock.FontWeight, "The 'FontWeight' property has an unexpected value...");
            Assert.AreEqual(FontStyles.Oblique, textBlock.FontStyle, "The 'FontStyle' property hasan unexpected value...");
            Assert.AreEqual(FontStretches.ExtraCondensed, textBlock.FontStretch, "The 'FontStretch' property has an unexpected value...");
            Assert.AreEqual(42f, textBlock.LineHeight, "The 'LineHeight' property has an unexpected value...");

            //TODO: Why are the right and bottom padding not getting set... The code in ReactTextShadowNode explictly sets them to 0
            Assert.AreEqual(new Thickness(42f, 43f, 0f, 0f), textBlock.Padding, "The 'Padding' property has an unexpected value...");

            //Assert.AreEqual(new Thickness(42f, 43f, 44f, 45f), textBlock.Padding, "The 'Padding' property has an unexpected value...");
            Assert.AreEqual(TextAlignment.Center, textBlock.TextAlignment, "The 'TextAlignment' property has an unexpected value...");
            Assert.AreEqual(TextTrimming.CharacterEllipsis, textBlock.TextTrimming, "The 'TextTrimming' property has an unexpected value...");
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void ReactTextViewManagerTests_CreateViewWithLayoutProperties()
        {
            ReactNative.Bridge.DispatcherHelpers.CurrentDispatcher = Dispatcher.CurrentDispatcher;

            var viewManager = new ReactTextViewManager();

            var viewTag = this.GetNextTag();

            ///////////////////////////////////////////////////////////////
            //FontWeight
            //
            //UWP FontWeight is a ushort...
            //WPF FontWeights is an enumeration 
            //         System.Windows.FontWeights.Thin (>= 0 && <=149)
            ///////////////////////////////////////////////////////////////

            var viewProps = new JObject
            {
                {ViewProps.Width, 42f},
                {ViewProps.MinWidth, 21f},
                {ViewProps.MaxWidth, 84f},
                {ViewProps.Height, 42f},
                {ViewProps.MinHeight, 21f},
                {ViewProps.MaxHeight, 84f},
                {ViewProps.Left, 42f},
                {ViewProps.Top, 42f},
                {ViewProps.Right, 42f},
                {ViewProps.Bottom, 42f},
                {ViewProps.Flex, 1},
                {ViewProps.FlexDirection, "column"},
                {ViewProps.FlexWrap, "wrap"},
                {ViewProps.AlignSelf, "flex-end"},
                {ViewProps.AlignItems, "stretch"},
                {ViewProps.JustifyContent, "center"},
                {ViewProps.Margin, 42f},
                {ViewProps.MarginVertical, 42f},
                {ViewProps.MarginHorizontal, 42f},
                {ViewProps.MarginLeft, 42f},
                {ViewProps.MarginTop, 42f},
                {ViewProps.MarginRight, 42f},
                {ViewProps.MarginBottom, 42f},
                {ViewProps.BorderWidth, 42f},
                {ViewProps.BorderLeftWidth, 42f},
                {ViewProps.BorderTopWidth, 42f},
                {ViewProps.BorderRightWidth, 42f},
                {ViewProps.BorderBottomWidth, 42f},
                {ViewProps.Position , "absolute"},
            };

            var viewTuple = CreateView(viewManager, viewTag, null, viewProps);

            var textBlock = (TextBlock)viewTuple.Item1;
            var cssNode = viewTuple.Item2;

            var referenceTextBlock = new TextBlock();

            var propertiesToIgnore = this.GetIgnoreDifferenceList();

            var compareResult = ReflectionUtilities.PublicInstancePropertiesEqual<TextBlock>(textBlock, referenceTextBlock, propertiesToIgnore);

            var propertyDifferencesList = compareResult.Item2;

            propertyDifferencesList.Remove("TextTrimming");
            propertyDifferencesList.Remove("TextAlignment");
            
            if (propertyDifferencesList.Count > 0)
            {
                Assert.AreEqual(referenceTextBlock.FontSize, textBlock.FontSize, 0.1, "The 'FontSize' property was unexpectedly changed...");
                Assert.AreEqual(referenceTextBlock.FontFamily, textBlock.FontFamily, "The 'FontFamily' property was unexpectedly changed...");
                Assert.AreEqual(referenceTextBlock.FontWeight, textBlock.FontWeight, "The 'FontWeight' property was unexpectedly changed...");
                Assert.AreEqual(referenceTextBlock.FontStyle, textBlock.FontStyle, "The 'FontStyle' property was unexpectedly changed...");
                Assert.AreEqual(referenceTextBlock.FontStretch, textBlock.FontStretch, "The 'FontStretch' property was unexpectedly changed...");
                Assert.AreEqual(referenceTextBlock.LineHeight, textBlock.LineHeight, "The 'LineHeight' property was unexpectedly changed...");

                CollectionAssert.IsNotEmpty(propertyDifferencesList, "At least one property on the TextBlock was unexpectedly changed...");
            }
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void ReactTextViewManagerTests_CreateViewWithUpdateStyleProperties()
        {
            var viewManager = new ReactTextViewManager();

            var viewTag = this.GetNextTag();

            var initialViewProps = new JObject
            {
                {ViewProps.FontSize, 42f},
                {ViewProps.FontFamily, "Comic Sans MS"},
                {ViewProps.FontWeight, 42}, //FontWeights.Thin
                {ViewProps.FontStyle, "Oblique"},
                {ViewProps.LetterSpacing, 2}, //FontStretches.ExtraCondensed
                {ViewProps.LineHeight, 42f},
                {ViewProps.NumberOfLines, 42f},
                {ViewProps.TextAlign, "center"},
                {ViewProps.Padding, 21f},
                {ViewProps.PaddingLeft, 42f},
                {ViewProps.PaddingTop, 43f},
                {ViewProps.PaddingRight, 44f},
                {ViewProps.PaddingBottom, 45f},
            };

            var viewTuple = CreateView(viewManager, viewTag, null, initialViewProps);

            var textBlock = (TextBlock)viewTuple.Item1;

            Assert.AreEqual(42f, textBlock.FontSize, 0.1, "The 'FontSize' property was set to an unexpected value...");
            Assert.AreEqual(new FontFamily("Comic Sans MS"), textBlock.FontFamily, "The 'FontFamily' property has an unexpected value...");
            Assert.AreEqual(FontWeights.Thin, textBlock.FontWeight, "The 'FontWeight' property has an unexpected value...");
            Assert.AreEqual(FontStyles.Oblique, textBlock.FontStyle, "The 'FontStyle' property hasan unexpected value...");
            Assert.AreEqual(FontStretches.ExtraCondensed, textBlock.FontStretch, "The 'FontStretch' property has an unexpected value...");
            Assert.AreEqual(42f, textBlock.LineHeight, "The 'LineHeight' property has an unexpected value...");
            Assert.AreEqual(new Thickness(42f, 43f, 0f, 0f), textBlock.Padding, "The 'Padding' property has an unexpected value...");
            Assert.AreEqual(TextAlignment.Center, textBlock.TextAlignment, "The 'TextAlignment' property has an unexpected value...");
            Assert.AreEqual(TextTrimming.CharacterEllipsis, textBlock.TextTrimming, "The 'TextTrimming' property has an unexpected value...");

            ////////////////////////////////////////
            
            var updateViewProps01 = new JObject
            {
                {ViewProps.FontSize, 21f},
                {ViewProps.TextAlign, "right"},
            };

            this.UpdateView(viewManager, viewTuple, updateViewProps01);

            //Properties that changed...
            Assert.AreEqual(21f, textBlock.FontSize, 0.1, "The 'FontSize' property was set to an unexpected value...");
            Assert.AreEqual(TextAlignment.Right, textBlock.TextAlignment, "The 'TextAlignment' property has an unexpected value...");

            //Properties that shouldn't have changed...
            Assert.AreEqual(new FontFamily("Comic Sans MS"), textBlock.FontFamily, "The 'FontFamily' property has an unexpected value...");
            Assert.AreEqual(FontWeights.Thin, textBlock.FontWeight, "The 'FontWeight' property has an unexpected value...");
            Assert.AreEqual(FontStyles.Oblique, textBlock.FontStyle, "The 'FontStyle' property hasan unexpected value...");
            Assert.AreEqual(FontStretches.ExtraCondensed, textBlock.FontStretch, "The 'FontStretch' property has an unexpected value...");
            Assert.AreEqual(42f, textBlock.LineHeight, "The 'LineHeight' property has an unexpected value...");
            Assert.AreEqual(new Thickness(42f, 43f, 0f, 0f), textBlock.Padding, "The 'Padding' property has an unexpected value...");
            Assert.AreEqual(TextTrimming.CharacterEllipsis, textBlock.TextTrimming, "The 'TextTrimming' property has an unexpected value...");

            ////////////////////////////////////////

            var updateViewProps02 = new JObject
            {
                {ViewProps.FontSize, 84f},
                {ViewProps.FontFamily, "Arial"},
                {ViewProps.FontWeight, 600}, //FontWeights.SemiBold
                {ViewProps.FontStyle, "Italic"},
                {ViewProps.LetterSpacing, 9}, //FontStretches.UltraExpanded
                {ViewProps.LineHeight, 12f},
                {ViewProps.NumberOfLines, 42f},
                {ViewProps.TextAlign, "left"},
                {ViewProps.Padding, 18f},
                {ViewProps.PaddingLeft, 18f},
                {ViewProps.PaddingTop, 19f},
                {ViewProps.PaddingRight, 20f},
                {ViewProps.PaddingBottom, 21f},
            };
            
            this.UpdateView(viewManager, viewTuple, updateViewProps02);

            //Properties that changed...
            Assert.AreEqual(84f, textBlock.FontSize, 0.1, "The 'FontSize' property was set to an unexpected value...");
            Assert.AreEqual(new FontFamily("Arial"), textBlock.FontFamily, "The 'FontFamily' property has an unexpected value...");
            Assert.AreEqual(FontWeights.SemiBold, textBlock.FontWeight, "The 'FontWeight' property has an unexpected value...");
            Assert.AreEqual(FontStyles.Italic, textBlock.FontStyle, "The 'FontStyle' property hasan unexpected value...");
            Assert.AreEqual(FontStretches.UltraExpanded, textBlock.FontStretch, "The 'FontStretch' property has an unexpected value...");
            Assert.AreEqual(12f, textBlock.LineHeight, "The 'LineHeight' property has an unexpected value...");
            Assert.AreEqual(new Thickness(18f, 19f, 0f, 0f), textBlock.Padding, "The 'Padding' property has an unexpected value...");
            Assert.AreEqual(TextAlignment.Left, textBlock.TextAlignment, "The 'TextAlignment' property has an unexpected value...");
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void ReactTextViewManagerTests_CreateTextViewWithChild()
        {
            var textViewManager = new ReactTextViewManager();

            var textViewTag = this.GetNextTag();

            var textViewProps = new JObject
            {
                {ViewProps.FontSize, 15f},
            };

            var textViewTuple = CreateView(textViewManager, textViewTag, null, textViewProps);

            var textViewElement = (TextBlock)textViewTuple.Item1;
            var textViewCSSNode = textViewTuple.Item2;

            ///////////////////////////////////
            var rawTextViewManager = new ReactRunManager();

            var rawTextViewTag = this.GetNextTag();

            var rawTextToSet = @"Welcome to React Native";

            var rawTextViewProps = new JObject
            {
                {"text", rawTextToSet}
            };

            var rawTextViewTuple = CreateView(rawTextViewManager, textViewTag, textViewCSSNode, rawTextViewProps);

            var rawTextViewElement = rawTextViewTuple.Item1;
            var rawTextViewCSSNode = rawTextViewTuple.Item2;

            textViewManager.AddView(textViewElement, rawTextViewElement, 0);

            Assert.That(textViewElement.Inlines.Count, Is.EqualTo(1));
            Assert.IsInstanceOf<Run>(textViewElement.Inlines.FirstInline);

            var firstInline = (Run) textViewElement.Inlines.FirstInline;

            Assert.That(firstInline.Text, Is.EqualTo(rawTextToSet));
        }

        [Test]
        [Apartment(ApartmentState.STA)]
        public void ReactTextViewManagerTests_CreateViewWithRoot()
        {
            var viewViewManager = new ReactViewManager();

            var rootViewTag = this.GetNextTag();

            var rootViewProps = new JObject
            {
                {ViewProps.Flex, 1},
                {ViewProps.JustifyContent, "center"},
                {ViewProps.AlignItems, "center"},
                {ViewProps.BackgroundColor, 4294311167}
            };

            var rootViewTuple = CreateView(viewViewManager, rootViewTag, null, rootViewProps);

            var rootView = rootViewTuple.Item1;
            var rootCSSNode = rootViewTuple.Item2;

            Assert.IsInstanceOf<Border>(rootView);

            var textViewManager = new ReactTextViewManager();

            var textViewTag = this.GetNextTag();

            var textViewProps = new JObject
            {
                {ViewProps.FontSize, 15f},
                {ViewProps.Width, 32f},
                {ViewProps.Height, 32f},
            };

            var textViewTuple = CreateView(textViewManager, textViewTag, rootCSSNode, textViewProps);

            var textViewElement = (TextBlock)textViewTuple.Item1;
            var textViewCSSNode = textViewTuple.Item2;
        }

        #region Helper Methods

        private int _nextTag = 0;

        private int GetNextTag()
        {
            return Interlocked.Increment(ref _nextTag);
        }

        private Tuple<DependencyObject, ReactShadowNode> CreateViewInstance(IViewManager viewManager, int tag, ReactShadowNode rootNode, JObject props)
        {
            if (viewManager == null)
            {
                throw new ArgumentNullException(nameof(viewManager), "Test using null IViewManager...");
            }

            var cssNode = viewManager.CreateShadowNodeInstance();
            cssNode.ReactTag = tag;
            cssNode.ViewClass = viewManager.Name;
            cssNode.RootNode = rootNode;

            ReactStylesDiffMap reactStyles = null;

            if (props != null)
            {
                reactStyles = new ReactStylesDiffMap(props);
                cssNode.UpdateProperties(reactStyles);
            }

            var dependencyObject = viewManager.CreateView(null);

            if (reactStyles != null)
            {
                viewManager.UpdateProperties(dependencyObject, reactStyles);
            }

            return new Tuple<DependencyObject, ReactShadowNode>(dependencyObject, cssNode);
        }

        private Tuple<DependencyObject, ReactShadowNode> CreateView(IViewManager viewManager, int tag, ReactShadowNode rootNode, JObject props)
        {
            var viewTuple = CreateViewInstance(viewManager, tag, rootNode, props);

            var dependencyObject = viewTuple.Item1;
            var cssNode = viewTuple.Item2;

            viewManager.UpdateExtraData(dependencyObject, cssNode);

            return viewTuple;
        }

        private void UpdateView(IViewManager viewManager, Tuple<DependencyObject, ReactShadowNode> viewTuple, JObject props)
        {
            var reactStyles = new ReactStylesDiffMap(props);

            viewTuple.Item2.UpdateProperties(reactStyles);

            viewManager.UpdateExtraData(viewTuple.Item1, viewTuple.Item2);
        }

        private string[] GetIgnoreDifferenceList()
        {
            if (referenceIgnoreDifferencesList == null)
            {
                //Has to be run on STA
                var h1 = new TextBlock();
                var h2 = new TextBlock();

                var result = ReflectionUtilities.PublicInstancePropertiesEqual<TextBlock>(h1, h2);

                this.referenceIgnoreDifferencesList = result.Item2;

                this.referenceIgnoreDifferencesList.Add("IsMeasureValid");
                this.referenceIgnoreDifferencesList.Add("IsArrangeValid");
            }

            return this.referenceIgnoreDifferencesList.ToArray();
        }

        #endregion

        #region Mocks

        class MockEventDispatcher : IEventDispatcher
        {
            public void DispatchEvent(Event @event)
            {
                throw new NotImplementedException();
            }

            public void AddListener(IEventDispatcherListener listener)
            {
                throw new NotImplementedException();
            }

            public void RemoveListener(IEventDispatcherListener listener)
            {
                throw new NotImplementedException();
            }
        }

        #endregion
    }
}