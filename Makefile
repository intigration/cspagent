#
# Makefile for CSP Utilities Library 
# Cloud Services Platform v1.0
# Mentor Graphics Limited
#

include ./Makefile.variables

CXXFLAGS	+= -fPIC -std=c++11
APP			 = cspagent_hello 

LDFLAGS +=	-L$(SYSROOT)/usr/lib				          \
			-L$(SYSROOT)/lib					          \
            -L$(SYSROOT)/usr/local/mgc/lib -lcspeappagent \
            -lboost_system                                \
			-Wl,-rpath=/usr/local/mgc/lib
										  
OBJECTS	  += $(OBJ_DIR)/cspagent_app.o          \
             $(OBJ_DIR)/main.o

INCLUDES  += -Iinc								\
			 -I$(SYSROOT)/usr/include			\
			 -I$(SYSROOT)/include				\
			 -I$(SYSROOT)/usr/local/mgc/include
						 
.PHONEY : all
all: $(APP)

# Link Objects & Produce Output
$(APP): $(OBJECTS)
	@echo ""
	@echo "Linking..." $(APP)
	@$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/$(APP) $(OBJECTS) $(LDFLAGS)			

# Compile all objects
$(OBJ_DIR)/%.o : %.cpp
	@$(CHK_DIR_EXISTS) $(OBJ_DIR) || $(MKDIR) $(OBJ_DIR) 
	@echo "Compiling..." $<
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
		
.PHONEY : clean
clean:
	@echo ""
	@echo "Cleaning $(APP)..."
	@rm -rf $(OBJECTS) $(OBJ_DIR)
	@echo "DONE"
		
.PHONEY : install
install:
	@echo ""
	@echo "Installing $(APP)..."
	@cp $(OBJ_DIR)/$(APP) $(TARGET_BIN)

.PHONEY : uninstall
uninstall:
	@echo ""
	@echo "Un-installing $(APP) from target filesystem..."
	@-rm -rf $(TARGET_BIN)/$(APP)  
		
.PHONEY : distclean
distclean:
	@echo ""
	@echo "Cleaning Everything $(APP)..."
	@rm -rf $(BUILD_DIR)
	@echo "DONE"

vpath %.cpp $(SRC_DIR)
vpath %.o $(OBJ_DIR)
