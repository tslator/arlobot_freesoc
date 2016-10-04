# THIS FILE IS AUTOMATICALLY GENERATED
# Project: F:\d_drive\projects\ArloBot\arlobot_freesoc\freesoc.cydsn\freesoc.cyprj
# Date: Tue, 04 Oct 2016 10:27:36 GMT
#set_units -time ns
create_clock -name {CyILO} -period 10000 -waveform {0 5000} [list [get_pins {ClockBlock/ilo}] [get_pins {ClockBlock/clk_100k}] [get_pins {ClockBlock/clk_1k}] [get_pins {ClockBlock/clk_32k}]]
create_clock -name {CyIMO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyPLL_OUT} -period 15.625 -waveform {0 7.8125} [list [get_pins {ClockBlock/pllout}]]
create_clock -name {CyMASTER_CLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/clk_sync}]]
create_generated_clock -name {CyBUS_CLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 2 3} [list [get_pins {ClockBlock/clk_bus_glb}]]
create_generated_clock -name {SPI_Clock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 3 5} [list [get_pins {ClockBlock/dclk_glb_0}]]
create_generated_clock -name {PWM_Clock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 25 49} [list [get_pins {ClockBlock/dclk_glb_1}]]
create_generated_clock -name {Encoder_Clock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 25 49} [list [get_pins {ClockBlock/dclk_glb_2}]]


# Component constraints for F:\d_drive\projects\ArloBot\arlobot_freesoc\freesoc.cydsn\TopDesign\TopDesign.cysch
# Project: F:\d_drive\projects\ArloBot\arlobot_freesoc\freesoc.cydsn\freesoc.cyprj
# Date: Tue, 04 Oct 2016 10:27:27 GMT
