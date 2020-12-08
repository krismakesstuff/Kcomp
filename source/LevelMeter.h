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
const float infinity = -100.0f;

class LevelMeter  : public juce::Component,
                    public juce::Timer/*,
                    public juce::MouseListener*/
{
public:
    class LevelMeterGetter
    {
        class LevelMeterData
        {
        public:
            LevelMeterData(size_t rmsWindow = 8):
                max(),
                maxOverall(),
                clip(false),
                reduction(1.0f),
                hold(0),
                rmsHistory((size_t) rmsWindow, 0.0),
                rmsSum(0.0),
                rmsPtr(0)
            {}

            LevelMeterData(const LevelMeterData& other) :
                max(other.max.load()),
                maxOverall(other.maxOverall.load()),
                clip(other.clip.load()),
                reduction(other.reduction.load()),
                hold(other.hold.load()),
                rmsHistory(8, 0.0),
                rmsSum(0.0),
                rmsPtr(0)
            {}
            
            LevelMeterData& operator=(const LevelMeterData& other)
            {
                max.store(other.max.load());
                maxOverall.store(other.maxOverall.load());
                clip.store(other.clip.load());
                reduction.store(other.reduction.load());
                hold.store(other.hold.load());
                rmsHistory.resize(other.rmsHistory.size(), 0.0);
                rmsSum = 0.0;
                rmsPtr = 0;
                return (*this);
            }

            std::atomic<float> max;
            std::atomic<float> maxOverall;
            std::atomic<bool> clip;
            std::atomic<float> reduction;
            std::atomic<float> prevReduction;

            std::atomic<double> rmsSum;
            std::atomic<juce::int64> hold;
            std::vector<double> rmsHistory;
            size_t rmsPtr;


            float getAvgRMS() const
            {
                if (rmsHistory.size() > 0)
                {
                    return(std::sqrt(std::accumulate(rmsHistory.begin(), rmsHistory.end(), 0.0f) / static_cast<float>(rmsHistory.size())));
                }
                return float(std::sqrt(rmsSum));
            }

            
            void pushNextRMS(const float newRMS)
            {
                const double squaredRMS = std::min(newRMS * newRMS, 1.0f);
                if (rmsHistory.size() > 0)
                {
                    rmsHistory[(size_t)rmsPtr] = squaredRMS;
                    rmsPtr = (rmsPtr + 1) % rmsHistory.size();
                }
                else
                {
                    rmsSum = squaredRMS;
                }
            }


            void setLevels(const juce::int64 time, const float newMax, const float newRMS, const juce::int64 newHoldms)
            {
                if (newMax > 1.0 || newRMS > 1.0)
                {
                    clip = true;
                }

                maxOverall = fmaxf(maxOverall, newMax);

                if (newMax >= max)
                {
                    max = std::min(1.0f, newMax);
                    hold = time + newHoldms;
                }
                else if (time > hold)
                {
                    max = std::min(1.0f, newMax);
                }

                pushNextRMS(std::min(1.0f, newRMS));
            }

            void setRMSSize(const size_t numBlocks)
            {
                rmsHistory.assign(numBlocks, 0.0);
                rmsSum = 0.0;
                if (numBlocks > 1)
                {
                    rmsPtr %= rmsHistory.size();
                }
                else
                {
                    rmsPtr = 0;
                }
            }

        };
          //LevelMeterGetter Class Continues 
    public:

        ~LevelMeterGetter()
        {
            masterReference.clear();
        }

        void resize(const int channels, const int rmsWindow)
        {
            meterData.resize(size_t(channels), LevelMeterData(size_t(rmsWindow)));
            for (auto& d : meterData)
            {
                d.setRMSSize(size_t(rmsWindow));
            }
            updateMeter = true;
        }

        template<typename FloatType>
        void loadMeterData(const juce::AudioBuffer<FloatType>& buffer)
        {
            if (!suspended)
            {
                const int numChannels = buffer.getNumChannels();
                const int numSamples = buffer.getNumSamples();

                meterData.resize(size_t(numChannels));

                for (int channel = 0; channel < std::min(numChannels, int(meterData.size())); ++channel)
                {
                    meterData[size_t(channel)].setLevels(lastMeasurement,
                        buffer.getMagnitude(channel, 0, numSamples),
                        buffer.getRMSLevel(channel, 0, numSamples),
                        holdMS);
                }
            }
            updateMeter = true;
        }

        void decay()
        {
            juce::int64 time = juce::Time::currentTimeMillis();
            if (time - lastMeasurement < holdMS)
            {
                return;
            }

            lastMeasurement = time;
            for (size_t channel = 0; channel < meterData.size(); ++channel)
            {
                meterData[channel].setLevels(lastMeasurement, 0.0f, 0.0f, holdMS);
                meterData[0].reduction = meterData[0].reduction * 0.9f;
                meterData[1].reduction = meterData[1].reduction * 0.9f;
            }
            
            updateMeter = true;
        }

        bool shouldUpdateMeter() const
        {
            return updateMeter;
        }

        void resetUpdateMeter()
        {
            updateMeter = false;
        }

        void setSuspended(const bool shouldBeSus)
        {
            suspended = shouldBeSus;
        }

        void setMaxHoldMs(const juce::int64 millis)
        {
            holdMS = millis;
        }

        void setReductionLevel(const float newMag, int channel)
        {
            if (updateMeter)
            {
                meterData[channel].reduction = juce::jmap<float>(meterData[channel].max - newMag, 1.0f, 0.0f);
            }
            /*else
            {
                meterData[channel].reduction = 1.0f;
            }*/
            
        }

        float getReductionLevel(const int channel) const
        {
            if (updateMeter)
            {
                return meterData.at(size_t(channel)).reduction;
            }
            else
            {
                return 1.0f;
            }
        }

        float getPrevReduction(const int channel)
        {

            return meterData.at(size_t(channel)).prevReduction;

        }

        void setPrevReduction(const float newPrev, int channel)
        {
            meterData[channel].prevReduction = newPrev;
        }

        float getRMSLevel(const int channel) const
        {
            return meterData.at(size_t(channel)).getAvgRMS();
        }

        float getMaxLevel(const int channel) const
        {
            return meterData.at(size_t(channel)).max;
        }

        float getMaxOverallLevel(const int channel)
        {
            return meterData.at(size_t(channel)).maxOverall;
        }

        void clearMaxOveralls()
        {
            for (LevelMeterData& channel : meterData)
            {
                channel.maxOverall = infinity;
            }
        }

        bool getClipFlag(const int channel) const
        {
            return meterData.at(size_t(channel)).clip;
        }

        void clearAllClipFlags()
        {
            for (LevelMeterData& l : meterData)
            {
                l.clip = false;
            }
        }

        bool updateMeter{ true };
        bool suspended{ false };
        std::vector<LevelMeterData> meterData;
        std::atomic<juce::int64> lastMeasurement;
        juce::int64 holdMS{100};

        juce::WeakReference<LevelMeterGetter>::Master masterReference;
        friend class juce::WeakReference<LevelMeterGetter>;
    };


public:
    LevelMeter(int channels)
    {
        levelMeters.ensureStorageAllocated(channels);
        for (auto i = 0; i <= channels; i++)
        {
            levelMeters.set(i, new juce::Rectangle<float>, true);
        }

        //PeakLabels
        addAndMakeVisible(peakLLabel);
        peakLLabel.setText("0.00", juce::dontSendNotification);
        peakLLabel.setFont({ 10.0f, juce::Font::FontStyleFlags::plain });
        peakLLabel.setJustificationType(juce::Justification::centred);
        peakLLabel.setEditable(false);
        peakLLabel.setTooltip("Double-Click anywhere on the meter to reset.");

        addAndMakeVisible(peakRLabel);
        peakRLabel.setText("0.00", juce::dontSendNotification);
        peakRLabel.setFont({ 10.0f, juce::Font::FontStyleFlags::plain });
        peakRLabel.setJustificationType(juce::Justification::centred);
        peakRLabel.setEditable(false);
        peakRLabel.setTooltip("Double-Click anywhere on the meter to reset.");

        //Gain Reduction Labels
        addAndMakeVisible(grLLabel);
        grLLabel.setEditable(false);
        grLLabel.setFont({ 10.0f, juce::Font::FontStyleFlags::plain });
        grLLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(grRLabel);
        grRLabel.setEditable(false);
        grRLabel.setFont({ 10.0f, juce::Font::FontStyleFlags::plain });
        grRLabel.setJustificationType(juce::Justification::centred);
        

        startTimerHz(refreshRate);
        

    }

    ~LevelMeter() override
    {
        stopTimer();
    }

    void paint(juce::Graphics& g) override
    {
        juce::Graphics::ScopedSaveState saved(g);

        g.setColour(meterBGColor);
        g.fillRect(metersBackground);


        juce::Rectangle<float> area = getLocalBounds().toFloat();
        
        float rmsDB;
        float peakDB;
        


        for (auto channel = 0; channel < source->meterData.size() ; ++channel)
        {
            g.setColour(meterColor);
            juce::Rectangle<float> channelRect = { area.getX(), area.getY() + grLabelOffset, area.getWidth(), area.getHeight() - peakLabelOffset - grLabelOffset };

            //Divides the bounds by number of Channels to space them, then sets its postion
            auto meter = levelMeters[channel];
            meter->setBounds(channelRect.getX() + 2.0f, channelRect.getY(),
                (area.getWidth() / (levelMeters.size()-1)) * (channel + 1) - 4.0f,
                channelRect.getHeight());


            //sets the left of meters 
            auto prevMeterIn = levelMeters.indexOf(levelMeters[channel - 1]);
            if (prevMeterIn != -1)
            {
                auto prevMeter = levelMeters.getUnchecked(prevMeterIn);
                meter->setLeft(prevMeter->getRight() + 4);
            }


            //draws Level Meter
            rmsDB = juce::Decibels::gainToDecibels(source->getRMSLevel(channel), infinity);
            g.fillRect(meter->withTop(meter->getY() + rmsDB * meter->getHeight() / infinity));


            //draws Peak bar
            peakDB = juce::Decibels::gainToDecibels(source->getMaxLevel(channel), infinity);
            if (peakDB > -80)
            {
                g.drawHorizontalLine(juce::jmax<float>(meter->getY() + peakDB * meter->getHeight() / infinity, grLabelOffset), meter->getX(), meter->getRight());
            }
        
            //draws Clip Bar
            if (source->getClipFlag(channel))
            {
                g.setColour(juce::Colours::red.darker());
                g.fillRect(meter->getX(), channelRect.getY(), meter->getWidth(), 5.0f);
            }
            
        }
        
        source->decay();

        //draws Reduction Meter
        //auto reInfinity = infinity + 50.0f;
        //auto reductionL = juce::Decibels::gainToDecibels(source->getReductionLevel(0), reInfinity);
        //auto reductionR = juce::Decibels::gainToDecibels(source->getReductionLevel(1), reInfinity);
        ////prevReduction = juce::Decibels::gainToDecibels(source->getPrevReduction(channel), reInfinity);
        //g.setColour(juce::Colours::orange.withAlpha(0.8f));
        //g.setFont({ "Unispace", 10.0f, juce::Font::FontStyleFlags::plain });
        //juce::Rectangle<float> labelLRect = { area.getX(), area.getY(), 30.0f, 25.0f };
        //juce::Rectangle<float> labelRRect = { area.getX() + (area.getWidth() / 2), area.getY(), 30.0f, 25.0f };
        //if (reductionL < -1.0f || reductionR < -1.0f && refreshGR)
        //{
        //    /*juce::Rectangle<float> reductionMeter = meter->withWidth(meter->getWidth() / 2).withBottom(meter->getY() + prevReduction * meter->getHeight() / reInfinity);
        //    juce::Rectangle<float> reductionLevel = reductionMeter.withBottom(reductionMeter.getY() + reduction * reductionMeter.getHeight() / reInfinity);
        //    g.fillRect(reductionLevel);
        //    juce::Rectangle<float> labelRect = { reductionMeter.getX(), reductionLevel.getBottom(), 30.0f, 25.0f };*/
        //    
        //   
        //    g.drawText(grLLabel.getText(), labelLRect, juce::Justification::right);
        //    g.drawText(grRLabel.getText(), labelRRect, juce::Justification::right);
        //    
        //   // source->setPrevReduction(reduction, channel);
        //    refreshGR = true;
        //}
        //else if (refreshGR)
        //{
        //    
        //    g.drawText(grLLabel.getText(), labelLRect, juce::Justification::right);
        //    g.drawText(grRLabel.getText(), labelRRect, juce::Justification::right);

        //    refreshGR = true;
        //}

        
    }

    void resized() override
    {
        auto area = getLocalBounds().toFloat();
        metersBackground.setBounds(area.getX(), area.getY() + grLabelOffset, area.getWidth(), area.getHeight() - peakLabelOffset - grLabelOffset);

        peakLLabel.setBounds(metersBackground.getX() + 10, metersBackground.getBottom(), 60, 20);
        peakRLabel.setBounds((metersBackground.getWidth()/2) + 10, metersBackground.getBottom(), 60, 20);

        grLLabel.setBounds(metersBackground.getX() + 10, metersBackground.getY() - 20, 60, 20);
        grRLabel.setBounds(metersBackground.getX() + (metersBackground.getWidth() / 2) + 10, metersBackground.getY() - 20, 60, 20);
    }

    void setMeterSource(LevelMeterGetter* src)
    {
        source = src;
        repaint();
    }

    void setMeterBGColor(juce::Colour newColour)
    {
        meterBGColor = newColour;
    }

    void setMeterColor(juce::Colour newColour)
    {
        meterColor = newColour; 
    }

   

    void timerCallback() override
    {
        if ((source && source->shouldUpdateMeter()) || bgNeedsRepaint)
        {
            
            if (source->getClipFlag(0))
            {
                peakLLabel.setText("CLIP", juce::dontSendNotification);
            }
            if (source->getClipFlag(1))
            {
                peakRLabel.setText("CLIP", juce::dontSendNotification);
            }
            else
            {
                auto leftPeak = juce::Decibels::gainToDecibels(juce::jmin<float>(source->getMaxOverallLevel(0), 1.0f));
                auto rightPeak = juce::Decibels::gainToDecibels(juce::jmin<float>(source->getMaxOverallLevel(1), 1.0f));
                
                peakLLabel.setText(juce::String(leftPeak).dropLastCharacters(3) + " dB", juce::dontSendNotification);
                peakRLabel.setText(juce::String(rightPeak).dropLastCharacters(3) + " dB", juce::dontSendNotification);
                
            }

            auto leftGR = juce::Decibels::gainToDecibels(source->getReductionLevel(0));
            auto rightGR = juce::Decibels::gainToDecibels(source->getReductionLevel(1));

            if (leftGR < -1.0f || rightGR < -1.0f)
            {
                grLLabel.setText(juce::String(leftGR).dropLastCharacters(4).trimCharactersAtStart("-") + " dB", juce::dontSendNotification);
                grRLabel.setText(juce::String(rightGR).dropLastCharacters(4).trimCharactersAtStart("-") + " dB", juce::dontSendNotification);
            }
            else
            {
                grLLabel.setText("0.0 dB", juce::dontSendNotification);
                grRLabel.setText("0.0 dB", juce::dontSendNotification);
            }

            if (source)
            {
                source->resetUpdateMeter();
            }
            repaint();

            refreshGR = true;
        }
    }
    
    void mouseDoubleClick(const juce::MouseEvent& event) override
    {
        source->clearAllClipFlags();
        source->clearMaxOveralls();
    }


private:

    juce::WeakReference<LevelMeterGetter> source;
    

    juce::Rectangle<float> metersBackground;

    juce::OwnedArray<juce::Rectangle<float>> levelMeters;


    bool bgNeedsRepaint = true;
    float meterHeight{};

    juce::Colour meterBGColor{ juce::Colours::black };
    juce::Colour meterColor{ juce::Colours::lime };

    int refreshRate = 30;
    
    int peakLabelOffset = 25;
    int grLabelOffset = 25;
    juce::Label peakLLabel;
    juce::Label peakRLabel;

    juce::Label grLLabel;
    juce::Label grRLabel;
    bool refreshGR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
};
