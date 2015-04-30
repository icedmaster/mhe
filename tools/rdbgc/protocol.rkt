#lang racket

(provide parse
		 var-desc?
		 var-desc-name
		 var-desc-type
		 SET_COMMAND
		 GET_COMMAND
		 SET_VAR_COMMAND
		 GET_VAR_COMMAND)

(define SET_COMMAND "s")
(define GET_COMMAND "g")
(define SET_VAR_COMMAND "sv")
(define GET_VAR_COMMAND "gv")

(define TYPE_NONE 0)
(define TYPE_INT 1)
(define TYPE_FLOAT 2)
(define TYPE_STRING 3)
(define TYPE_VEC3 4)
(define TYPE_VEC4 5)

(struct var-desc ([name #:mutable] [type #:mutable]))

(define (process-var-parsing inlist)
  (define vars '())
  (for ([i (range (/ (length inlist) 3))])
	#:break (>= i (length inlist))
	(let ([vartag (list-ref inlist (* i 3))]
		  [varname (list-ref inlist (+ (* i 3) 1))]
		  [vartype (string->number (list-ref inlist (+ (* i 3) 2)))])
	  (when (string=? vartag "var")
		(set! vars (append vars (list (var-desc varname vartype)))))))
  vars
)

(define (parse inlist)
  (let ([varlist (member "var" inlist string=?)])
	(let ([typelist (remove* varlist inlist)])
	  (list (process-type-parsing typelist) (process-var-parsing varlist))))
)

#|
(let ([inlist '("type" "light" "fields" "1" "diffuse" "5" "fields" "0")] [state find-type])
  (define curtest (cons '() '()))
  (define tests '())
  (process-list inlist state curtest tests)
)
|#
