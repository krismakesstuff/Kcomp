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
class LevelMeter  : public juce::Component,
                    public juce::Timer
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
            if (time - lastMeasurement < 100)
            {
                return;
            }

            lastMeasurement = time;
            for (size_t channel = 0; channel < meterData.size(); ++channel)
            {
                meterData[channel].setLevels(lastMeasurement, 0.0f, 0.0f, holdMS);
                meterData[channel].reduction = 1.0f;
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



        void setReductionLevel(const float newReduction)
        {
            for (auto& channel : meterData)
            {
                channel.reduction = newReduction;
            }
        }

        float getReductionLevel(const int channel) const
        {
            //FIX ME
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
        juce::int64 holdMS;

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
            //levelMeters.add(new juce::Rectangle<float>);
        }

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
        const auto infinity = -100.0f;
        float rmsDB;
        float peakDB;


        for (auto channel = 0; channel < source->meterData.size() ; ++channel)
        {
            g.setColour(meterColor);
            if (channel > 0)
            {
                g.setColour(meterColor.brighter());
            }
            

            juce::Rectangle<float> channelRect = area;

            auto meter = levelMeters[channel];
            
            //Divides the bounds by number of Channels to space them, then sets its postion
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

            rmsDB = juce::Decibels::gainToDecibels(source->getRMSLevel(channel), infinity);
            peakDB = juce::Decibels::gainToDecibels(source->getMaxLevel(channel), infinity);

            g.fillRect(meter->withTop(meter->getY() + rmsDB * meter->getHeight() / infinity));
            source->setMaxHoldMs(500);
            if (peakDB > -80)
            {
                g.drawHorizontalLine(juce::jmax<float>(peakDB * channelRect.getHeight() / infinity, 0.0f), meter->getX(), meter->getRight());
                source->decay();
            }
            
        }

        
    }

    void resized() override
    {
        auto area = getLocalBounds().toFloat();
        metersBackground.setBounds(area.getX(), area.getY(), area.getWidth(), area.getHeight());

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
            if (source)
            {
                source->resetUpdateMeter();
            }
            repaint();
        }
    }
    

private:

    juce::WeakReference<LevelMeterGetter> source;
    

    juce::Rectangle<float> metersBackground;

    juce::OwnedArray<juce::Rectangle<float>> levelMeters;

    /*juce::Rectangle<float> levelMeterRectL;
    juce::Rectangle<float> levelMeterRectR;*/

    bool bgNeedsRepaint = true;
    float meterHeight{};

    juce::Colour meterBGColor{ juce::Colours::black };
    juce::Colour meterColor{ juce::Colours::green };

    int refreshRate = 60;
    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
};
