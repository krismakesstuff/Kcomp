/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KcompAudioProcessorEditor::KcompAudioProcessorEditor(KcompAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts), levelMeter(p.getTotalNumInputChannels())
{
    getLookAndFeel().setDefaultLookAndFeel(&kCompLaf);

    getLookAndFeel().setDefaultSansSerifTypefaceName("Unispace");


    titleImage = juce::ImageFileFormat::loadFrom(juce::File(juce::File::getCurrentWorkingDirectory().getChildFile("Resources/Title.png")));
    mainBGImage = juce::ImageFileFormat::loadFrom(juce::File(juce::File::getCurrentWorkingDirectory().getChildFile("Resources/grid.png")));

    //Input 
    addAndMakeVisible(inputSlider);
    
    inputSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    inputSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 55, 20);
    inputSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    inputSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    inputGainAttachment.reset(new SliderAttachment(valueTreeState, inputGainParam_ID, inputSlider));
    inputSlider.onValueChange = [this] { audioProcessor.setInputGain(inputSlider.getValue()); };

    addAndMakeVisible(inputLabel);
    inputLabel.attachToComponent(&inputSlider, false);
    inputLabel.setJustificationType(juce::Justification::centred);

    //Threshold
    addAndMakeVisible(thresholdSlider);
    thresholdSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
    thresholdSliderAttachment.reset(new SliderAttachment(valueTreeState, thresholdParam_ID, thresholdSlider));
    thresholdSlider.onValueChange = [this] { audioProcessor.setThreshold(thresholdSlider.getValue()); };

    addAndMakeVisible(thresholdLabel);
    thresholdLabel.attachToComponent(&thresholdSlider, false);
    thresholdLabel.setJustificationType(juce::Justification::centred);

    //Make-Up Gain
    addAndMakeVisible(makeUpGainSlider);
    makeUpGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    makeUpGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
    makeUpGainAttachment.reset(new SliderAttachment(valueTreeState, makeUpGainParam_ID, makeUpGainSlider));
    makeUpGainSlider.onValueChange = [this] { audioProcessor.setMakeUpGain(makeUpGainSlider.getValue()); };

    addAndMakeVisible(makeUpGainLabel);
    makeUpGainLabel.attachToComponent(&makeUpGainSlider, false);
    makeUpGainLabel.setJustificationType(juce::Justification::centred);
    //makeUpGainLabel.setFont({ 15.0f, juce::Font::FontStyleFlags::plain });

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
    //ratioLabel.setFont({ 12.0f, juce::Font::FontStyleFlags::plain });

    
    //Attack
    addAndMakeVisible(attackSlider);
    attackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 55, 20);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    attackSliderAttachment.reset(new SliderAttachment(valueTreeState, attackParam_ID, attackSlider));
    attackSlider.onValueChange = [this] { audioProcessor.setAttack(attackSlider.getValue()); };

    addAndMakeVisible(attackLabel);
    attackLabel.attachToComponent(&attackSlider, false);
    attackLabel.setJustificationType(juce::Justification::centred);

    //Release
    addAndMakeVisible(releaseSlider);
    releaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    releaseSliderAttachment.reset(new SliderAttachment(valueTreeState, releaseParam_ID, releaseSlider));
    releaseSlider.onValueChange = [this] { audioProcessor.setRelease(releaseSlider.getValue()); };

    addAndMakeVisible(releaseLabel);
    releaseLabel.attachToComponent(&releaseSlider, false);
    releaseLabel.setJustificationType(juce::Justification::centred);

    //Tame
    addAndMakeVisible(tameButton);
    tameButton.setLookAndFeel(&kCompLaf);
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
    outputGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    outputGainSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    outputGainSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::transparentBlack);
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

    for (auto child = 0; child < getNumChildComponents(); ++child)
    {
        getChildComponent(child)->setLookAndFeel(nullptr);
    }

    //stopTimer();
}

//==============================================================================
void KcompAudioProcessorEditor::paint (juce::Graphics& g)
{
    //-----Bounds-----//
    
    //Left side of Rectangle
    auto compControlsBG = controlsBackground.withRight(thresholdSlider.getX());

    //Right side of Rectangle
    auto dryWetControlsBG = controlsBackground.withLeft(makeUpGainSlider.getRight());

    //Middle of Rectangle
    auto centerBG = controlsBackground.withLeft(compControlsBG.getRight() + 5 ).withRight(dryWetControlsBG.getX() - 5);
    auto leftCenterBG = centerBG.withRight(centerBG.getCentreX());
    auto rightCenterBG = centerBG.withLeft(centerBG.getCentreX());

    //Borders
    auto topBorder = controlsBackground.withBottom(controlsBackground.getY() + 200);
    auto bottomBorder = controlsBackground.withTop(controlsBackground.getBottom() - 200);


    //-----Fills-----//
    
    

    //Component Background
    g.drawImageWithin(mainBGImage, getX(), getY(), getWidth(), getHeight(), juce::RectanglePlacement::fillDestination);
    
    //Title image
    
    g.drawImage(titleImage, titleRect, juce::RectanglePlacement::centred);
    
    
    //Kcomp Rectangle

    //juce::DropShadow(juce::Colours::black, 1, {}).drawForRectangle(g, {controlsBackground.getX(), controlsBackground.getHeight()/2, controlsBackground.getWidth() , controlsBackground.getHeight() /2 });
    g.setColour(kCompLaf.controlsBGColor);
    g.drawRoundedRectangle(controlsBackground.toFloat().expanded(0.5f, 0.5f), 4.0f, 4.0f);
    

    //Left side of Rectangle
    juce::ColourGradient leftGrade = juce::ColourGradient::horizontal<int>(kCompLaf.controls1Color.darker(), kCompLaf.controls1Color, compControlsBG);
    //leftGrade.addColour(0.5f, kCompLaf.spectrumColor);
    g.setGradientFill(leftGrade);
    g.fillRoundedRectangle(compControlsBG.toFloat(), 4.0f);

    //Right side of Rectangle
    juce::ColourGradient rightGrade = juce::ColourGradient::horizontal<int>(kCompLaf.controls1Color, kCompLaf.controls1Color.darker(), dryWetControlsBG);
    g.setGradientFill(rightGrade);
    g.fillRoundedRectangle(dryWetControlsBG.toFloat(), 4.0f);
    
    //Middle of Rectangle
    
    /*juce::ColourGradient midGrade = juce::ColourGradient::horizontal<int>(kCompLaf.controlsBGColor.darker(), kCompLaf.controlsBGColor.darker(), centerBG);
    midGrade.addColour(0.25, kCompLaf.controlsBGColor);
    midGrade.addColour(0.5, kCompLaf.spectrumColor);
    midGrade.addColour(0.75, kCompLaf.controlsBGColor);
    g.setGradientFill(midGrade);*/

    juce::ColourGradient leftCGrade = juce::ColourGradient::horizontal<int>(kCompLaf.controlsBGColor.darker(), kCompLaf.spectrumColor, leftCenterBG);
    //leftCGrade.addColour(0.5 ,kCompLaf.controlsBGColor.brighter());
    g.setGradientFill(leftCGrade);
    g.fillRect(leftCenterBG);
    juce::ColourGradient rightCGrade = juce::ColourGradient::horizontal<int>(kCompLaf.spectrumColor, kCompLaf.controlsBGColor.darker(), rightCenterBG);
    //rightCGrade.addColour(0.5, kCompLaf.controlsBGColor.brighter());
    g.setGradientFill(rightCGrade);
    g.fillRect(rightCenterBG);

    //Borders
    /*g.setGradientFill(juce::ColourGradient::vertical<int>(kCompLaf.controls1Color.darker().withAlpha(0.5f), kCompLaf.controls1Color.brighter().withAlpha(0.0f), topBorder));
    g.fillRect(topBorder);
    g.setGradientFill(juce::ColourGradient::vertical<int>(kCompLaf.controls1Color.brighter().withAlpha(0.0f), kCompLaf.controls1Color.darker().withAlpha(0.5f), bottomBorder));
    g.fillRect(bottomBorder);*/

    
}

void KcompAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    titleRect.setBounds(0, 0, area.getWidth() /*- (area.getWidth() - 100)*/, 100);

    
    controlsBackground = area.reduced(10);
    controlsBackground.setLeft(area.getX() + 10);
    controlsBackground.setRight(area.getRight() - 10);
    controlsBackground.setTop(titleRect.getBottom() + 10);
    controlsBackground.setBottom(getBottom() - 45);

    //Center Section
    thresholdSlider.setBounds((controlsBackground.getWidth() / 4.5) + 30, controlsBackground.getY() + 40, controlsBackground.getWidth() / 7, controlsBackground.getHeight() - 57);
    levelMeter.setBounds(thresholdSlider.getRight() + 10, controlsBackground.getY() + 50, controlsBackground.getWidth()/5, controlsBackground.getHeight() - 55);
    makeUpGainSlider.setBounds(levelMeter.getRight() + 10, controlsBackground.getY() + 40, controlsBackground.getWidth() / 7, controlsBackground.getHeight() - 57);

    //preRMSLabel.setBounds(inputSlider.getRight() + 10, controlsBackground.getBottom() - 20, 50, 20);
    //postRMSLabel.setBounds(preRMSLabel.getRight() + 5, controlsBackground.getBottom() - 20, 50, 20);

    int ratioH = controlsBackground.getHeight() / 6;
    int ratioW = controlsBackground.getWidth() / 6;
    int space = (controlsBackground.getHeight() / 6);
    int leftIndent = 20;
    

    //Left side of Center Section
    inputSlider.setBounds(controlsBackground.getX() + leftIndent, controlsBackground.getY() + space , ratioW + 15, ratioH + 30);
    //tameButton.setBounds(controlsBackground.getX() + leftIndent + 30, controlsBackground.getY() + (space * 4) - 40, ratioW - 40, ratioH / 2);
    ratioLabel.setBounds(controlsBackground.getX() + leftIndent + 55, controlsBackground.getY() + (space * 3) - 20, (ratioW / 4) + 15, (ratioH / 4) + 5);
    ratio1Button.setBounds(controlsBackground.getX() + leftIndent + 10 , controlsBackground.getY() + (space * 3) + 5 , ratioW/4, ratioH/4 + 5);
    ratio2Button.setBounds(ratio1Button.getRight(), controlsBackground.getY() + (space * 3) + 5 , ratioW/4, (ratioH/4) + 5);
    ratio3Button.setBounds(ratio2Button.getRight(), controlsBackground.getY() + (space * 3) + 5, ratioW/4, (ratioH / 4) + 5);
    ratio4Button.setBounds(ratio3Button.getRight(), controlsBackground.getY() + (space * 3) + 5, ratioW/4, (ratioH / 4) + 5);

    
    attackSlider.setBounds(controlsBackground.getX() + 5, controlsBackground.getY() + (space * 5) - 45, ratioW - 45, ratioH);
    releaseSlider.setBounds(attackSlider.getRight() , controlsBackground.getY() + (space * 5) - 45, ratioW - 45, ratioH);


    //Right side of Center Section
    outputGainSlider.setBounds(makeUpGainSlider.getRight() + 50, controlsBackground.getY() + (controlsBackground.getHeight() / 6), (controlsBackground.getHeight()/5) + 20, (controlsBackground.getWidth() /6) + 20);

    tameButton.setBounds(makeUpGainSlider.getRight() + 55, outputGainSlider.getBottom() + 15, ratioW - 40, ratioH / 2);

    dryWetSlider.setBounds(makeUpGainSlider.getRight() + 72, controlsBackground.getY() + (controlsBackground.getHeight() /1.5), (controlsBackground.getHeight() / 5) - 25 , (controlsBackground.getWidth() / 6) - 25);
    dryLabel.setBounds(dryWetSlider.getX() -10 , dryWetSlider.getBottom() - 10, 40, 20);
    wetLabel.setBounds(dryWetSlider.getRight() - 25 , dryWetSlider.getBottom() - 10, 40, 20);


}

juce::Button& KcompAudioProcessorEditor::getActiveRatio()
{
    //DBG("Called");
    if (ratio1Button.getToggleState())
    {
        DBG(ratioOneParam_ID);
        return ratio1Button;
    }
    else if (ratio2Button.getToggleState())
    {
        DBG(ratioTwoParam_ID);
        return ratio2Button;
    }
    else if (ratio3Button.getToggleState())
    {
        DBG(ratioThreeParam_ID);
        return ratio3Button;
    }
    else if (ratio4Button.getToggleState())
    {
        DBG(ratioFourParam_ID);
        return ratio4Button;
    }
    else
    {
        DBG("ERROR");
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
    repaint();
}


//void KcompAudioProcessorEditor::timerCallback()
//{
//    
//    
//}

