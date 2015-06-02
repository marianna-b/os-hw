SUBMODULES = lib cat revwords filter bufcat simplesh filesender bipiper

all:
	@for dir in $(SUBMODULES) ; do \
		echo "Making $$dir"; \
		$(MAKE) --no-print-directory -C $$dir; \
	done
clean:
	@for dir in $(SUBMODULES) ; do \
		echo "Cleaning $$dir"; \
		$(MAKE) --no-print-directory clean -C $$dir; \
	done
