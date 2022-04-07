#include "SnuCrusher.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SnuCrusher::SnuCrusher(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{

  GetParam(kGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
  GetParam(kBitDepth)->InitInt("BitDepth", 24, 0, 24, "Bits");
  GetParam(kDownsample)->InitInt("Downsample", 0, 0, 24, "n");
  GetParam(kBitMask)->InitDouble("Bit Mask", 0., 0., 100.0, 0.01, "% Bits");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

  
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->LoadFont("Fontaudio", FONTAUDIO_FN);

    const IVStyle style{
      true, // Show label
      true, // Show value
      {
        DEFAULT_BGCOLOR,          // Background
        DEFAULT_FGCOLOR,          // Foreground
        DEFAULT_PRCOLOR,          // Pressed
        COLOR_BLACK,              // Frame
        DEFAULT_HLCOLOR,          // Highlight
        DEFAULT_SHCOLOR,          // Shadow
        COLOR_BLACK,              // Extra 1
        DEFAULT_X2COLOR,          // Extra 2
        DEFAULT_X3COLOR           // Extra 3
      },                          // Colors
      IText(12.f, EAlign::Center) // Label text
    };

    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetGridCell(0, 4, 6, iplug::igraphics::EDirection::Horizontal,4), "SnuCrusher by Snudio Records", IText(35)));
    pGraphics->AttachControl(new IVPeakAvgMeterControl<2>(b.GetGridCell(20, 4, 6, iplug::igraphics::EDirection::Vertical,3), "Meter", style.WithColor(kFG, COLOR_WHITE.WithOpacity(0.3f))),
      kCtrlTagPeakAvgMeter, "vcontrols");
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(12, 4, 6, iplug::igraphics::EDirection::Horizontal), kGain));
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(13, 4, 6, iplug::igraphics::EDirection::Horizontal), kBitDepth));
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(14, 4, 6, iplug::igraphics::EDirection::Horizontal), kDownsample));
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(15, 4, 6, iplug::igraphics::EDirection::Horizontal), kBitMask));

  };
#endif
}

#if IPLUG_DSP
void SnuCrusher::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const int bitdepth = (int)GetParam(kBitDepth)->Value();
  const int downsampl = (int)GetParam(kDownsample)->Value();
  const int nChans = NOutChansConnected();
  const int bitmask = (int)(GetParam(kBitMask)->Value() / 100 * 16777215);

  sample lastSample = 0.0;
  int sampleCounter = 0;
  for (int c = 0; c < nChans; c++)
  {
    for (int s = 0; s < nFrames; s++)
    {
      sample thisSample = inputs[c][s];
      if (sampleCounter < downsampl)
      {
        thisSample = lastSample;
        sampleCounter++;
      }
      else
      {
        sampleCounter = 0;
      }

      int pot = pow(2, (bitdepth - 1));

      lastSample = thisSample;
      double val = (double)thisSample;

      if (GetParam(kBitMask)->Value() > 1.0)
        val = (double)(((int)(val * 16777215) | bitmask) / 16777215);

      val = floor(val * pot) / pot;

      outputs[c][s] = val * gain;
    }
  }
  mPeakAvgMeterSender.ProcessBlock(outputs, nFrames, kCtrlTagPeakAvgMeter);
}

void SnuCrusher::OnReset()
{
  if (GetSampleRate() != mSampleRate)
  {
    mSampleRate = GetSampleRate();

    //mShifter.set_srate(mSampleRate);
  }
}

void SnuCrusher::OnIdle()
{

  mPeakAvgMeterSender.TransmitData(*this);
}
#endif
