/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class KcompAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    KcompAudioProcessorEditor(KcompAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
        
    ~KcompAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    

private:
    
    enum RadioButtonIds
    {
        ratioButtonGroup = 42
    };

    juce::Label kCompTitle{juce::String(), "KCOMP"};

    
    juce::Slider inputSlider; 
    juce::Label inputLabel{juce::String(), "Input"};
    std::unique_ptr<SliderAttachment> inputGainAttachment;

    juce::Slider makeUpGainSlider;
    juce::Label makeUpGainLabel{juce::String(), "Make-Up Gain"};
    std::unique_ptr <SliderAttachment> makeUpGainAttachment;

    juce::TextButton ratio1Button;
    juce::TextButton ratio2Button;
    juce::TextButton ratio3Button;
    juce::TextButton ratio4Button;
    juce::Label ratioLabel{juce::String(), "Ratio"};
    std::unique_ptr<ButtonAttachment> ratio1ButtonAttachment;
    std::unique_ptr<ButtonAttachment> ratio2ButtonAttachment;
    std::unique_ptr<ButtonAttachment> ratio3ButtonAttachment;
    std::unique_ptr<ButtonAttachment> ratio4ButtonAttachment;

    juce::Slider thresholdSlider;
    juce::Label thresholdLabel{ juce::String(), "Threshold" };
    std::unique_ptr <SliderAttachment> thresholdSliderAttachment;

    juce::Slider attackSlider;
    juce::Label attackLabel{ juce::String(), "Attack" };
    std::unique_ptr <SliderAttachment> attackSliderAttachment;

    juce::Slider releaseSlider;
    juce::Label releaseLabel{ juce::String(), "Release" };
    std::unique_ptr <SliderAttachment> releaseSliderAttachment;

    juce::TextButton tameButton;
    std::unique_ptr<ButtonAttachment> tameButtonAttachment;


    juce::Rectangle<int> controlsBackground;
    juce::Rectangle<int> metersBackground;

    juce::AudioProcessorValueTreeState& valueTreeState;
    KcompAudioProcessor& audioProcessor;


    struct KCompLookAndFeel : public juce::LookAndFeel_V4
    {
        /*void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle,
                                juce::Slider& slider) override
        {

            

        }*/
        
    };
    KCompLookAndFeel kCompLAF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KcompAudioProcessorEditor)
};
