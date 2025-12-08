BUILD_DIR = build
MAKEFLAGS += --no-print-directory

build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. && cmake --build .
	@ln -sf "build/compile_commands.json"

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned." 

run:
	@sudo $(BUILD_DIR)/app/AeBPF

debug:
	@sudo cat /sys/kernel/tracing/trace_pipe
