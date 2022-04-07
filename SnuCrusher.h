#pragma once

#define WDL_SIMPLEPITCHSHIFT_IMPLEMENT

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kBitDepth,
  kDownsample,
  kBitMask,
  kNumParams
};

enum EControlTags
{
  kCtrlTagPeakAvgMeter = 0,
  kCtrlTags
};

using namespace iplug;
using namespace igraphics;

class SnuCrusher final : public Plugin
{
public:
  SnuCrusher(const InstanceInfo& info);

private:

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnIdle() override;
#endif

  double mSampleRate = 0.0;
  IPeakAvgSender<2> mPeakAvgMeterSender{-90.0, true, 10.0f, 5.0f, 100.0f, 1000.0f};
};
