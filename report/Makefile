
SOURCE = Report
PAPER = a4
LATEXMK = ./tools/latexmk

OBJS = bbl ilg ind blg glo out aux dvi log ps backup bak toc idx DS_Stor

.PHONY: all print bib clean deep-clean dvi

all: update show

update: deep-clean bib

preview:
	$(LATEXMK) -dvi -pvc -silent $(SOURCE).tex

dvi: $(SOURCE).dvi
$(SOURCE).dvi: $(SOURCE).tex
	latex $<
	latex $<

ps: $(SOURCE).ps
$(SOURCE).ps: $(SOURCE).dvi
	dvips -Ppdf -G0 -t$(PAPER) -o $@ $<

pdf: $(SOURCE).pdf
$(SOURCE).pdf: $(SOURCE).ps
	ps2pdf -dSubsetFonts=true -dMaxSubsetPct=100 -dEmbedAllFonts=true \
		-dUseFlateCompression=true -dCompatibilityLevel=1.4 \
		-dPDFSETTINGS=/prepress -sPAPERSIZE=$(PAPER) $< $@

bib: $(SOURCE).bbl
$(SOURCE).bbl : $(SOURCE).bib
	latex $(SOURCE).tex
	bibtex $(SOURCE)
	rm -f $(SOURCE).dvi

report: $(SOURCE).pdf
	pdffonts $< > fonts.log
	cat fonts.log

show: pdf
	kill -9 `ps aux | grep $(SOURCE).pdf | grep -v grep \
		| awk '{print $2}'` >/dev/null 2>&1 || true
	gnome-open $(SOURCE).pdf

clean:
	for e in $(OBJS); do \
		find . -path './.git' -prune -o -name "*.$$e" \
		-exec rm -f \{\} \; ;\
		done
	find . -path './.git' -prune -o -name "*~" -exec rm -f \{\} \;

deep-clean: clean
	rm -rf *.bbl *.pdf *.fdb_latexmk

