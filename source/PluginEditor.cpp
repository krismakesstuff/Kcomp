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
    makeUpGainSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::darkgrey);
    makeUpGainSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::black);
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
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    dryWetSliderAttachment.reset(new SliderAttachment(valueTreeState, dryWetParam_ID, dryWetSlider));
    dryWetSlider.onValueChange = [this] { audioProcessor.setDryWetMix(dryWetSlider.getValue()); };

    addAndMakeVisible(dryLabel);
    dryLabel.setFont({ 11.0f, juce::Font::FontStyleFlags::plain });
    addAndMakeVisible(wetLabel);
    wetLabel.setFont({ 11.0f, juce::Font::FontStyleFlags::plain });

    //RMS Labels
    //addAndMakeVisible(preRMSLabel);
    //addAndMakeVisible(postRMSLabel);

    //Level Meter
    addAndMakeVisible(levelMeter);
    levelMeter.setMeterSource(audioProcessor.getLevelMeterGetter());
    
    //Output Gain 
    addAndMakeVisible(outputGainSlider);
    outputGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    outputGainSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black);
    outputGainSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::darkgrey);
    outputGainSliderAttachment.reset(new SliderAttachment(valueTreeState, outputGainParam_ID, outputGainSlider));
    outputGainSlider.onValueChange = [this] { audioProcessor.setOutputGain(outputGainSlider.getValue()); };

    addAndMakeVisible(outputGainLabel);
    outputGainLabel.attachToComponent(&outputGainSlider, false);
    outputGainLabel.setJustificationType(juce::Justification::centred);
    outputGainLabel.setFont({ 15.0f, juce::Font::FontStyleFlags::plain });


    setResizable(true, true);
    setResizeLimits(560, 400, 1260, 900);
    setSize (840, 600);
}

KcompAudioProcessorEditor::~KcompAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    //stopTimer();
}

//==============================================================================
void KcompAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    g.fillAll(juce::Colours::grey);

    g.setColour(juce::Colours::darkgrey);
    g.fillRect(controlsBackground);
    g.setColour(juce::Colours::white);
    g.drawRect(controlsBackground, 1.0f);

    
    
}

void KcompAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    kCompTitle.setBounds((getWidth() / 2) - 50, 10, area.getWidth() - (area.getWidth() - 100), 60);

    inputSlider.setBounds(area.getX() + 10, kCompTitle.getBottom() + 50, area.getWidth() - (area.getWidth() - 65), area.getHeight() - 250);

    outputGainSlider.setBounds(area.getRight() -  75, kCompTitle.getBottom() + 50, area.getWidth() - (area.getWidth() - 65), area.getHeight() - 250);

    controlsBackground = area.reduced(10);
    controlsBackground.setLeft(inputSlider.getRight());
    controlsBackground.setRight(outputGainSlider.getX());
    controlsBackground.setTop(kCompTitle.getBottom() + 10);
    controlsBackground.setBottom(getBottom() - 45);


    thresholdSlider.setBounds((controlsBackground.getWidth() / 3) + 20, controlsBackground.getY() + 50, controlsBackground.getWidth() / 7, controlsBackground.getHeight() - 100);

    levelMeter.setBounds(thresholdSlider.getRight() + 10, controlsBackground.getY() + 50, controlsBackground.getWidth()/5, controlsBackground.getHeight() - 100);

    makeUpGainSlider.setBounds(levelMeter.getRight() + 10, controlsBackground.getY() + 50, controlsBackground.getWidth() / 7, controlsBackground.getHeight() - 100);


    //preRMSLabel.setBounds(inputSlider.getRight() + 10, controlsBackground.getBottom() - 20, 50, 20);
    //postRMSLabel.setBounds(preRMSLabel.getRight() + 5, controlsBackground.getBottom() - 20, 50, 20);
    int ratioH = controlsBackground.getHeight() / 15;
    int ratioW = (controlsBackground.getWidth() /5) - 15;
    int space = controlsBackground.getBottom() - (controlsBackground.getHeight() / 4);
    

    ratioLabel.setBounds(controlsBackground.getX() + 20,  - 175, 40, 25);
    ratio1Button.setBounds(controlsBackground.getX() + 20, space - 145, ratioW/4, ratioH);
    ratio2Button.setBounds(ratio1Button.getRight(), space - 145, ratioW/4, ratioH);
    ratio3Button.setBounds(ratio2Button.getRight(), space - 145, ratioW/4, ratioH);
    ratio4Button.setBounds(ratio3Button.getRight(), space - 145, ratioW/4, ratioH);

    tameButton.setBounds(controlsBackground.getX() + 40, space - 110, ratioW - 40, ratioH);

    attackSlider.setBounds(controlsBackground.getX() + 20, space - 60, ratioW, ratioH);

    releaseSlider.setBounds(controlsBackground.getX() + 20, space, ratioW, ratioH);

    dryWetSlider.setBounds(levelMeter.getRight() + 100, controlsBackground.getY() + ((controlsBackground.getHeight() /2) - 30), controlsBackground.getWidth() / 5, controlsBackground.getHeight()/7);
    dryLabel.setBounds(dryWetSlider.getX() , dryWetSlider.getBottom() - 10, 40, 20);
    wetLabel.setBounds(dryWetSlider.getRight() - 40 , dryWetSlider.getBottom() - 10, 40, 20);


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

