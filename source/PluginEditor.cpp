/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KcompAudioProcessorEditor::KcompAudioProcessorEditor (KcompAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    setLookAndFeel(&kCompLAF);

    addAndMakeVisible(kCompTitle);
    kCompTitle.setJustificationType(juce::Justification::centred);

    //Input 
    addAndMakeVisible(inputSlider);
    inputSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    inputSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    inputSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black);
    inputSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::darkgrey);
    inputGainAttachment.reset(new SliderAttachment(valueTreeState, inputGainParam_ID, inputSlider));
    
    addAndMakeVisible(inputLabel);
    inputLabel.attachToComponent(&inputSlider, false);
    inputLabel.setJustificationType(juce::Justification::centred);

    //Make-Up Gain
    addAndMakeVisible(makeUpGainSlider);
    makeUpGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    makeUpGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    makeUpGainSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black);
    makeUpGainSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::darkgrey);
    makeUpGainAttachment.reset(new SliderAttachment(valueTreeState, makeUpGainParam_ID, makeUpGainSlider));

    addAndMakeVisible(makeUpGainLabel);
    makeUpGainLabel.attachToComponent(&makeUpGainSlider, false);
    makeUpGainLabel.setJustificationType(juce::Justification::centred);
    makeUpGainLabel.setFont({ 15.0f, 0 });

    //Ratio Buttons
    addAndMakeVisible(ratio1Button);
    ratio1Button.setClickingTogglesState(true);
    ratio1Button.setButtonText("1.5");
    ratio1ButtonAttachment.reset(new ButtonAttachment(valueTreeState, ratioOneParam_ID, ratio1Button));
    addAndMakeVisible(ratio2Button);
    ratio2Button.setClickingTogglesState(true);
    ratio2Button.setButtonText("3");
    ratio2ButtonAttachment.reset(new ButtonAttachment(valueTreeState, ratioTwoParam_ID, ratio2Button));
    addAndMakeVisible(ratio3Button);
    ratio3Button.setClickingTogglesState(true);
    ratio3Button.setButtonText("5");
    ratio3ButtonAttachment.reset(new ButtonAttachment(valueTreeState, ratioThreeParam_ID, ratio3Button));
    addAndMakeVisible(ratio4Button);
    ratio4Button.setClickingTogglesState(true);
    ratio4Button.setButtonText("10");
    ratio4ButtonAttachment.reset(new ButtonAttachment(valueTreeState, ratioFourParam_ID, ratio4Button));

    ratio1Button.setRadioGroupId(ratioButtonGroup, juce::dontSendNotification);
    ratio2Button.setRadioGroupId(ratioButtonGroup, juce::dontSendNotification);
    ratio3Button.setRadioGroupId(ratioButtonGroup, juce::dontSendNotification);
    ratio4Button.setRadioGroupId(ratioButtonGroup, juce::dontSendNotification);

    ratio1Button.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    ratio2Button.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft + juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    ratio3Button.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft + juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    ratio4Button.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft);

    addAndMakeVisible(ratioLabel);
    ratioLabel.setJustificationType(juce::Justification::centred);

    //Threshold
    addAndMakeVisible(thresholdSlider);
    thresholdSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::black);
    thresholdSliderAttachment.reset(new SliderAttachment(valueTreeState, thresholdParam_ID, thresholdSlider));

    addAndMakeVisible(thresholdLabel);
    thresholdLabel.attachToComponent(&thresholdSlider, false);
    thresholdLabel.setJustificationType(juce::Justification::centred);

    //Attack
    addAndMakeVisible(attackSlider);
    attackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    attackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, 50, 20);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::black);
    attackSliderAttachment.reset(new SliderAttachment(valueTreeState, attackParam_ID, attackSlider));

    addAndMakeVisible(attackLabel);
    attackLabel.attachToComponent(&attackSlider, false);
    attackLabel.setJustificationType(juce::Justification::centred);

    //Release
    addAndMakeVisible(releaseSlider);
    releaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    releaseSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, 50, 20);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::black);
    releaseSliderAttachment.reset(new SliderAttachment(valueTreeState, releaseParam_ID, releaseSlider));

    addAndMakeVisible(releaseLabel);
    releaseLabel.attachToComponent(&releaseSlider, false);
    releaseLabel.setJustificationType(juce::Justification::centred);

    //Tame
    addAndMakeVisible(tameButton);
    tameButton.setClickingTogglesState(true);
    tameButton.setButtonText("Tame");
    tameButtonAttachment.reset(new ButtonAttachment(valueTreeState, filterParam_ID, tameButton));


    setSize (650, 400);
}

KcompAudioProcessorEditor::~KcompAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void KcompAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::darkgrey);
    g.fillRect(controlsBackground);

}

void KcompAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    kCompTitle.setBounds((getWidth() / 2) - 50, 10, 100, 30);

    inputSlider.setBounds(10, kCompTitle.getBottom() + 50, 65, 250);

    makeUpGainSlider.setBounds(570, kCompTitle.getBottom() + 50, 65, 250);

    controlsBackground = area.reduced(10);
    controlsBackground.setLeft(inputSlider.getRight());
    controlsBackground.setRight(makeUpGainSlider.getX());
    controlsBackground.setTop(kCompTitle.getBottom() + 10);
    controlsBackground.setBottom(getBottom() - 45);

    thresholdSlider.setBounds(inputSlider.getRight() + 10, controlsBackground.getY() + 50, 65, 150);

    ratioLabel.setBounds(controlsBackground.getRight() - 112, controlsBackground.getY() + 20, 40, 25);
    ratio1Button.setBounds(controlsBackground.getRight() - 170, controlsBackground.getY() + 50, 40, 25);
    ratio2Button.setBounds(ratio1Button.getRight(), controlsBackground.getY() + 50, 40, 25);
    ratio3Button.setBounds(ratio2Button.getRight(), controlsBackground.getY() + 50, 40, 25);
    ratio4Button.setBounds(ratio3Button.getRight(), controlsBackground.getY() + 50, 40, 25);

    tameButton.setBounds(controlsBackground.getRight() - 140, controlsBackground.getBottom() - 190, 100, 30);

    attackSlider.setBounds(controlsBackground.getRight() - 170, controlsBackground.getBottom() - 125, 160, 50);

    releaseSlider.setBounds(controlsBackground.getRight() - 170, controlsBackground.getBottom() - 55, 160, 50);


}


