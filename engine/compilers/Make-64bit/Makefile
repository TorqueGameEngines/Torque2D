DEPS :=
LIB_TARGETS :=
LIB_TARGETS_DEBUG :=
SHARED_LIB_TARGETS :=
SHARED_LIB_TARGETS_DEBUG :=
APP_TARGETS :=
APP_TARGETS_DEBUG :=

build-in-docker: docker-buildenv
	docker run  \
		--rm \
		--user $(shell id -u):$(shell id -g) \
		-v $(shell readlink -e ../../../ ):/torque2d-engine-build/ \
		-w /torque2d-engine-build/engine/compilers/Make-64bit/ \
		torque2d-linux64-build-env \
			make -j all

all: debug release

docker-buildenv: Dockerfile
	docker build -t torque2d-linux64-build-env .

clean:
	rm -rf Debug
	rm -rf Release
	rm -rf lib

.PHONY: all debug release clean

-include x Torque2D.mk
-include x zlib
-include x lpng
-include x ljpeg
-include x vorbis
-include x ogg

release: $(LIB_TARGETS) $(SHARED_LIB_TARGETS) $(APP_TARGETS)
	@echo Built libraries: $(LIB_TARGETS)
	@echo Built shared libraries: $(SHARED_LIB_TARGETS)
	@echo Built apps: $(APP_TARGETS)

debug: $(LIB_TARGETS_DEBUG) $(SHARED_LIB_TARGETS_DEBUG) $(APP_TARGETS_DEBUG)
	@echo Built libraries: $(LIB_TARGETS_DEBUG)
	@echo Built shared libraries: $(SHARED_LIB_TARGETS_DEBUG)
	@echo Built apps: $(APP_TARGETS_DEBUG)
