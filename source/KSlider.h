/*
  ==============================================================================

    KSlider.h
    Created: 2 Dec 2020 2:30:28pm
    Author:  krisc

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class KSlider  :     public juce::LookAndFeel_V4
{
public:
    KSlider()
    {
        
        

    }

    ~KSlider() override
    {
    }

    //void paint (juce::Graphics& g) override
    //{
    //    g.setColour(trackColor);

    //    g.fillRoundedRectangle(track.toFloat(), 7.0f);
    //   
    //    juce::Path thumbPath;

    //    //drawLinearSliderThumb(g, 100, 100, 50, 50, 50, 0, 100, juce::Slider::SliderStyle::LinearBarVertical, *this);
    //    
    //}

    //void resized() override
    //{
    //    auto area = getLocalBounds();

    //    track.setBounds(area.getCentreX() , area.getY(), area.getWidth()/13, area.getHeight() - 10);

    //}

    void drawLinearSliderThumb(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        juce::Rectangle<int> thumb = { 100, 100, 50, 50 };

        g.setColour(juce::Colours::limegreen);
        g.fillRect(thumb);
    }

private:

    juce::Colour trackColor{ juce::Colours::black };
    juce::Rectangle<int> track;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KSlider)
};
