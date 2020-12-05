/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    

    juce::NormalisableRange<float> inputGainRange = { juce::Decibels::decibelsToGain<float>(-60.0f), juce::Decibels::decibelsToGain<float>(4.0f), 0.0001f };
    float defInputGain = 1.0f;

    juce::NormalisableRange <float> makeUpGainRange = { juce::Decibels::decibelsToGain<float>(0.0f), juce::Decibels::decibelsToGain<float>(15.0f), 0.0001f };
    float defMakeUpGain = 1.0f;

    juce::NormalisableRange<float> thresholdRange = { juce::Decibels::decibelsToGain<float>(-90.0f),juce::Decibels::decibelsToGain<float>(0.0f), 0.000001f };
    thresholdRange.setSkewForCentre(juce::Decibels::decibelsToGain<float>(-45.0f));
    float defThreshold = 1.0f;

    juce::NormalisableRange<float> attackRange = { 0.0f, 2000.0f, 0.01f };
    float defAttack = attackRange.convertTo0to1(20.0f);

    juce::NormalisableRange<float> releaseRange = { 0.0f, 4000.0f, 0.01f };
    float defRelease = releaseRange.convertTo0to1(100.0f);

    juce::StringArray ratioStrings{"1.5", "5.0", "10.0", "20.0" };

    juce::NormalisableRange<float> outputGainRange = { juce::Decibels::decibelsToGain<float>(-60.0f), juce::Decibels::decibelsToGain<float>(4.0f), 0.0001f };
    float defOutputGain = 1.0f;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    
    layout.add(std::make_unique<juce::AudioParameterFloat>(inputGainParam_ID, "Input Gain", inputGainRange, defInputGain, juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) {return juce::String(juce::Decibels::gainToDecibels(value), 1) + " dB"; },
        [](juce::String text) {return juce::Decibels::decibelsToGain(text.dropLastCharacters(3).getFloatValue()); }));

    layout.add(std::make_unique<juce::AudioParameterFloat>(makeUpGainParam_ID, "MakeUp Gain", makeUpGainRange, defMakeUpGain, juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) {return juce::String(juce::Decibels::gainToDecibels(value), 1) + " dB"; },
        [](juce::String text) {return juce::Decibels::decibelsToGain(text.dropLastCharacters(3).getFloatValue()); }));

    layout.add(std::make_unique<juce::AudioParameterFloat>(thresholdParam_ID, "Threshold", thresholdRange, defThreshold, juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) {return juce::String(juce::Decibels::gainToDecibels(value), 1) + " dB"; },
        [](juce::String text) {return juce::Decibels::decibelsToGain(text.dropLastCharacters(3).getFloatValue()); }));

    layout.add(std::make_unique<juce::AudioParameterFloat>(attackParam_ID, "Attack", attackRange, defAttack));
    layout.add(std::make_unique<juce::AudioParameterFloat>(releaseParam_ID, "Release", releaseRange, defRelease));
    layout.add(std::make_unique<juce::AudioParameterFloat>(dryWetParam_ID, "Dry Wet Mix", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(filterParam_ID, "Filter", false));

  
    layout.add(std::make_unique<juce::AudioParameterBool>(ratioOneParam_ID, "Ratio 1", true));
    layout.add(std::make_unique<juce::AudioParameterBool>(ratioTwoParam_ID, "Ratio 2", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(ratioThreeParam_ID, "Ratio 3", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(ratioFourParam_ID, "Ratio 4", false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(outputGainParam_ID, "Output Gain", outputGainRange, defOutputGain, juce::String(), juce::AudioProcessorParameter::genericParameter,
        [](float value, int) {return juce::String(juce::Decibels::gainToDecibels(value), 1) + " dB"; },
        [](juce::String text) {return juce::Decibels::decibelsToGain(text.dropLastCharacters(3).getFloatValue()); }));
    
    return layout;
}


//==============================================================================
KcompAudioProcessor::KcompAudioProcessor()
     : AudioProcessor   (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                        ),
                        parameters(*this, nullptr, juce::Identifier("KcompParamTree"), createParameterLayout())
{
    
   /* inputGainParam = parameters.getRawParameterValue(inputGainParam_ID);
    makeUpGainParam = parameters.getRawParameterValue(makeUpGainParam_ID);
    ratioOneParam = parameters.getRawParameterValue(ratioOneParam_ID);
    ratioTwoParam = parameters.getRawParameterValue(ratioTwoParam_ID);
    ratioThreeParam = parameters.getRawParameterValue(ratioThreeParam_ID);
    ratioFourParam = parameters.getRawParameterValue(ratioFourParam_ID);
    thresholdParam = parameters.getRawParameterValue(thresholdParam_ID);
    attackParam = parameters.getRawParameterValue(attackParam_ID);
    releaseParam = parameters.getRawParameterValue(releaseParam_ID);
    dryWetParam = parameters.getRawParameterValue(dryWetParam_ID);
    filterParam = parameters.getRawParameterValue(filterParam_ID);*/

}

KcompAudioProcessor::~KcompAudioProcessor()
{
}

//==============================================================================
const juce::String KcompAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KcompAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool KcompAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool KcompAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double KcompAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KcompAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int KcompAudioProcessor::getCurrentProgram()
{
    return 0;
}

void KcompAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String KcompAudioProcessor::getProgramName (int index)
{
    return {};
}

void KcompAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void KcompAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;

    /*auto& inputGain = kComp.get<inputGain_ID>();*/
    inputGain.setGainLinear(*parameters.getRawParameterValue(inputGainParam_ID));

    auto& filter = kComp.get<filter_ID>();
    filter.state = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, filterFreq);

    auto& comp = kComp.get<compressor_ID>();
    comp.setAttack(*parameters.getRawParameterValue(attackParam_ID));
    comp.setRelease(*parameters.getRawParameterValue(releaseParam_ID));
    comp.setThreshold(*parameters.getRawParameterValue(thresholdParam_ID));

    float ratioOneVal = *parameters.getRawParameterValue(ratioOneParam_ID);
    float ratioTwoVal = *parameters.getRawParameterValue(ratioTwoParam_ID);
    float ratioThreeVal = *parameters.getRawParameterValue(ratioThreeParam_ID);
    float ratioFourVal = *parameters.getRawParameterValue(ratioFourParam_ID);    

    if (ratioOneVal > 0.5f)
    {
        comp.setRatio(ratioOne);
    }
    else if (ratioTwoVal > 0.5f)
    {
        comp.setRatio(ratioTwo);
    }
    else if (ratioThreeVal > 0.5f)
    {
        comp.setRatio(ratioThree);
    }
    else if (ratioFourVal > 0.5f)
    {
        comp.setRatio(ratioFour);
    }

    auto& makeUpGain = kComp.get<makeUpGain_ID>();
    makeUpGain.setGainLinear(*parameters.getRawParameterValue(makeUpGainParam_ID));

    outputGain.setGainLinear(*parameters.getRawParameterValue(outputGainParam_ID));

    kComp.prepare(spec);
    dryWet.prepare(spec);
    dryWet.setMixingRule(juce::dsp::DryWetMixingRule::squareRoot3dB);

    
    levelMeterGetter.resize(spec.numChannels, sampleRate / samplesPerBlock);

}

void KcompAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KcompAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void KcompAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (buffer.getNumSamples() < 1)
    {
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());
    }
    
    juce::dsp::AudioBlock<float> block(buffer);
    dryWet.pushDrySamples(block);
    juce::dsp::ProcessContextReplacing<float> context(block);


    /*preRMSL = buffer.getRMSLevel(0, buffer.getSample(0, 0), buffer.getNumSamples());
    preRMSR = buffer.getRMSLevel(1, buffer.getSample(1, 0), buffer.getNumSamples());*/

    inputGain.process(context);

    levelMeterGetter.loadMeterData(buffer);

    kComp.process(context);
    
    levelMeterGetter.setReductionLevel(buffer.getMagnitude(0, buffer.getNumSamples()));

   /* postRMSL = buffer.getRMSLevel(0, buffer.getSample(0, 0), buffer.getNumSamples());
    postRMSR = buffer.getRMSLevel(1, buffer.getSample(1, 0), buffer.getNumSamples());*/

    dryWet.mixWetSamples(context.getOutputBlock());
    dryWet.setWetMixProportion(dryWetMix);
    
    outputGain.process(context);

}


void KcompAudioProcessor::setFilterBypass(bool isFilterBypassed)
{
    kComp.setBypassed<filter_ID>(isFilterBypassed);
}

void KcompAudioProcessor::setInputGain(double newInputGain)
{
    /*auto& inputGain = kComp.get<inputGain_ID>();*/
    inputGain.setGainLinear((float)newInputGain);
}


void KcompAudioProcessor::setMakeUpGain(double newGain)
{
    auto& makeUp = kComp.get<makeUpGain_ID>();
    makeUp.setGainLinear((float)newGain);
}

void KcompAudioProcessor::setRatio(juce::String newRatioID)
{
    auto& ratio = kComp.get<compressor_ID>();
    ratio.setRatio(getRatioValue(newRatioID));
    DBG(newRatioID);
}


float KcompAudioProcessor::getRatioValue(juce::String ratioID)
{
    if (ratioID == ratioOneParam_ID)
    {
        return ratioOne;
    }
    else if (ratioID == ratioTwoParam_ID)
    {
        return ratioTwo;
    }
    else if (ratioID == ratioThreeParam_ID)
    {
        return ratioThree;
    }
    else if (ratioID == ratioFourParam_ID)
    {
        return ratioFour;
    }
    else return 1.0f;
}

void KcompAudioProcessor::setThreshold(double newThreshold)
{
    auto& comp = kComp.get<compressor_ID>();
    comp.setThreshold(juce::Decibels::gainToDecibels<float>(newThreshold));
    DBG("Threshold: " + juce::String(newThreshold));
}

void KcompAudioProcessor::setAttack(double newAttack)
{
    auto& comp = kComp.get<compressor_ID>();
    comp.setAttack(newAttack);
    DBG("Attack: " + juce::String(newAttack));
}

void KcompAudioProcessor::setRelease(double newRelease)
{
    auto& comp = kComp.get<compressor_ID>();
    comp.setRelease(newRelease);
    DBG("Release: " + juce::String(newRelease));
}


void KcompAudioProcessor::setDryWetMix(double newMix)
{
    if (newMix > 1)
    {
        dryWetMix = 1.0f;
    }
    else
    {
        dryWetMix = newMix;
    }

    DBG(juce::String(dryWetMix));
    
}

void KcompAudioProcessor::setOutputGain(double newOutGain)
{
    outputGain.setGainLinear((float)newOutGain);
}




float KcompAudioProcessor::getPreRMSLevel()
{
    return (preRMSL + preRMSR) / 2;
}

float KcompAudioProcessor::getPostRMSLevel()
{
    return (postRMSL + postRMSR) / 2;
}

LevelMeter::LevelMeterGetter* KcompAudioProcessor::getLevelMeterGetter()
{
    return &levelMeterGetter;
}

juce::NormalisableRange<float>* KcompAudioProcessor::getMinMax()
{
    return &minMax;
}

//==============================================================================
bool KcompAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* KcompAudioProcessor::createEditor()
{
    return new KcompAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void KcompAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
    
}

void KcompAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType())) 
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    } 

    DBG(parameters.state.toXmlString());
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KcompAudioProcessor();
}
