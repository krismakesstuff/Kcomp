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
class KCompLAF  :    public juce::LookAndFeel_V4
{
public:
    
    juce::Colour baseColor = juce::Colours::purple;
    
    juce::Colour mainBGColor{ juce::Colours::purple.darker(0.9f) };

    juce::Colour controlsBGColor{ juce::Colours::darkslateblue.withAlpha(0.7f) };
    juce::Colour controls1Color{ juce::Colours::blue.withAlpha(0.7f) };
    juce::Colour controls2Color{ juce::Colours::darkgreen };

    juce::Colour spectrumColor{ juce::Colours::red.withAlpha(0.7f) };
    juce::Colour accent1Color{ juce::Colours::yellow.brighter(0.2f) };
    juce::Colour accent2Color{ juce::Colours::red.brighter(0.2f) };

    juce::Font mainFont{ "Unispace", 14.0f, juce::Font::FontStyleFlags::bold };
    juce::Font smallFont{ "Unispace", 11.5f, juce::Font::FontStyleFlags::plain };


    //FIX ME
    //Make this the Default Look and Feel in Editor Constructor

    KCompLAF()
    {
        //Slider Colors
        setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::aliceblue);
        setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::black);
        setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black);
        setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::black);
        setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::red.darker());

        //Button Colors
        setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);
        setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::yellow);
        
    }

    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& bgColor, bool isHighlighted, bool isButtonDown) override
    {
        auto area = button.getLocalBounds();
        
        auto flatOnLeft = button.isConnectedOnLeft();
        auto flatOnRight = button.isConnectedOnRight();
        auto flatOnTop = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();
        auto cornerSize = 6.0f;

        auto color = findColour(juce::TextButton::ColourIds::buttonColourId);
        auto onColor = findColour(juce::TextButton::ColourIds::buttonOnColourId);
        
        if (isButtonDown || isHighlighted)
        {
            color = isHighlighted ? color.contrasting(0.1f) : color.contrasting(0.5f);
            onColor = isHighlighted ? onColor.contrasting(0.1f) : onColor.contrasting(0.5f);
        }
        
        if (button.getToggleState())
        {
            
            //g.setColour(onColor);
            g.setGradientFill(juce::ColourGradient::vertical<int>(baseColor, onColor, area));
            
        }
        else
        {
            //g.setColour(color);
            g.setGradientFill(juce::ColourGradient::vertical<int>(baseColor, color, area));
        }


        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            juce::Path path;
            path.addRoundedRectangle(area.getX(), area.getY(),
                area.getWidth(), area.getHeight(),
                cornerSize, cornerSize,
                !(flatOnLeft || flatOnTop),
                !(flatOnRight || flatOnTop),
                !(flatOnLeft || flatOnBottom),
                !(flatOnRight || flatOnBottom));

            g.fillPath(path);
            g.strokePath(path, juce::PathStrokeType(1.0f));
        }
        else     //No Connected Edges
        {

            g.fillRoundedRectangle(area.toFloat(), cornerSize);
            g.drawRoundedRectangle(area.toFloat(), cornerSize, 1.0f);
        }

    }
    

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        

        auto radius = (float)juce::jmin(width / 2, height / 2) - 2.0f;
        auto centerX = (float)x + (float)width * 0.5f;
        auto centerY = (float)y + (float)height * 0.5f;
        auto rx = centerX - radius;
        auto ry = centerY - radius;
        auto rw = radius * 2.0f;
        
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();


        

        juce::ColourGradient rotGrade{ accent2Color, juce::Point<float>{centerX, centerY}, accent1Color, juce::Point<float>{rx, ry}, true };
        
        
        if (!isMouseOver)
        {
            
            rotGrade.setColour(0, rotGrade.getColour(0).darker(0.2f));
            rotGrade.setColour(1, rotGrade.getColour(1).brighter(0.2f));
            //rotGrade.addColour(0.3, juce::Colours::red);
        }
        
        //Make a rect with a gradeint fill circle that needle reveals
        //FIX ME

        g.setGradientFill(rotGrade);


        juce::Rectangle<float> mainRect(rx, ry, rw, rw);
        
        auto lineThickness = juce::jmin(15.0f, (float)juce::jmin(width, height) * 0.45f) * 0.1f;
        juce::Path mainCircle;
        mainCircle.addArc(rx, ry, rw, rw, rotaryStartAngle, rotaryStartAngle * 4, true);
        g.fillPath(mainCircle);
        

        g.setColour(juce::Colours::black);
        
        juce::Path p;
        auto pointerLength = radius * 0.5f;
        auto pointerThickness = 5.0f;
        p.addRectangle(-pointerThickness * 0.5f, - radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centerX, centerY));
        g.fillPath(p);

    }
        

private:

    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KCompLAF)
};
