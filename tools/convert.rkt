#lang racket

(require racket/path)

(define mhe_working_directory "../bin/Debug/")
(define exporter_cmdline "\"../../tools/meshconverter/build/Debug/meshconverter.exe\" ~a ~a /textures")
(define binary_extension "bin")

;; Put in the same directory with the same name but the extension changed to .bin
(define (build-output-name src)
  (add-extension (filename-without-extension (string->path src)) binary_extension)
)

(define (add-extension p e)
  (string->path (string-append (path->string p) e))
)

(define (filename-without-extension filename)
  (let ([ext (bytes->string/utf-8 (filename-extension filename))]
		[filename_str (path->string filename)])
	(string->path (string-trim filename_str ext #:left? #f #:repeat? #f)))
)

(command-line
 #:args (src)
 (current-directory mhe_working_directory)
 (system (format exporter_cmdline src (build-output-name src)))
)
 
