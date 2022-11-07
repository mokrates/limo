;; limo formular-renderer-mode

(require 'thingatpt)
(provide 'frender-mode)

(defvar todo-marker-marker "TODO"
  "The todo marker used by todo-marker-mode")

(defun turn-on-todo-marker ()
  "turns on todo-marker-mode."
  (todo-marker-mode 1))


(defun frender-buffer ()
  (interactive)
  (with-help-window "*limo formula-render*"
    (ignore-errors
      (let* ((foo (save-excursion
		    (goto-char (car (thing-at-point-bounds-of-list-at-point)))
		    (insert "'")
		    (prog1
			(buffer-string)
		      (delete-char -1))))
	     (foo (replace-regexp-in-string "\n" "" foo))
	     ;; (foo (replace-regexp-in-string "'" "\\\\'" foo))
	     (foo (replace-regexp-in-string "\"" "\\\\\"" foo))
	     (thecall (format "limo -e '(load \"/home/moritzm/Syncope/projects/formula-renderer/formula-renderer.limo\")' -e \"(writerender '%s)\" -e '(exit)'" foo)))
	(message thecall)
	(call-process-shell-command thecall nil "*limo formula-render*" :t)))))

(defun frender-command-hook ()
  (while-no-input
    (redisplay)
    (frender-buffer)))

(define-minor-mode frender-mode
  "live-renders the buffer with limos formular-renderer"
  nil
  " frender"
  nil
  (add-hook 'post-command-hook 'frender-command-hook nil t))

