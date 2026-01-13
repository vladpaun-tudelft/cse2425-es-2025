.PHONY: latest assignment1 assignment2 assignment3

BUILD_LATEST := build_latest
SRC_LATEST := src

BUILD_ASSIGNMENT1 := build_assignment1
SNAPSHOT_ASSIGNMENT1 := snapshots/assignment1_pwm

BUILD_ASSIGNMENT2 := build_assignment2
SNAPSHOT_ASSIGNMENT2 := snapshots/assignment2_pio

BUILD_ASSIGNMENT3 := build_assignment3
SNAPSHOT_ASSIGNMENT3 := snapshots/assignment3_pid

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

assignment2:
	cmake -S $(SNAPSHOT_ASSIGNMENT2) -B $(SNAPSHOT_ASSIGNMENT2)/$(BUILD_ASSIGNMENT2) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(SNAPSHOT_ASSIGNMENT2)/$(BUILD_ASSIGNMENT2)
	picotool load $(SNAPSHOT_ASSIGNMENT2)/$(BUILD_ASSIGNMENT2)/robot.uf2
	picotool reboot

assignment3:
	cmake -S $(SNAPSHOT_ASSIGNMENT3) -B $(SNAPSHOT_ASSIGNMENT3)/$(BUILD_ASSIGNMENT3) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(SNAPSHOT_ASSIGNMENT3)/$(BUILD_ASSIGNMENT3)
	picotool load $(SNAPSHOT_ASSIGNMENT3)/$(BUILD_ASSIGNMENT3)/robot.uf2
	picotool reboot

