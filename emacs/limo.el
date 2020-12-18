;;; limo-mode

(defun limo-help (s)
  (interactive (list (read-string "Get Limo-Help for: " (thing-at-point 'symbol))))
  (with-help-window "*limo-help*"
    (call-process-shell-command (format "limo -e '(help %s)' -e '(exit)'" s) nil "*limo-help*" :t)))

(define-derived-mode limo-mode scheme-mode "Limo"
  "Derived mode for editing limo-files... Work in progress"
  
  ;; how many arguments before the body
  (put 'defun 'scheme-indent-function 2)
  (put 'with-gensym 'scheme-indent-function 2)
  (put 'with-file-open 'scheme-indent-function 3)
  (put 'defmacro 'scheme-indent-function 2)
  (put 'for-each 'scheme-indent-function 2)
  (put 'for-list 'scheme-indent-function 2)
  (put 'block 'scheme-indent-function 1)
  (put 'synchronized 'scheme-indent-function 1)
  (put 'while 'scheme-indent-function 1)
  (put 'switch 'scheme-indent-function 1)
  
  (setq prettify-symbols-alist
        '(("lambda" . 955))) 
  (prettify-symbols-mode t)
  
  (define-key limo-mode-map (kbd "C-c h") 'limo-help)
  (define-key limo-mode-map (kbd "C-c p") 'paredit-mode)
  (show-paren-mode))

(add-to-list 'auto-mode-alist '("\\.limo\\'" . limo-mode))
