#######################################################
#		Copyright Thoughtfocus(C) 2010
#
# Makefile to build all BDO modules, including
# core libraries, core apps and server
# libraries and applications.
#
#	USAGE:
#		makemt TARGETNAME
#
# 	Eg:- makemt release
#
#######################################################

#
# NOTE: variable MT must be set from command line
#

#XIPCROOT=/local/xipc
LIBDIR=../Libraries/core_aix
LIBBIN=$(LIBDIR)/bin
SRVDIR=../Server_AIX
SRVBIN=$(SRVDIR)/bin

# Target names as string, used for comparison below, in
# arriving at the correct build command.
tgrelease=release
tgdebug=debug
tgclean=clean
tgconvert=convert
tgconvAndBldRel=convAndBldRel
tgconvAndBldDbg=convAndBldDbg

####################################################################
#
#	Set Build command based on target specified
#
#	$(Make)			for target release
#	$(Make) 		for target debug
#	$(Make) clean		for target clean
#	nt_to_unix_all		for target convert
#	/usr/bin/build1		for target convAndBldRel
#	$(Make)			for target convAndBldDbg
#
BUILD_CMD=$(if $(subst $(MT),,$(tgrelease)), \
$(if $(subst $(MT),,$(tgdebug)), \
$(if $(subst $(MT),,$(tgclean)), \
$(if $(subst $(MT),,$(tgconvert)), \
$(if $(subst $(MT),,$(tgconvAndBldRel)), \
$(if $(subst $(MT),,$(tgconvAndBldDbg)), \
echo 'ERROR: Please set MT paramemter', \
$(MAKE)), \
/usr/bin/build1 ), \
nt_to_unix_all), \
$(MAKE) clean), \
$(MAKE)), \
$(MAKE))
#
BUILD_CMD=$(MAKE) clean; $(MAKE)

# build commands are in reverse order of targets specified above:
#	convAndBldDbg, convAndBldRel, convert, clean, debug, release
#
####################################################################

.PHONY: default
# Default target prints available options
default:
#	-@echo $(BUILD_CMD)
#	-@echo $(MT)
	-@echo '\t\t\tOptions:
	-@echo '\t\t\t------- '
	-@echo ' '
	-@echo '\t\t\tmakemt'
	-@echo '\t\t\tmakemt help'
	-@echo '\t\t\tmakemt clean'
	-@echo '\t\t\tmakemt release'
	-@echo '\t\t\tmakemt debug'
	-@echo '\t\t\tmakemt convert'
	-@echo '\t\t\tmakemt convAndBldRel'
	-@echo '\t\t\tmakemt convAndBldDbg'
	-@echo ' '
	-@echo '\t\t\tSee Readme.txt or type "makemt help" for an
	-@echo '\t\t\texplanation of the above options.'

.PHONY: help
help:
#	-@echo $(BUILD_CMD)
	@cat Readme.txt|more

.PHONY: release
release: part1 part2 part3 part4

.PHONY: debug
debug: part1 part2debug part3 part4

#
#	Part 1 below includes all libraries that can be built
# without any dependency on any other module
#
.PHONY: part1
part1:
#	-@echo $(BUILD_CMD)
#	-@echo $(MT)
	cd $(LIBDIR)/pte && $(BUILD_CMD) && cd ../../../build
	cd $(LIBDIR)/pteipc && $(BUILD_CMD) && cd ../../../build
	cd $(LIBDIR)/ptecom && $(BUILD_CMD) && cd ../../../build
	cd $(LIBDIR)/ptedb && $(BUILD_CMD) && cd ../../../build

#
#	Part 2 below includes all libraries that can be built
# without any dependency on any other module. In addition, for
# building a debug library, the makefile provides a different
# configuration (other than the default target).
#
# Below are the makefile targets for compiling a RELEASE build.
#
.PHONY: part2
part2:
	cd $(LIBDIR)/util && $(BUILD_CMD) && cd ../../../build
	cd $(LIBDIR)/mymalloc && $(BUILD_CMD) && cd ../../../build
	cd $(LIBDIR)/xcom && $(BUILD_CMD) && cd ../../../build

#
#	Part2debug below includes all libraries that can be built
# without any dependency on any other module. In addition, for
# building a debug library, the makefile provides a different
# configuration (other than the default target).
#
# Below are the makefile targets for compiling a DEBUG build.
#
.PHONY: part2debug
part2debug:
	cd $(LIBDIR)/util && $(BUILD_CMD) debug && cd ../../../build
	cd $(LIBDIR)/mymalloc && $(BUILD_CMD) staticlib && cd ../../../build
	cd $(LIBDIR)/xcom && $(BUILD_CMD) staticlib && cd ../../../build

#
#	part3 below, is the group of modules that are dependent
# on the libraries built above.
#
.PHONY: part3
part3:
	
	cd $(SRVDIR)/libequit && $(BUILD_CMD) && cd ../../build
	cd $(LIBDIR)/dialog && $(BUILD_CMD) && cd ../../../build
	cd $(LIBDIR)/applnk && $(BUILD_CMD) && cd ../../../build
	cd $(LIBDIR)/srvcmgr && $(BUILD_CMD) && cd ../../../build
	cd $(LIBDIR)/coreds && $(BUILD_CMD) && cd ../../../build
	cd $(LIBDIR)/xcomcmdln && $(BUILD_CMD) && cd ../../../build
	cd $(SRVDIR)/xcomdaemon && $(BUILD_CMD) && cd ../../build
#	cd $(SRVDIR)/xcomguisrv && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/devds && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/log_file && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncjcb && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/netds && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/Falcon2 && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/Falcon_arc && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/LogDecryptor && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/timerds && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/atpkill && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/dumpsql && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncjcb2 && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/pingapp && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/trands && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/atpnetman && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/atpnetmon && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/dbtest && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/merchbatch && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncmcr2 && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/qcheck && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/txcntl && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/hostgen && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/natma2 && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncmcrd && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/racal && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/updatds && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/binimport && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/import && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncamaq && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncvis2 && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/repds && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/cleanup && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncamex && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncvisa && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/sbatch && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncvsms && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/settle && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/dcpiso && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/nccup && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/nccup2 && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/nciftr && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/dcpimp && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/settle_mp && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/nceqit && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/dbf_file && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/dbf_file_archive && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/sbatch_mp && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/dcpos && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/posds && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/mpva_file && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncdci && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncdci2 && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ncpul && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/Mracon2 && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/fconds && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/merch_integrity_updation && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/ftimer && $(BUILD_CMD) && cd ../../build
#   cd $(SRVDIR)/Falcon && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/FAIrcv && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/FAIsnd && $(BUILD_CMD) && cd ../../build
	cd $(SRVDIR)/tr31keyblock && $(BUILD_CMD) && cd ../../build
    
	

.PHONY: part4
part4:
	cp $(LIBBIN)/* ./bin/
	cp $(SRVBIN)/* ./bin/

.PHONY: clean
clean: part1 part2 part3
	-@rm -f $(LIBDIR)/bin/*
	-@rm -f $(LIBDIR)/lib/*
	-@rm -f $(SRVDIR)/bin/*
	-@rm -f $(SRVDIR)/lib/*
	-@rm -f $(SRVDIR)/xcomdaemon/xcomdaemon
	-@rm -f ./bin/*

.PHONY: convert
convert: clean part1 part2 part3
	-cd $(LIBDIR)/include && nt_to_unix_all && cd ../../build
	-cd $(SRVDIR)/include && nt_to_unix_all && cd ../../build
	-cd $(SRVDIR)/Common && nt_to_unix_all && cd ../../build

.PHONY: convAndBldRel
convAndBldRel: clean part1 part2 part3 part4

.PHONY: convAndBldDbg
convAndBldDbg: clean convert part1 part2debug part3 part4

.PHONY: delbins
delbins:
	-@rm -f ./bin/* $(SRVBIN)/* $(LIBBIN)/* $(SRVBIN)/../xcomdaemon/xcomdaemon
