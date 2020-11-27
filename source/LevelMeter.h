/*
  ==============================================================================

    LevelMeter.h
    Created: 27 Nov 2020 11:04:23am
    Author:  krisc

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class LevelMeter  : public juce::Component
{
public:
    LevelMeter()
    {
        

    }

    ~LevelMeter() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        
        
        g.setColour(meterBGColor);
        g.fillRect(metersBackground);

        g.setColour(meterColor);
        g.fillRect(levelMeterRect);

    }

    void resized() override
    {
        auto area = getLocalBounds().toFloat();
        metersBackground.setBounds(area.getX(), area.getY(), area.getWidth(), area.getHeight());

        levelMeterRect.setTop(metersBackground.getBottom() - meterHeight);
        levelMeterRect.setBottom(metersBackground.getBottom());
        levelMeterRect.setLeft(metersBackground.getX());
        levelMeterRect.setRight(metersBackground.getRight());

    }

    void setMeterBGColor(juce::Colour newColour)
    {
        meterBGColor = newColour;
    }

    void setMeterColor(juce::Colour newColour)
    {
        meterColor = newColour; 
    }

    void setLevelMeter(float rms)
    {
        DBG("RMS: " + juce::String(rms));

        float newHeight = juce::jmap<float>(rms, 0, metersBackground.getHeight());
        
        DBG("New Height: " + juce::String(newHeight));
        meterHeight = newHeight;
        repaint();
        DBG("Level Meter: " + juce::String(levelMeterRect.getHeight()));
    }

private:

    
    juce::Rectangle<float> metersBackground;
    juce::Rectangle<float> levelMeterRect;

    float meterHeight{};

    juce::Colour meterBGColor{ juce::Colours::black };
    juce::Colour meterColor{ juce::Colours::green };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
};
