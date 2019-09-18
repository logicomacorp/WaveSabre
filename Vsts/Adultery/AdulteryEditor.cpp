#include "AdulteryEditor.h"
#include "AdulteryVst.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

using namespace std;

AdulteryEditor::AdulteryEditor(AudioEffect *audioEffect, Adultery *adultery)
	: VstEditor(audioEffect, 580, 400, "ADULTERY")
{
	this->adultery = adultery;

	menu = nullptr;
}

AdulteryEditor::~AdulteryEditor()
{
}

void AdulteryEditor::Open()
{
	// We're going to group the available samples by the first character of their name. Some of this code
	//  assumes the samples are stored in alphabetical order for simpler indexing, but this appears to be
	//  a correct assumption, and this file never changes.
	map<string, vector<pair<string, int>>> options;
	pair<string, int> noSampleOption("No sample", -1);
	vector<pair<string, int>> defaultOptions;
	defaultOptions.push_back(noSampleOption);
	options.insert(pair<string, vector<pair<string, int>>>("", defaultOptions));
	optionNames.insert(pair<int, string>(noSampleOption.second, noSampleOption.first));

	// Read gm.dls file
	auto gmDls = GmDls::Load();

	// Seek to wave pool chunk's data
	auto ptr = gmDls + GmDls::WaveListOffset;

	// Walk wave pool entries
	for (int i = 0; i < GmDls::NumSamples; i++)
	{
		// Walk wave list
		auto waveListTag = *((unsigned int *)ptr); // Should be 'LIST'
		ptr += 4;
		auto waveListSize = *((unsigned int *)ptr);
		ptr += 4;

		// Walk wave entry
		auto wave = ptr;
		auto waveTag = *((unsigned int *)wave); // Should be 'wave'
		wave += 4;

		// Skip fmt chunk
		auto fmtChunkTag = *((unsigned int *)wave); // Should be 'fmt '
		wave += 4;
		auto fmtChunkSize = *((unsigned int *)wave);
		wave += 4;
		wave += fmtChunkSize;

		// Skip wsmp chunk
		auto wsmpChunkTag = *((unsigned int *)wave); // Should be 'wsmp'
		wave += 4;
		auto wsmpChunkSize = *((unsigned int *)wave);
		wave += 4;
		wave += wsmpChunkSize;

		// Skip data chunk
		auto dataChunkTag = *((unsigned int *)wave); // Should be 'data'
		wave += 4;
		auto dataChunkSize = *((unsigned int *)wave);
		wave += 4;
		wave += dataChunkSize;

		// Walk info list
		auto infoList = wave;
		auto infoListTag = *((unsigned int *)infoList); // Should be 'LIST'
		infoList += 4;
		auto infoListSize = *((unsigned int *)infoList);
		infoList += 4;

		// Walk info entry
		auto info = infoList;
		auto infoTag = *((unsigned int *)info); // Should be 'INFO'
		info += 4;

		// Skip copyright chunk
		auto icopChunkTag = *((unsigned int *)info); // Should be 'ICOP'
		info += 4;
		auto icopChunkSize = *((unsigned int *)info);
		info += 4;
		// This size appears to be the size minus null terminator, yet each entry has a null terminator
		//  anyways, so they all seem to end in 00 00. Not sure why.
		info += icopChunkSize + 1;

		// Read name (finally :D)
		auto nameChunkTag = *((unsigned int *)info); // Should be 'INAM'
		info += 4;
		auto nameChunkSize = *((unsigned int *)info);
		info += 4;

		// Insert name into appropriate group
		auto name = string((char *)info);
		auto groupKey = string(1, name[0]);
		options[groupKey].push_back(pair<string, int>(name, i));
		optionNames.insert(pair<int, string>(i, name));

		ptr += waveListSize;
	}

	delete[] gmDls;

	menu = addOptionMenu("LOAD SAMPLE");
	addSpacer();

	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();

	caption = addTextLabel(140, 40, 100, 20, "sample name");
	caption->setFontColor(kBlackCColor);
	caption->setTransparency(true);
	caption->setTextTransparency(true);
	caption->setStyle(kBoldFace);

	int editorParamBase = 1000;

	menu->setTag(editorParamBase);
	editorParamBase++;

	for (auto group : options)
	{
		if (group.first.empty())
		{
			for (auto option : group.second)
			{
				menu->addEntry(new CMenuItem(option.first.c_str()));
				editorParamBase++;
			}
		}
		else
		{
			auto subMenu = new COptionMenu(CRect(), this, editorParamBase);
			for (auto option : group.second)
			{
				subMenu->addEntry(new CMenuItem(option.first.c_str()));
				editorParamBase++;
			}
			menu->addEntry(subMenu, group.first.c_str());
		}
	}

	updateCaption();
	uncheckMenuItems();

	startNextRow();

	addKnob((VstInt32)Adultery::ParamIndices::SampleStart, "SAMPLE START");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::Reverse, "REVERSE");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::LoopMode, "LOOP MODE");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::LoopBoundaryMode, "LOOP BOUND");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::LoopStart, "LOOP START");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::LoopLength, "LOOP LENGTH");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::InterpolationMode, "INTERPOLATION");

	startNextRow();

	addKnob((VstInt32)Adultery::ParamIndices::AmpAttack, "ATTACK");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::AmpDecay, "DECAY");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::AmpSustain, "SUSTAIN");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::AmpRelease, "RELEASE");

	startNextRow();

	addKnob((VstInt32)Adultery::ParamIndices::FilterType, "FLT TYPE");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::FilterFreq, "FLT FREQ");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::FilterResonance, "FLT RES");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::FilterModAmt, "FLT MOD");

	startNextRow();

	addKnob((VstInt32)Adultery::ParamIndices::ModAttack, "M ATTACK");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::ModDecay, "M DECAY");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::ModSustain, "M SUSTAIN");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::ModRelease, "M RELEASE");

	startNextRow();

	addKnob((VstInt32)Adultery::ParamIndices::VoicesUnisono, "UNISONO");
	addKnob((VstInt32)Adultery::ParamIndices::VoicesDetune, "DETUNE");
	addKnob((VstInt32)Adultery::ParamIndices::VoicesPan, "PAN");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::CoarseTune, "COARSE TUNE");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::FineTune, "FINE TUNE");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::VoiceMode, "MODE");
	addKnob((VstInt32)Adultery::ParamIndices::SlideTime, "SLIDE");
	addSpacer();
	addKnob((VstInt32)Adultery::ParamIndices::Master, "MASTER");

	VstEditor::Open();
}

void AdulteryEditor::setParameter(VstInt32 index, float value)
{
	if (!frame) return;

	if (index >= 1000)
	{
		adultery->SetParam((int)Adultery::ParamIndices::SampleIndex, (float)(index - 1000 + (int)value - 1));
		updateCaption();
		uncheckMenuItems();
	}
	else
	{
		VstEditor::setParameter(index, value);
	}
}

void AdulteryEditor::updateCaption()
{
	auto optionIndex = (int)adultery->GetParam((int)Adultery::ParamIndices::SampleIndex) - 1;
	caption->setText(optionNames[optionIndex].c_str());
}

void AdulteryEditor::uncheckMenuItems()
{
	for (int i = 0; i < menu->getNbEntries(); i++)
	{
		auto entry = menu->getEntry(i);
		if (entry->isChecked())
			entry->setChecked(false);
	}
}

