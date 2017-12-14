#!/usr/bin/env python
# -*- coding: utf-8 -*-

from petsys import daqd, config
from copy import deepcopy
import argparse

parser = argparse.ArgumentParser(description='Acquire data for TDC calibration')
parser.add_argument("--config", type=str, required=True, help="Configuration file")
parser.add_argument("-o", type=str, dest="fileNamePrefix", required=True, help="Data filename (prefix)")
parser.add_argument("--time", type=float, required=True, help="Acquisition time (in seconds)")
parser.add_argument("--mode", type=str, required=True, choices=["tot", "qdc"], help="Acquisition mode (ToT or QDC)")
parser.add_argument("--enable-hw-trigger", dest="hwTrigger", action="store_true", help="Enable the hardware coincidence filter")
args = parser.parse_args()

systemConfig = config.ConfigFromFile(args.config)

daqd = daqd.Connection()
daqd.initializeSystem()
systemConfig.loadToHardware(daqd, bias_enable=config.APPLY_BIAS_ON, hw_trigger_enable=args.hwTrigger)

qdcMode = args.mode == "qdc"

asicsConfig = daqd.getAsicsConfig()
for ac in asicsConfig.values():
	gc = ac.globalConfig

        #configure feedback channel
        #To activate debug output for any channel (one at a time) change "debug_mode" to 0b01 from 0b00. In config.ini, change global.debug_mode to 1 from 0.
        #ac.channelConfig[23].setValue("debug_mode",0b01)
        #ac.channelConfig[23].setValue("trigger_mode_2_q",0b00)

	for cc in ac.channelConfig:
		cc.setValue("trigger_mode_1", 0b00)

		if not qdcMode:
			cc.setValue("qdc_mode", 0)
			cc.setValue("intg_en", 0)
			cc.setValue("intg_signal_en", 0)
		else:
			cc.setValue("qdc_mode", 1)
			cc.setValue("intg_en", 1)
			cc.setValue("intg_signal_en", 1)
	
#
#	MCP CHANNEL SETUP
#
	
#Put here the channel number of the MCP
	
targetChannel = 41
for ac in asicsConfig.values():
	# Set single threshold mode for target channel
	ac.channelConfig[targetChannel].setValue("trigger_mode_2_t", 0b00)
	ac.channelConfig[targetChannel].setValue("trigger_mode_2_e", 0b000)
	ac.channelConfig[targetChannel].setValue("trigger_mode_2_q", 0b00)
	ac.channelConfig[targetChannel].setValue("trigger_mode_2_b", 0b00)

        #turn on feedback (debug) channel
        ac.channelConfig[targetChannel].setValue("debug_mode",0b01)

	# Set threshold for vth_T1 at the desired value from 0 (higher threshold) to 63 (lower threshold). Now setted to 30. Beware, code 63 might be below baseline!!
	ac.channelConfig[targetChannel].setValue("vth_t1", 50)

targetChannel = 32
for ac in asicsConfig.values():
	# Set single threshold mode for target channel
	ac.channelConfig[targetChannel].setValue("trigger_mode_2_t", 0b00)
	ac.channelConfig[targetChannel].setValue("trigger_mode_2_e", 0b000)
	ac.channelConfig[targetChannel].setValue("trigger_mode_2_q", 0b00)
	ac.channelConfig[targetChannel].setValue("trigger_mode_2_b", 0b00)

        #turn on feedback (debug) channel
        #ac.channelConfig[targetChannel].setValue("debug_mode",0b01)

	# Set threshold for vth_T1 at the desired value from 0 (higher threshold) to 63 (lower threshold). Now setted to 30. Beware, code 63 might be below baseline!!
	ac.channelConfig[targetChannel].setValue("vth_t1", 40)


# TO enable special channel 0
#for ac in asicsConfig.values():
#        ac.channelConfig[0].setValue("trigger_mode_1", 0b00)
#        ac.channelConfig[0].setValue("trigger_mode_1", 0b01)
#daqd.setAsicsConfig(asicsConfig, forceAccess=True)

#
#	END of MCP CHANNEL SETUP
#

daqd.setAsicsConfig(asicsConfig)

daqd.openRawAcquisition(args.fileNamePrefix, qdcMode=qdcMode)
daqd.acquire(args.time, 0, 0);

systemConfig.loadToHardware(daqd, bias_enable=config.APPLY_BIAS_OFF)
