/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KcompAudioProcessorEditor::KcompAudioProcessorEditor(KcompAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts)
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
    inputSlider.onValueChange = [this] { audioProcessor.setInputGain(inputSlider.getValue()); };

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
    makeUpGainSlider.onValueChange = [this] { audioProcessor.setMakeUpGain(makeUpGainSlider.getValue()); };

    addAndMakeVisible(makeUpGainLabel);
    makeUpGainLabel.attachToComponent(&makeUpGainSlider, false);
    makeUpGainLabel.setJustificationType(juce::Justification::centred);
    makeUpGainLabel.setFont({ 15.0f, juce::Font::FontStyleFlags::plain });

    //Ratio Buttons
    addAndMakeVisible(ratio1Button);
    ratio1Button.setButtonText(juce::String(audioProcessor.getRatioValue(ratioOneParam_ID)));
    ratio1ButtonAttachment.reset(new ButtonAttachment(valueTreeState, ratioOneParam_ID, ratio1Button));
    ratio1Button.onClick = [this] { updateRatioState(&ratio1Button, ratioOneParam_ID); };
    
    addAndMakeVisible(ratio2Button);
    ratio2Button.setButtonText(juce::String(audioProcessor.getRatioValue(ratioTwoParam_ID)));
    ratio2ButtonAttachment.reset(new ButtonAttachment(valueTreeState, ratioTwoParam_ID, ratio2Button));
    ratio2Button.onClick = [this] { updateRatioState(&ratio2Button, ratioTwoParam_ID); };
    
    addAndMakeVisible(ratio3Button);
    ratio3Button.setButtonText(juce::String(audioProcessor.getRatioValue(ratioThreeParam_ID)));
    ratio3ButtonAttachment.reset(new ButtonAttachment(valueTreeState, ratioThreeParam_ID, ratio3Button));
    ratio3Button.onClick = [this] { updateRatioState(&ratio3Button, ratioThreeParam_ID); };
    
    addAndMakeVisible(ratio4Button);
    ratio4Button.setButtonText(juce::String(audioProcessor.getRatioValue(ratioFourParam_ID)));
    ratio4ButtonAttachment.reset(new ButtonAttachment(valueTreeState, ratioFourParam_ID, ratio4Button));
    ratio4Button.onClick = [this] { updateRatioState(&ratio4Button, ratioFourParam_ID); };
    
    ratio1Button.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    ratio2Button.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft + juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    ratio3Button.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft + juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    ratio4Button.setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft);

    ratio1Button.setClickingTogglesState(true);
    ratio2Button.setClickingTogglesState(true);
    ratio3Button.setClickingTogglesState(true);
    ratio4Button.setClickingTogglesState(true);

    updateRatioState(&ratio1Button, ratioOneParam_ID);

    addAndMakeVisible(ratioLabel);
    ratioLabel.setJustificationType(juce::Justification::centred);

    //Threshold
    addAndMakeVisible(thresholdSlider);
    thresholdSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::black);
    thresholdSliderAttachment.reset(new SliderAttachment(valueTreeState, thresholdParam_ID, thresholdSlider));
    thresholdSlider.onValueChange = [this] { audioProcessor.setThreshold(thresholdSlider.getValue()); };

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
    attackSlider.onValueChange = [this] { audioProcessor.setAttack(attackSlider.getValue()); };

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
    releaseSlider.onValueChange = [this] { audioProcessor.setRelease(releaseSlider.getValue()); };

    addAndMakeVisible(releaseLabel);
    releaseLabel.attachToComponent(&releaseSlider, false);
    releaseLabel.setJustificationType(juce::Justification::centred);

    //Tame
    addAndMakeVisible(tameButton);
    tameButton.setClickingTogglesState(true);
    tameButton.setToggleState(true, juce::dontSendNotification);
    tameButton.setButtonText("Tame");
    tameButtonAttachment.reset(new ButtonAttachment(valueTreeState, filterParam_ID, tameButton));
    tameButton.onClick = [this] { audioProcessor.setFilterBypass(tameButton.getToggleState()); };

    //DryWet
    addAndMakeVisible(dryWetSlider);
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    dryWetSliderAttachment.reset(new SliderAttachment(valueTreeState, dryWetParam_ID, dryWetSlider));
    dryWetSlider.onValueChange = [this] { audioProcessor.setDryWetMix(dryWetSlider.getValue()); };

    addAndMakeVisible(dryLabel);
    dryLabel.setFont({ 11.0f, juce::Font::FontStyleFlags::plain });
    addAndMakeVisible(wetLabel);
    wetLabel.setFont({ 11.0f, juce::Font::FontStyleFlags::plain });

    //RMS Labels
    addAndMakeVisible(preRMSLabel);
    addAndMakeVisible(postRMSLabel);

    //Level Meter
    addAndMakeVisible(levelMeter);
    levelMeter.setMeterSource(audioProcessor.getLevelMeterGetter());
    
    setSize (650, 400);
}

KcompAudioProcessorEditor::~KcompAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    //stopTimer();
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

    thresholdSlider.setBounds(inputSlider.getRight() + 10, controlsBackground.getY() + 50, 65, 200);

    levelMeter.setBounds(thresholdSlider.getRight() + 10, thresholdSlider.getY(), 45, 200);


    preRMSLabel.setBounds(inputSlider.getRight() + 10, controlsBackground.getBottom() - 20, 50, 20);
    postRMSLabel.setBounds(preRMSLabel.getRight() + 5, controlsBackground.getBottom() - 20, 50, 20);

    ratioLabel.setBounds(controlsBackground.getRight() - 112, controlsBackground.getY() + 10, 40, 25);
    ratio1Button.setBounds(controlsBackground.getRight() - 170, controlsBackground.getY() + 40, 40, 25);
    ratio2Button.setBounds(ratio1Button.getRight(), controlsBackground.getY() + 40, 40, 25);
    ratio3Button.setBounds(ratio2Button.getRight(), controlsBackground.getY() + 40, 40, 25);
    ratio4Button.setBounds(ratio3Button.getRight(), controlsBackground.getY() + 40, 40, 25);

    tameButton.setBounds(controlsBackground.getRight() - 140, controlsBackground.getBottom() - 230, 100, 30);

    attackSlider.setBounds(controlsBackground.getRight() - 170, controlsBackground.getBottom() - 175, 160, 40);

    releaseSlider.setBounds(controlsBackground.getRight() - 170, controlsBackground.getBottom() - 110, 160, 40);

    dryWetSlider.setBounds(controlsBackground.getRight()/3, controlsBackground.getBottom() - 55, 265, 35);
    dryLabel.setBounds(dryWetSlider.getX() - 15, dryWetSlider.getBottom() - 10, 40, 20);
    wetLabel.setBounds(dryWetSlider.getRight() - 15 , dryWetSlider.getBottom() - 10, 40, 20);


}

juce::String KcompAudioProcessorEditor::getActiveRatio()
{
    //DBG("Called");
    if (ratio1Button.getToggleState())
    {
        DBG(ratioOneParam_ID);
        return ratioOneParam_ID;
    }
    else if (ratio2Button.getToggleState())
    {
        DBG(ratioTwoParam_ID);
        return ratioTwoParam_ID;
    }
    else if (ratio3Button.getToggleState())
    {
        DBG(ratioThreeParam_ID);
        return ratioThreeParam_ID;
    }
    else if (ratio4Button.getToggleState())
    {
        DBG(ratioFourParam_ID);
        return ratioFourParam_ID;
    }
    else
    {
        DBG("ERROR");
        return "ERROR";
    }

    
}


void KcompAudioProcessorEditor::updateRatioState(juce::Button* activeButton, juce::String ratioID)
{
    ratio1Button.setToggleState(false, juce::dontSendNotification);
    ratio2Button.setToggleState(false, juce::dontSendNotification);
    ratio3Button.setToggleState(false, juce::dontSendNotification);
    ratio4Button.setToggleState(false, juce::dontSendNotification);


    activeButton->setToggleState(true,juce::dontSendNotification);
    audioProcessor.setRatio(ratioID);
}


//void KcompAudioProcessorEditor::timerCallback()
//{
//    
//    
//}

