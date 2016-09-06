#lang racket

(require racket/path)

(define mhe_working_directory "../bin/")
(define exporter_cmdline "\"../tools/meshconverter/build/meshconverter\" -v 3 -s ~a -d ~a")
(define binary_extension "bin")
(define mesh_extension "mesh")
(define mhe_meshes_directory "../assets/meshes/")
(define mhe-textures-directory (make-parameter ""))
(define mhe-materials-directory (make-parameter ""))
(define copy-textures (make-parameter #t))
(define copy-materials (make-parameter #t))
(define export_args "")

(let ([os (system-type 'os)])
  (printf "OS is ~a\n" os)
  (cond
   [(equal? os 'windows)
	(set! mhe_working_directory "../bin/Debug/")
	(set! exporter_cmdline "\"../../tools/meshconverter/build/Debug/meshconverter.exe\" -v 3 -s ~a -d ~a")
	(set! mhe_meshes_directory "../../assets/meshes/")]
   [(not (equal? os 'unix)) (printf "Unsupported OS\n")]))

;; Put in the same directory with the same name but the extension changed to .mesh and .bin
(define (build-output-name src)
  (let ([base_name (fullpath-without-extension (string->path src))])
    (cons (add-extension base_name mesh_extension) (add-extension base_name binary_extension)))
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
  (printf "copy ~a -> ~a\n" src dst)
  (copy-file src dst #t)
)

(command-line
 #:once-each
 [("--ct" "--copy-textures") "Copy textures after export"
  (copy-textures #t)]
 [("--cm" "--copy-materials") "Copy materials after export"
  (copy-materials #t)]
 [("--tp" "--textures-path") path "Additional textures path"
  (mhe-textures-directory (string-append mhe-textures-directory path))]
 [("--mp" "--materials-path") path "Additional materials path"
  (mhe-materials-directory (string-append mhe-materials-directory path))]
 #:args (src)
 (current-directory mhe_working_directory)
 (let ([output_name (build-output-name src)])
   (let ([export_cmdline (format exporter_cmdline src (cdr output_name))])
     (printf "~a\n" export_cmdline)
     (system export_cmdline))
   (copy-file-verbose (car output_name) (build-path mhe_meshes_directory (file-name-from-path (car output_name))))
   (copy-file-verbose (cdr output_name) (build-path mhe_meshes_directory (file-name-from-path (cdr output_name)))))
)
 
