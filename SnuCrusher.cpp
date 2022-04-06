#include "SnuCrusher.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SnuCrusher::SnuCrusher(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{

  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");
  GetParam(kBitDepth)->InitInt("BitDepth", 24, 0, 24, "Bits");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50), "SnuCrusher by Snudio Records", IText(50)));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-150), kGain));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-50), kBitDepth));
  };
#endif
}

#if IPLUG_DSP
void SnuCrusher::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const int bitdepth = GetParam(kBitDepth)->Value();
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      sample thisSample = inputs[c][s];
      int pot = pow(2, (bitdepth - 1));
      double val = floor((double)thisSample * pot) / pot;

      outputs[c][s] = val * gain;
    }
  }
}
#endif
