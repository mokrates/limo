docs: limo-info.info

clean:
	rm -f limo-info.texi

realclean: clean
	rm -f limo-info.info *~

limo-info.info: limo-info.org
	emacs --batch --eval='(find-file "limo-info.org")' --eval='(org-texinfo-export-to-info)'
