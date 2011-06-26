TEX = $(wildcard *.tex)
PDF = $(TEX:.tex=.pdf)

TARGET = SoC-report

$(TARGET).pdf : $(TEX)
	@pdflatex $(TARGET).tex
	@bibtex   $(TARGET).aux
	@pdflatex $(TARGET).tex
	@pdflatex $(TARGET).tex
	@pdflatex $(TARGET).tex

clean :
	rm $(PDF)
