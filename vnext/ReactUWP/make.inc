LIBLET_LOB = 1

!include $(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\makewinrt.inc

SOLUTION_DIR = $(PROJROOT)\OfficeISS\ReactWindows\ReactUWP

INCLUDES = $(INCLUDES); \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\include\ReactUWP; \
	$(SOLUTION_DIR) \
