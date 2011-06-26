TEX = $(wildcard *.tex)
PDF = $(TEX:.tex=.pdf)

FIGS = $(wildcard figs/*.pdf)

TARGET = SoC-report

makebib=no


$(TARGET).pdf : $(TEX) $(FIGS)
	@pdflatex $(TARGET).tex
ifeq ($(makebib),yes)
	@bibtex   $(TARGET).aux
endif
	@pdflatex $(TARGET).tex
	@pdflatex $(TARGET).tex
	@pdflatex $(TARGET).tex


clean :
	rm $(PDF)
