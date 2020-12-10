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

    juce::Colour controlsBGColor{ juce::Colours::darkslateblue/*.withAlpha(0.9f)*/ };
    juce::Colour controls1Color{ juce::Colours::blue.withAlpha(0.7f).darker(0.4f) };
    juce::Colour controls2Color{ juce::Colours::white };

    juce::Colour spectrumColor{ juce::Colours::red.withAlpha(0.7f) };
    juce::Colour accent1Color{ juce::Colours::yellow };
    juce::Colour accent2Color{ juce::Colours::red.brighter(0.2f) };

    juce::Font mainFont{ "Unispace", 13.0f, juce::Font::FontStyleFlags::bold };
    juce::Font smallFont{ "Unispace", 10.0f, juce::Font::FontStyleFlags::plain };

    juce::Colour fontColor{ juce::Colours::white };


    //FIX ME
    //Make this the Default Look and Feel in Editor Constructor

    KCompLAF()
    {
        //Slider Colors
        setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::aliceblue);
        setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::yellow.withAlpha(0.7f));
        setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::darkgrey);
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
            color = isHighlighted ? color.contrasting(0.3f) : color.contrasting(0.5f);
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
        
        juce::Rectangle<float> mainRect(x, y, rw, rw);

        g.setGradientFill(rotGrade);

        juce::Path mainCircle;
        mainCircle.addArc(rx, ry, rw, rw, rotaryStartAngle, rotaryStartAngle * 4, true);
        g.fillPath(mainCircle);

       // g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.setColour(juce::Colours::black);
        

        //TODO
        //Make needle a cool Synthwave Triangle
        juce::Path p;
        auto pointerLength = radius * 0.7f;
        auto pointerThickness = 5.0f;
        p.addRectangle(-pointerThickness * 0.5f, - radius, pointerThickness, pointerLength);
        //p.addTriangle(-pointerThickness * 0.5f, -radius, radius * 0.5f , ry , radius * 0.8f, rx );
        //p.addTriangle({ centerX * 0.5f, centerY * 0.5f }, { rx, ry}, { centerX * 0.2f, centerY * 0.2f });
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centerX, centerY));
        g.fillPath(p);

    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style == juce::Slider::SliderStyle::LinearVertical)
        {
            auto trackWidth = width / 15.0f;
            juce::Rectangle<float> rect{ (width/2.0f) - (trackWidth/2), (float)y, trackWidth, (float)height };
            g.setColour(juce::Colours::black);
            g.fillRect(rect);

            drawLinearSliderThumb(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);

        }
        else
        {
            juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
        
    }

    void drawLinearSliderThumb(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        auto mouseOver = slider.isMouseOver();
        

        juce::Rectangle<float> rect{ (width/2.0f) - 25.0f , sliderPos - 10.0f, 50.0f, 20.0f };
        juce::ColourGradient grade = { juce::ColourGradient::vertical<float>(baseColor, findColour(juce::TextButton::ColourIds::buttonColourId), rect) };


        if (mouseOver)
        {
            grade.setColour(0, grade.getColour(0).brighter());

            if (slider.isMouseButtonDown())
            {
                grade.setColour(0, accent1Color);
            }
        }
        
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(rect, 2.0f, 1.0f);
        g.setGradientFill(grade);
        g.fillRect(rect);

        g.setColour(juce::Colours::white);
        g.setFont(smallFont);
        juce::String valueString = juce::String(juce::Decibels::gainToDecibels<float>(slider.getValue())).dropLastCharacters(3) << " dB";

        g.drawText(valueString, rect, juce::Justification::centred);
        
        
    }

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {

        auto bounds = label.getLocalBounds().toFloat();
        auto relativeBounds = label.getBoundsInParent();

        /*juce::ColourGradient grade;


        grade = juce::ColourGradient::vertical<float>(baseColor.withAlpha(0.1f), baseColor, bounds);
        grade.multiplyOpacity(0.8f);
        
        auto bg = bounds.reduced(1);

        if (!(findColour(juce::Label::ColourIds::backgroundColourId) == juce::Colours::transparentBlack))
        {
            g.setGradientFill(grade);
            g.fillRoundedRectangle(bg.toFloat(), 1.0f);
        }*/
       
        g.setFont(label.getFont());
        g.setColour(fontColor);
        //g.drawText(label.getText(true), bounds, juce::Justification::centred);

        g.drawFittedText(label.getText(true), bounds.toNearestInt().reduced(2), juce::Justification::centred,1,0.0f);

    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override
    {


        auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        juce::Rectangle<int> boxBounds(0, 0, width, height);
        

        g.setGradientFill(juce::ColourGradient::vertical<int>(baseColor, accent2Color, boxBounds));
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

        juce::Rectangle<int> arrowZone(width - 30, 0, 20, height);
        juce::Path path;
        path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
        path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, juce::PathStrokeType(2.0f));
        

    }

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
        const bool isSeparator, const bool isActive,
        const bool isHighlighted, const bool isTicked,
        const bool hasSubMenu, const juce::String& text,
        const juce::String& shortcutKeyText,
        const juce::Drawable* icon, const juce::Colour* const textColourToUse) override
    {
        if (isSeparator)
        {
            auto r = area.reduced(5, 0);
            r.removeFromTop(juce::roundToInt(((float)r.getHeight() * 0.5f) - 0.5f));

            //g.setColour(findColour(juce::PopupMenu::textColourId).withAlpha(0.3f));
            g.setColour(spectrumColor);
            g.fillRect(r.removeFromTop(1));
        }
        else
        {
            auto textColour = (textColourToUse == nullptr ? findColour(juce::PopupMenu::textColourId)
                : *textColourToUse);

            auto r = area.reduced(1);

            if (isHighlighted && isActive)
            {
                //g.setColour(findColour(juce::PopupMenu::highlightedBackgroundColourId));
                g.setColour(accent2Color);
                g.fillRect(r);

                //g.setColour(findColour(juce::PopupMenu::highlightedTextColourId));
                g.setColour(controls1Color);
            }
            else
            {
                //g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
                g.setColour(controls2Color);
            }

            r.reduce(juce::jmin(5, area.getWidth() / 20), 0);

            auto font = getPopupMenuFont();

            auto maxFontHeight = (float)r.getHeight() / 1.3f;

            if (font.getHeight() > maxFontHeight)
                font.setHeight(maxFontHeight);

            g.setFont(font);

            auto iconArea = r.removeFromLeft(juce::roundToInt(maxFontHeight)).toFloat();

            if (icon != nullptr)
            {
                icon->drawWithin(g, iconArea, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize, 1.0f);
                r.removeFromLeft(juce::roundToInt(maxFontHeight * 0.5f));
            }
            else if (isTicked)
            {
                auto tick = getTickShape(1.0f);
                g.fillPath(tick, tick.getTransformToScaleToFit(iconArea.reduced(iconArea.getWidth() / 5, 0).toFloat(), true));
            }

            if (hasSubMenu)
            {
                auto arrowH = 0.6f * getPopupMenuFont().getAscent();

                auto x = static_cast<float> (r.removeFromRight((int)arrowH).getX());
                auto halfH = static_cast<float> (r.getCentreY());

                juce::Path path;
                path.startNewSubPath(x, halfH - arrowH * 0.5f);
                path.lineTo(x + arrowH * 0.6f, halfH);
                path.lineTo(x, halfH + arrowH * 0.5f);

                g.strokePath(path, juce::PathStrokeType(2.0f));
            }

            r.removeFromRight(3);
            g.drawFittedText(text, r, juce::Justification::centredLeft, 1);

            if (shortcutKeyText.isNotEmpty())
            {
                auto f2 = font;
                f2.setHeight(f2.getHeight() * 0.75f);
                f2.setHorizontalScale(0.95f);
                g.setFont(f2);

                g.drawText(shortcutKeyText, r, juce::Justification::centredRight, true);
            }
        }
    }

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        juce::ColourGradient grade = juce::ColourGradient::vertical<int>(baseColor, accent1Color, { width, height });
        juce::ColourGradient bGrade = juce::ColourGradient::vertical<int>(accent2Color, baseColor, { width, height });

        //g.fillAll(accent2Color);
        g.setGradientFill(bGrade);
        g.fillAll();
        juce::ignoreUnused(width, height);

        //g.setGradientFill(grade);
        g.setColour(juce::Colours::white);
        g.drawRect(0, 0, width, height);

    }

    void drawPopupMenuSectionHeader(juce::Graphics& g, const juce::Rectangle<int>& area, const juce::String& sectionName)
    {
        g.setFont(getPopupMenuFont().boldened());
        //g.setColour(findColour(juce::PopupMenu::headerTextColourId));
        g.setColour(baseColor);

        g.drawFittedText(sectionName,
            area.getX() + 12, area.getY(), area.getWidth() - 16, (int)((float)area.getHeight() * 0.8f),
            juce::Justification::bottomLeft, 1);
    }


private:

    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KCompLAF)
};
