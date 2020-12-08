/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LevelMeter.h"
#include "KCompLAF.h"
#include "Klog.h"

//==============================================================================
/**
*/
class KcompAudioProcessorEditor  :  public juce::AudioProcessorEditor/*,
                                    public juce::Timer*/
{
public:

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    KcompAudioProcessorEditor(KcompAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
        
    ~KcompAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::Button& getActiveRatio();
    
    void updateRatioState(juce::Button*, juce::String);

    void showDebugger(bool shouldBeVisible);

    /*void timerCallback() override;*/

    

private:
    
    enum RadioButtonIds
    {
        ratioButtonGroup = 42
    };

    enum PresetsMenuIds
    {
        drums_ID,
        bass_ID,
        vocals_ID,
        guitar_ID,
        keys_ID,
        user_ID
    };

    juce::TextButton debugModeButton;
    SafePointer<Klog> logger;

    juce::Rectangle<int> controlsBackground;

    juce::Image titleImage;
    juce::Rectangle<float> titleRect;

    juce::Image mainBGImage;
    juce::Rectangle<float> mainBGRect;
    
    juce::ComboBox presetsCombo;
    juce::StringArray presetsHeadingStrings{"Drums", "Bass", "Vocals", "Guitar", "Keys", "User"};
    juce::StringArray drumsSubMenuStrings{ "FAT", "Thin", "Distorted", "Peak Catcher" };
    juce::StringArray bassSubMenuStrings{ "Thick", "Toned Back", "Chunky", "Balanced" };
    juce::StringArray vocalsSubMenuStrings{ "Upfront", "Warm", "Clean", "ASMR" };
    juce::StringArray guitarSubMenuStrings{ "Attacked", "Crunchy", "Blown", "Mild" };
    juce::StringArray keysSubMenuStrings{ "Destroyed", "Just A Touch", "Soft" };
    juce::StringArray userSubMenuStrings{"...", "..", "."};
    juce::Array<juce::StringArray*> subMenuStrings{&drumsSubMenuStrings, &bassSubMenuStrings, &vocalsSubMenuStrings, &guitarSubMenuStrings, &keysSubMenuStrings, &userSubMenuStrings};
    juce::OwnedArray<juce::PopupMenu> subMenus;

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

    juce::Slider dryWetSlider;
    juce::Label dryWetLabel{ juce::String(), "Dry/Wet Mix" };
    juce::Label dryLabel{ juce::String(), "Dry" };
    juce::Label wetLabel{ juce::String(), "Wet" };
    std::unique_ptr<SliderAttachment> dryWetSliderAttachment;

    juce::Slider outputGainSlider;
    juce::Label outputGainLabel{ juce::String(), "Output" };
    std::unique_ptr<SliderAttachment> outputGainSliderAttachment;

    //juce::Label preRMSLabel{juce::String() ,"666"};
    //juce::Label postRMSLabel{juce::String(), "777"};
    
    LevelMeter levelMeter;
    juce::Label gainReductionLabel{ juce::String(), "GR" };
    juce::Label peakLabel{ juce::String(), "Peak" };

    juce::AudioProcessorValueTreeState& valueTreeState;
    KcompAudioProcessor& audioProcessor;

    KCompLAF kCompLaf;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KcompAudioProcessorEditor)
};
