/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LevelMeter.h"
//==============================================================================
/**
*/
const juce::String inputGainParam_ID = "inputGain";
const juce::String makeUpGainParam_ID = "makeUpGain";
const juce::String thresholdParam_ID = "threshold";
const juce::String attackParam_ID = "attack";
const juce::String releaseParam_ID = "release";
const juce::String filterParam_ID = "filter";
const juce::String dryWetParam_ID = "dryWet";
const juce::String ratioOneParam_ID = "ratioOne";
const juce::String ratioTwoParam_ID = "ratioTwo";
const juce::String ratioThreeParam_ID = "ratioThree";
const juce::String ratioFourParam_ID = "ratioFour";


class KcompAudioProcessor  : public juce::AudioProcessor
{
public:

    enum ChainIDs
    {
        inputGain_ID,
        filter_ID,
        compressor_ID,
        makeUpGain_ID,
        wetDry_ID
    };

    //==============================================================================
    KcompAudioProcessor();
    ~KcompAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;


    void setFilterBypass(bool);
    void setInputGain(double);
    void setMakeUpGain(double);
    void setRatio(juce::String);
    float getRatioValue(juce::String);
    
    void setThreshold(double);
    void setAttack(double);
    void setRelease(double);

    void setDryWetMix(double);
    
    float getPreRMSLevel();
    float getPostRMSLevel();

    LevelMeter::LevelMeterGetter* getLevelMeterGetter();
    
    juce::NormalisableRange<float>* getMinMax();
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    LevelMeter::LevelMeterGetter levelMeterGetter;

    juce::AudioProcessorValueTreeState parameters;

    using Gain = juce::dsp::Gain<float>;
    using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using Comp = juce::dsp::Compressor<float>;


    juce::dsp::ProcessorChain<Gain, Filter, Comp, Gain> kComp;
    juce::dsp::DryWetMixer<float> dryWet;

    /*std::atomic<float>* inputGainParam;
    std::atomic<float>* makeUpGainParam;
    std::atomic<float>* ratioOneParam;
    std::atomic<float>* ratioTwoParam;
    std::atomic<float>* ratioThreeParam;
    std::atomic<float>* ratioFourParam;
    std::atomic<float>* thresholdParam;
    std::atomic<float>* attackParam;
    std::atomic<float>* releaseParam;
    std::atomic<float>* dryWetParam;
    std::atomic<float>* filterParam;*/

    float ratioOne{ 1.5f };
    float ratioTwo{ 5.0f };
    float ratioThree{ 10.0f };
    float ratioFour{ 20.0f };

    float dryWetMix{ 1.0f };

    float filterFreq = 10000.0f;

    float preRMSL;
    float preRMSR;

    juce::NormalisableRange<float>minMax;

    float postRMSL;
    float postRMSR;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KcompAudioProcessor)
};
