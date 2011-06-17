# SocLib tools
PLATFORM_DESC   =  platform_desc
SOCLIB_INCLUDES = -I.
# use modelsim's sccom
SOCLIB_CC_ARGS  = -j4 $(SOCLIB_INCLUDES) -t sccom

SOCLIB_CC       = soclib-cc

# Verilog compilation
VLOG            = vlog +acc
VLOG_SRC        = video_in.sv wb_soc_slave.sv
VLOG_DAT        = $(patsubst %.sv,work/%/_primary.dat,$(VLOG_SRC))

# SystemC wrappers for verilog modules
WRAP_DIR = hdl/include
WRAPPERS =  $(patsubst %.sv,$(WRAP_DIR)/%.h,$(VLOG_SRC))
SCGENMOD = scgenmod
# wraps wires to bool and vectors to sc_uint
# 'scgenmod -help' for more options
SCGENMOD_OPT = -sc_uint -bool  -map "byte unsigned=unsigned char"

vpath %.sv hdl/SystemVerilog

.PHONY: all clean soft soclibcomp soft_clean vlogcompile xsim sim

all: soft soclibcomp vlogcompile

sim:
	vsim -c sc_main

xsim:
	vsim sc_main

work:
	vlib work

$(WRAP_DIR):
	mkdir $@

soclibcomp: $(PLATFORM_DESC) $(WRAPPERS) | work
	$(SOCLIB_CC) -P $(SOCLIB_CC_ARGS) -p $<

soft:
	$(MAKE) -C soft

vlogcompile: $(VLOG_DAT)

work/%/_primary.dat:%.sv | work
	$(VLOG) $<

$(WRAP_DIR)/%.h:work/%/_primary.dat | $(WRAP_DIR)
	$(SCGENMOD) $(SCGENMOD_OPT) $* > $@

clean: soft_clean
	@rm -f  tty.log
	rm -rf work
	rm -rf $(WRAP_DIR)
	@rm -f  transcript vsim.wlf
	$(SOCLIB_CC) -P $(SOCLIB_CC_ARGS) -p $(PLATFORM_DESC) -x

soft_clean:
	$(MAKE) -C soft clean

ctags:
	ctags -R * > tags
