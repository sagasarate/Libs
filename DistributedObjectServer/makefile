
WORKDIR = `pwd`

all: debug release

clean: clean_debug clean_release

debug:
	$(MAKE) -C ../Libs/Utils debug
	$(MAKE) -C ../Libs/DBLib debug
	$(MAKE) -C ../Libs/DOSLib debug
	$(MAKE) -C ../Libs/NetLib debug
	$(MAKE) -C ../Libs/ServerFrameWork debug
	$(MAKE) -C ./DistributedObjectServer debug

clean_debug: 	
	$(MAKE) -C ../Libs/Utils clean_debug
	$(MAKE) -C ../Libs/DBLib clean_debug
	$(MAKE) -C ../Libs/DOSLib clean_debug
	$(MAKE) -C ../Libs/NetLib clean_debug
	$(MAKE) -C ../Libs/ServerFrameWork clean_debug
	$(MAKE) -C ./DistributedObjectServer clean_debug

release:
	$(MAKE) -C ../Libs/Utils release
	$(MAKE) -C ../Libs/DBLib release
	$(MAKE) -C ../Libs/DOSLib release
	$(MAKE) -C ../Libs/NetLib release
	$(MAKE) -C ../Libs/ServerFrameWork release
	$(MAKE) -C ./DistributedObjectServer release

clean_release: 
	$(MAKE) -C ../Libs/Utils clean_release
	$(MAKE) -C ../Libs/DBLib clean_release
	$(MAKE) -C ../Libs/DOSLib clean_release
	$(MAKE) -C ../Libs/NetLib clean_release
	$(MAKE) -C ../Libs/ServerFrameWork clean_release
	$(MAKE) -C ./DistributedObjectServer clean_release

.PHONY: debug release clean_debug clean_release

