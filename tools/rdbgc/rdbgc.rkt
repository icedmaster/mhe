#lang racket

(require racket/tcp)
(require "protocol.rkt"
		 "rdbg-context.rkt")

(define (check-vec4 args)
  (define result #t)
  (if (= (length args) 4)
	  (for ([a args])
		#:break (not result)
		(unless (string->number a) (set! result #f)))
	  (set! result #f))
  result
)

(define (check-int args)
  (string->number (first args))
)

(define types-map (list (list 5 check-vec4) (list 1 check-int) ))

(define (exit-app in out)
  (close-input-port in)
  (close-output-port out)
  (exit)
)

(define (check-errors data)
  (when (> (string-length data) 4)
	(if (string=? "err " (substring data 0 4))
		((printf "error:~a\n" data) #t)
		#f))
  #f
)

(define (print-answer-handler context data)
  (printf "~a\n" data)
)

(define (request-all context in out)
  (send-command context in out "get" parse-request-all)
)

(define (parse-request-all context data)
  (printf "request-all: ~a\n" data)
  (let ([parse-result (parse (string-split data " "))])
	(print-result (first parse-result))
	(set! context (make-context (first parse-result) (second parse-result))))
  context
)

(define (valid-args? type args)
  (let ([t type] [set-args args])
	(let ([check-func (second (first (filter (lambda (tm) (= (first tm) t)) types-map)))])
	  (check-func set-args)))
)

(define (make-command cmd type args)
  (string-join args " ")
)

(define (type-set-command type args context in out)
  (if (valid-args? (type-desc-type type) (list-tail args 3))
	  (send-command context in out (make-command SET_COMMAND type args) print-answer-handler)
	  (printf "Arguments are invalid\n"))
)

;; typename.field id s value
(define (process-type type args context in out)
  (if (< (length args) 2)
	  (begin (printf "Invalid number of arguments. You shall use set or get command\n") #f)
	  (let ([cmd (third args)])
		(cond
		 [(string=? cmd SET_COMMAND) (type-set-command type args context in out)]
		 [(string=? cmd GET_COMMAND) #f]
		 [else (printf "Invalid command ~a\n" cmd) #f])))
)

(define (var-set-command var args context in out)
  (if (valid-args? (var-desc-type var) (list-tail args 2))
	  (send-command context in out (make-command SET_COMMAND var args) print-answer-handler)
	  (printf "Arguments are invalid\n"))
)

;; var_name s value
;; var_name g value
(define (process-var var args context in out)
  (if (< (length args) 3)
	  (begin (printf "Invalid number of arguments\n") #f)
	  (let ([cmd (second args)])
		(cond
		 [(string=? cmd SET_COMMAND) (var-set-command var args context in out)]
		 [else (printf "Unknown command ~a\n" cmd) #f])))
)

(define (do-command data context in out)
  (let ([args (string-split data " ")])
	(let ([var (context-var context (first args))])
	  (if (var-desc? var)
		  (process-var var args context in out)
		  (let ([type (context-type context (string-split (first args) "."))])
			(cond
			 [(type-desc? type) (process-type type args context in out)]
			 [else #f])))))
)

(define (get-profile-data context in out)
  (send-command context in out "profiler" print-answer-handler)
)

(define (get-stats context in out)
  (send-command context in out "stats" print-answer-handler)
)

(define (read-command in)
  (let ([length (integer-bytes->integer (read-bytes 4 in) #f #t 0 4)])
	(read-string length in))
)

(define (send-command context in out cmd handler)
  (write-bytes (integer->integer-bytes (string-length cmd) 4 #f #t) out)
  (write-string cmd out)
  (flush-output out)
  (let ([data (read-command in)])
	(if (equal? (check-errors data) #f)
		(handler context data) #f
		))
)

(define (user-loop context in out)
  (printf ">")
  (let ([str (string-trim (read-line))])
	(cond
	 [(string=? "quit" str) (exit-app in out)]
	 [(string=? "all" str) (set! context (request-all context in out))]
	 [(string=? "profiler" str) (get-profile-data context in out)]
	 [(string=? "stats" str) (get-stats context in out)]
	 [(string=? "vars" str) (print-variables-available context)]
	 [else (do-command str context in out)]
	 ))
  (user-loop context in out)
)

(define (connect)
  (with-handlers ([exn:fail:network?
				   (lambda (exn)
					 (printf (exn-message exn))
					 (exit)
					 )])
	(tcp-connect "127.0.0.1" 11111)
	)
)

(define (process)
  (let ([context (make-empty-context)])
	(define-values (in out) (connect))
	(printf "Connected to server\n")
	(set! context (request-all context in out))
	(user-loop context in out)
	(exit-app in out))
)

(process)
  
