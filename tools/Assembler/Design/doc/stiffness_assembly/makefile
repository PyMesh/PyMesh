input := $(wildcard *.tex)
all: $(input:.tex=.pdf)

%.pdf: *.tex
	pdflatex $?
	open $@

