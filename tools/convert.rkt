#lang racket

(require racket/path)

(define mhe_working_directory "../bin/Debug/")
(define exporter_cmdline "\"../../tools/meshconverter/build/Debug/meshconverter.exe\" ~a ~a")
(define binary_extension "bin")
(define mhe_meshes_directory "../../assets/meshes/")

;; Put in the same directory with the same name but the extension changed to .bin
(define (build-output-name src)
  (add-extension (fullpath-without-extension (string->path src)) binary_extension)
)

(define (add-extension p e)
  (string->path (string-append (path->string p) e))
)

(define (fullpath-without-extension filename)
  (let ([ext (bytes->string/utf-8 (filename-extension filename))]
		[filename_str (path->string filename)])
	(string->path (string-trim filename_str ext #:left? #f #:repeat? #f)))
)

(define (copy-file-verbose src dst)
  (printf "copy ~a -> ~a" src dst)
  (copy-file src dst #t)
)

(command-line
 #:args (src)
 (current-directory mhe_working_directory)
 (let ([output_name (build-output-name src)])
   (system (format exporter_cmdline src output_name))
   (copy-file-verbose output_name (build-path mhe_meshes_directory (file-name-from-path output_name))))
)
 
