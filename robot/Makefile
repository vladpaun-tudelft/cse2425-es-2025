.PHONY: latest assignment1

BUILD_LATEST := build_latest
SRC_LATEST := src

BUILD_ASSIGNMENT1 := build_assignment1
SRC_ASSIGNMENT1 := snapshots/assignment1_pwm/src

latest:
	cmake -S . -B $(BUILD_LATEST) -DROBOT_SRC_DIR=$(SRC_LATEST)
	cmake --build $(BUILD_LATEST)
	picotool load $(BUILD_LATEST)/robot.uf2
	picotool reboot

assignment1:
	cmake -S . -B $(BUILD_ASSIGNMENT1) -DROBOT_SRC_DIR=$(SRC_ASSIGNMENT1)
	cmake --build $(BUILD_ASSIGNMENT1)
	picotool load $(BUILD_ASSIGNMENT1)/robot.uf2
	picotool reboot
