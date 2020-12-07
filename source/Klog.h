/*
  ==============================================================================

    Klog.h
    Created: 7 Dec 2020 11:58:53am
    Author:  krisc

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Klog : public juce::DocumentWindow
{
public:

    Klog(const juce::String& name, juce::Colour bgColor, int buttonsNeeded) : juce::DocumentWindow(name, bgColor, buttonsNeeded)
    {
        juce::Rectangle<int> area(50, 50, 300, 400);

        juce::RectanglePlacement placement(juce::RectanglePlacement::xRight
            | juce::RectanglePlacement::yTop
            | juce::RectanglePlacement::doNotResize);

        auto result = placement.appliedTo(area, juce::Desktop::getInstance().getDisplays()
            .getMainDisplay().userArea.reduced(20));

        /*logger->*/setBounds(area);
        /*logger->*/setUsingNativeTitleBar(true);
        /*logger->*/setResizable(true, true);
        //logger->centreWithSize(300, 200);
        /*logger->*/setVisible(true);
        debugWindow.setFont({ "SansSerif", 13.0f, juce::Font::FontStyleFlags::plain });
        debugWindow.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::lightgrey);
        debugWindow.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::black.brighter(0.2f));
        debugWindow.setLineSpacing(1.2f);
        debugMode = true;
        debugWindow.setBounds(getLocalBounds());
        debugWindow.setMultiLine(true);
        debugWindow.setReadOnly(true);
        printDebug("*******************DEBUG MODE******************");
        printDebug("", "Time Stamp");
        printDebug("", "Machine Info");

        setContentOwned(&debugWindow, true);
        
    }

    void closeButtonPressed() override
    {
        delete this;
    }

    
    void setDebugMode(bool isActive)
    {
        //owner.setVisible(isActive);
    }

    bool getDebugMode()
    {
        return debugMode;
    }

    void printDebug(const juce::String& message, const juce::String& title = juce::String{})
    {
        if (debugMode)
        {
            if (title.isEmpty())
            {
                debugWindow.moveCaretToEndOfLine(false);
                debugWindow.insertTextAtCaret(message + juce::NewLine::getDefault());
            }
            else
            {
                debugWindow.moveCaretToEndOfLine(false);
                debugWindow.setFont({ "SansSerif", 14.0f, juce::Font::FontStyleFlags::bold });
                debugWindow.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::black);
                debugWindow.insertTextAtCaret(title + ":    ");

                
                debugWindow.setFont({ "SansSerif", 13.0f, juce::Font::FontStyleFlags::plain });
                debugWindow.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::black.brighter(0.2f));
                debugWindow.insertTextAtCaret(message + juce::NewLine::getDefault());
                
            }
        }
    }

private:

    juce::TextEditor debugWindow;
    bool debugMode{ false };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Klog)
};