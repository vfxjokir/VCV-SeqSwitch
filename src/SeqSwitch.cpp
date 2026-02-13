//
//  SeqSwitch.cpp
//  SeqSwitch
//
//  Created by Josh Kirschenbaum on 1/24/26.
//

#include <stdio.h>
#include "plugin.hpp"
#include <rack.hpp> // Add this directly to be safe


using namespace rack; // This tells Xcode to look in the Rack namespace for 'Module'

struct SeqSwitch : Module {
    enum ParamId {
        STEPS_PARAM,  // Knob to set how many steps (1-8)
        PARAMS_LEN
    };
    enum InputId {
        SIGNAL_INPUT,
        CLOCK_INPUT,
        RESET_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        ENUMS(STEP_OUTPUTS, 8),  // 8 output jacks
        OUTPUTS_LEN
    };
    enum LightId {
        ENUMS(STEP_LIGHTS, 8),  // 8 LEDs
        LIGHTS_LEN
    };
    
    int currentStep = 0;
    dsp::SchmittTrigger clockTrigger;
    dsp::SchmittTrigger resetTrigger;
    
    SeqSwitch() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(STEPS_PARAM, 1.f, 8.f, 8.f, "Number of Steps")->snapEnabled = true;
        configInput(SIGNAL_INPUT, "Signal");
        configInput(CLOCK_INPUT, "Clock");
        configInput(RESET_INPUT, "Reset");
        for (int i = 0; i < 8; i++) {
            configOutput(STEP_OUTPUTS + i, "Step " + std::to_string(i + 1));
        }
    }
    
    void process(const ProcessArgs& args) override {
        int numSteps = (int)params[STEPS_PARAM].getValue();
        
        // Handle reset
        if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
            currentStep = 0;
        }
        
        // Handle clock
        if (clockTrigger.process(inputs[CLOCK_INPUT].getVoltage())) {
            currentStep++;
            if (currentStep >= numSteps) {
                currentStep = 0;
            }
        }
        
        // Get input signal
        float signal = inputs[SIGNAL_INPUT].getVoltage();
        
        // Route signal to active output, silence others
        for (int i = 0; i < 8; i++) {
            if (i == currentStep) {
                outputs[STEP_OUTPUTS + i].setVoltage(signal);
                lights[STEP_LIGHTS + i].setBrightness(1.0f);
            } else {
                outputs[STEP_OUTPUTS + i].setVoltage(0.f);
                lights[STEP_LIGHTS + i].setBrightness(0.f);
            }
        }
    }
};

struct SeqSwitchWidget : ModuleWidget {
    SeqSwitchWidget(SeqSwitch* module) {
        setModule(module);
        
        
        // Create panel (you'll need to make a SeqSwitch.svg file)
        setPanel(createPanel(asset::plugin(pluginInstance, "res/SeqSwitch.svg")));
        
        Label* moduleLabel = new Label();
        moduleLabel->box.pos = mm2px(Vec(-0.75f, 4.0f));
        moduleLabel->text = "SEQ SWITCH";
        moduleLabel->fontSize = 9;
        moduleLabel->color = nvgRGB(0xff, 0x5d, 0x00);
        addChild(moduleLabel);
        
        // Layout constants
        const float centerX = 10.16f;  // Center of a 6HP module (20.32mm / 2)
        const float topY = 15.0f;
        const float spacing = 10.0f;   // Space between rows
        
        // Add screws (optional but nice)
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        // Input section at top
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(centerX, topY)), module, SeqSwitch::SIGNAL_INPUT));
        
        // Clock and Reset
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(centerX - 5.0f, topY + spacing)), module, SeqSwitch::CLOCK_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(centerX + 5.0f, topY + spacing)), module, SeqSwitch::RESET_INPUT));
        
        // Steps knob
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(centerX, (topY + 2) + spacing * 2)), module, SeqSwitch::STEPS_PARAM));
        
        // 8 outputs with LEDs
        float outputStartY = (topY + 3) + spacing * 3;
        for (int i = 0; i < 8; i++) {
            float y = outputStartY + (i * spacing);
            
            // LED to the left of output
            addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(centerX - 5.0f, y)), module, SeqSwitch::STEP_LIGHTS + i));
            
            // Output jack
            addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(centerX + 3.0f, y)), module, SeqSwitch::STEP_OUTPUTS + i));
        }
    }
};

// Don't forget to register the model
Model* modelSeqSwitch = createModel<SeqSwitch, SeqSwitchWidget>("SeqSwitch");
