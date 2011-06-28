TEX = $(wildcard *.tex)

AUX = $(TEX:.tex=.aux)

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
	@evince $(TARGET).pdf

clean :
	rm $(TARGET).pdf $(TARGET).log $(AUX)
