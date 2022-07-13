# Set project directory one level above of Makefile directory. $(CURDIR) is a GNU make variable containing the path to the current working directory
PROJDIR := $(realpath $(CURDIR))
SOURCEDIR := $(PROJDIR)/src
BUILDDIR := $(PROJDIR)/build

# Name of the final executable
TARGET = TetriC

# Decide whether the commands will be shown or not
VERBOSE = TRUE

# Create the list of directories
DIRS = src

OBJDIR = objs

TARGETDIRS = $(foreach dir, $(DIRS), $(dir))

# Generate the GCC includes parameters by adding -I before each source folder
INCLUDES = $(foreach dir, $(DIRS), $(addprefix -I, $(dir)))

# Add this list to VPATH, the place make will look for the source files
VPATH = $(DIRS)

# Create a list of *.c sources in DIRS
SOURCES = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))

# Define objects for all sources
OBJS := $(subst $(DIRS),$(OBJDIR),$(SOURCES:.c=.o))

# Define dependencies files for all objects
DEPS = $(OBJS:.o=.d)

# Name the compiler
CC = gcc

CCFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image -g -Wall

$(OBJDIR)/%.o: %.c
	@echo "COMPILING SOURCE $< INTO OBJECT $@"
	@mkdir -p '$(@D)'
	@$(CC) -c $(CCFLAGS) $< -o $@

# OS specific part
ifeq ($(OS),Windows_NT)
    RM = del /F /Q 
    RMDIR = -RMDIR /S /Q
    MKDIR = -mkdir
    ERRIGNORE = 2>NUL || true
    SEP=\\
else
    RM = rm -f 
    RMDIR = rm -rf 
    MKDIR = mkdir -p
    ERRIGNORE = 2>/dev/null
    SEP=/
endif

# Remove space after separator
PSEP = $(strip $(SEP))

# Hide or not the calls depending of VERBOSE
ifeq ($(VERBOSE),TRUE)
    HIDE =  
else
    HIDE = @
endif

.PHONY: all clean directories 

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(HIDE)@echo Linking $@
	$(CC) $(OBJS) $(CCFLAGS) -o $(TARGET)

directories: 
	$(MKDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)

# Remove all objects, dependencies and executable files generated during the build
clean:
	$(RMDIR) $(subst /,$(PSEP),$(OBJDIR)) $(ERRIGNORE)
	$(RM) $(TARGET) $(ERRIGNORE)
	@echo Cleaning done ! 

