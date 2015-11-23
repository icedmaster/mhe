#lang racket

(require "protocol.rkt")

(provide make-empty-context
		 make-context
		 context-type
		 context-var
		 print-variables-available)

(struct context (types components vars))

(define (make-empty-context)
  (context '() '() '())
)

(define (make-context types vars)
  (context types '() vars)
)

(define (context-type c type)
  (find-context-type (context-types c) type)
)

(define (find-context-type inlist type)
  (let ([l (filter (lambda (e) (string=? (first type) (type-desc-name e))) inlist)])
	(if (eq? empty l)
		(begin (printf "Can't find data with name ~a\n" (first type)) #f)
		(let ([next-types (rest type)])
		  (if (eq? empty next-types)
			  (first l)
			  (find-context-type (type-desc-children (first l)) next-types)))))
)

(define (context-var c name)
  (let ([l (filter (lambda (e) (string=? name (var-desc-name e))) (context-vars c))])
	(if (eq? l empty)
		#f
		(first l)))
)

(define (print-variables-available context)
  (for ([v (context-vars context)])
	(printf "~a\n" (var-desc-name v)))
)

