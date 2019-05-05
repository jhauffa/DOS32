OBJDIR = objs
OBJS_DOS32 = ImageFactory.o ImageLoaderException.o LEImage.o ExecutionEnvironment.o \
	DOS.o DOSExtender.o DPMI.o DescriptorTable.o Launcher.o \
	os/Context.o os/ExceptionInfo.o \
	os/unix/UnixMemMap.o os/unix/UnixException.o os/unix/UnixThread.o \
	os/unix/DarwinContext.o os/unix/DarwinExceptionManager.o \
	os/unix/DarwinExceptionInfo.o os/unix/DarwinLDT.o os/unix/UnixDateTime.o os/unix/OS.o
HEADERS_DOS32 = *.h os/*.h os/unix/*.h

GEN_DOS32 := $(addprefix $(OBJDIR)/,$(OBJS_DOS32))

.PHONY: all
all : $(OBJDIR) dos32

.PHONY: clean
clean :
	-rm -r $(OBJDIR)
	-rm dos32

$(OBJDIR) :
	mkdir $(OBJDIR)
	mkdir $(OBJDIR)/os
	mkdir $(OBJDIR)/os/unix

dos32 : $(GEN_DOS32) $(HEADERS_DOS32)
	$(CXX) -m32 -o $@ $(GEN_DOS32)

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -I. -m32 -c -o $@ $<
