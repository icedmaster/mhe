#lang racket

(provide parse
		 print-result
		 type-desc?
		 type-desc-name
		 type-desc-type
		 type-desc-children
		 var-desc?
		 var-desc-name
		 var-desc-type
		 SET_COMMAND
		 GET_COMMAND)

(define SET_COMMAND "s")
(define GET_COMMAND "g")

(struct type-desc ([name #:mutable] [type #:mutable] [children #:mutable] [children-number #:mutable]))
(struct var-desc ([name #:mutable] [type #:mutable]))

(define (make-type-desc)
  (type-desc "" "" '() 0)
)

(define (type-desc-append-child t child)
  (set-type-desc-children! t (append (type-desc-children t) (list child)))
  t
)

(define (type-desc-children-count t)
  (length (type-desc-children t))
)

(define (current-element curr)
  (if (pair? curr) (car curr) (cons '() '()))
)

(define (prev-element curr)
  (if (pair? curr) (cdr curr) (cons '() '()))
)

(define (print-result elements)
  (printf "print-results ~a\n" elements)
  (when (> (length elements) 0)
	(for ([e elements])
	  (printf "name:~a ~a\n\tfields:" (type-desc-name e) (length (type-desc-children e)))
	  (print-result (type-desc-children e))))
)

(define (dbg-print curelement elements)
  (printf "curr:~a\n" curelement)
  (when (type-desc? (current-element curelement))
	(printf "name:~a\n" (type-desc-name (current-element curelement))))
  (printf "elements length ~a\n" (length elements))
)

(define (add-element curelement elements)
  (printf "add-element ~a ~a\n" curelement elements)
  (define result '())
  (let ([prev (prev-element curelement)] [curr (current-element curelement)])
	(if (not (type-desc? prev))
		(begin (set! result (list #t curelement (append elements (list curr)))))
		(begin (set! curelement (cons curr (type-desc-append-child prev curr)))
		 (set! result (list #f curelement elements)))
		))
  result
)

(define (get-field-type arg curelement elements)
  (printf "get-field-type\n")
  (let ([curr (current-element curelement)] [prev (prev-element curelement)])
	(set-type-desc-type! curr (string->number arg))
	(add-element curelement elements))
  (list find-fields curelement elements)
)

(define (get-field-name arg curelement elements)
  (printf "get-field-name ~a\n" arg)
  (set! curelement (cons (make-type-desc) (current-element curelement)))
  (set-type-desc-name! (current-element curelement) arg)
  (list get-field-type curelement elements)
)

(define (get-fields-number arg curelement elements)
  (printf "get-fields-number\n")
  (let ([curr (current-element curelement)])
	(set-type-desc-children-number! (current-element curelement) (string->number arg))
	(if (= (type-desc-children-number curr) 0)
		(let ([prev (prev-element curelement)])
		  (set! curelement (cons prev (prev-element prev))))
		#f)
	
	(list get-field-name curelement elements)
	)
)

(define (find-fields arg curelement elements)
  (printf "find-fields\n")
  (if (string=? arg "fields")
	  (list get-fields-number curelement elements) #f)
 )

(define (get-type-name arg curelement elements)
  (printf "get-type-name\n")
  (set-type-desc-name! (current-element curelement) arg)
  (list find-fields curelement elements)
)

(define (find-type arg curelement elements)
  (if (string=? arg "type")
	  (set! curelement (cons (make-type-desc) curelement))
	  #f)
  (list get-type-name curelement elements)
)

(define (process-types inlist state curelement elements)
  (printf "process-types\n")
  (unless (eq? inlist empty)
	(printf "===========~a============\n" (car inlist)))
  (dbg-print curelement elements)
  (if (eq? inlist empty)
	  (let ([ret (add-element curelement elements)])
		(third ret))
	  (let ([ret (state (car inlist) curelement elements)])
		 (if (not ret)
			 (begin (printf "error occured") (exit))
			 (process-types (rest inlist) (first ret) (second ret) (third ret))
			 )))
)

(define (process-type-parsing inlist)
  (printf "process-type-parsing:~a\n" inlist)
  (define curelement '())
  (define elements '())
  (process-types inlist find-type curelement elements)
)

(define (process-var-parsing inlist)
  (printf "process-var-parsing:~a\n" inlist)
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
