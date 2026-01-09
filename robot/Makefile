.PHONY: latest assignment1 snapshot

BUILD_LATEST := build_latest
SRC_LATEST := src

BUILD_ASSIGNMENT1 := build_assignment1
SNAPSHOT_ASSIGNMENT1 := snapshots/assignment1_pwm

latest:
	cmake -S . -B $(BUILD_LATEST) -DROBOT_SRC_DIR=$(SRC_LATEST) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(BUILD_LATEST)
	picotool load $(BUILD_LATEST)/robot.uf2
	picotool reboot

assignment1:
	cmake -S $(SNAPSHOT_ASSIGNMENT1) -B $(SNAPSHOT_ASSIGNMENT1)/$(BUILD_ASSIGNMENT1) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(SNAPSHOT_ASSIGNMENT1)/$(BUILD_ASSIGNMENT1)
	picotool load $(SNAPSHOT_ASSIGNMENT1)/$(BUILD_ASSIGNMENT1)/robot.uf2
	picotool reboot
