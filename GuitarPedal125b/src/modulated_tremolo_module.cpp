#include "modulated_tremolo_module.h"

using namespace bkshepherd;

static const int s_paramCount = 5;
static const ParameterMetaData s_metaData[s_paramCount] = {{"Wave", -1, 0},
                                                           {"Depth", 1, 63},
                                                           {"Freq", 0, 63},
                                                           {"Osc Wave", -1, 0},
                                                           {"Osc Freq", 2, 0}};

// Default Constructor
ModulatedTremoloModule::ModulatedTremoloModule() : BaseEffectModule(),
                                                        m_tremoloFreqMin(1.0f),
                                                        m_tremoloFreqMax(20.0f),
                                                        m_freqOscFreqMin(0.01f),
                                                        m_freqOscFreqMax(1.0f),
                                                        m_cachedEffectMagnitudeValue(1.0f)
{
    // Set the name of the effect
    m_name = "Tremolo";

    // Initialize Parameters for this Effect
    this->InitParams(s_paramCount);
    m_paramMetaData = s_metaData;
}

// Destructor
ModulatedTremoloModule::~ModulatedTremoloModule()
{
    // No Code Needed
}

void ModulatedTremoloModule::Init(float sample_rate)
{
    BaseEffectModule::Init(sample_rate);

    m_tremolo.Init(sample_rate);
    m_freqOsc.Init(sample_rate);
}

float ModulatedTremoloModule::ProcessMono(float in)
{
    float value = BaseEffectModule::ProcessMono(in);

    // Calculate Tremolo Frequency Oscillation
    m_freqOsc.SetWaveform(GetParameter(3));
    m_freqOsc.SetAmp(1.0f);
    m_freqOsc.SetFreq(m_freqOscFreqMin + (GetParameterAsMagnitude(4) * m_freqOscFreqMax));
    float mod = m_freqOsc.Process();

    if (GetParameter(4) == 0) {
        mod = 1.0f;
    }

    // Calculate the effect
    m_tremolo.SetWaveform(GetParameter(0));
    m_tremolo.SetDepth(GetParameterAsMagnitude(1));
    m_tremolo.SetFreq(m_tremoloFreqMin + ((GetParameterAsMagnitude(2) * m_tremoloFreqMax) * mod));
    m_cachedEffectMagnitudeValue = m_tremolo.Process(1.0f);

    return value * m_cachedEffectMagnitudeValue;
}

float ModulatedTremoloModule::ProcessStereoLeft(float in)
{    
    return ProcessMono(in);
}

float ModulatedTremoloModule::ProcessStereoRight(float in)
{
    float value = BaseEffectModule::ProcessStereoRight(in);

    // Use the same magnitude as already calculated for the Left channel
    return value * m_cachedEffectMagnitudeValue;
}

float ModulatedTremoloModule::GetOutputLEDBrightness()
{    
    return BaseEffectModule::GetOutputLEDBrightness() * m_cachedEffectMagnitudeValue;
}
