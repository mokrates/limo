;;; limo-mode

(defun limo-help (s)
  (interactive (list (read-string "Get Limo-Help for: " (thing-at-point 'symbol))))
  (with-help-window "*limo-help*"
    (call-process-shell-command (format "limo -e '(help %s)' -e '(exit)'" s) nil "*limo-help*" :t)))

(define-derived-mode limo-mode scheme-mode "Limo"
  "Derived mode for editing limo-files... Work in progress"
  (put 'defun 'scheme-indent-function 1)
  (put 'with-gensym 'scheme-indent-function 1)
  (put 'defmacro 'scheme-indent-function 1)
  (define-key limo-mode-map (kbd "C-c h") 'limo-help))

(add-to-list 'auto-mode-alist '("\\.limo\\'" . limo-mode))
