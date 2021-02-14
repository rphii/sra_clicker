.PHONY: clean All

All:
	@echo "----------Building project:[ sra_clicker - Release ]----------"
	@cd "sra_clicker" && "$(MAKE)" -f  "sra_clicker.mk"
clean:
	@echo "----------Cleaning project:[ sra_clicker - Release ]----------"
	@cd "sra_clicker" && "$(MAKE)" -f  "sra_clicker.mk" clean
