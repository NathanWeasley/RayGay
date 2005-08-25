
(define (make-rounded-wire-box lowercorner uppercorner radius material)
  ;; Makes a wireframe box with round edges.
  ;; The radius of the round edges are 'radius'.
  (let* (
	 (x+ (- (list-ref uppercorner 0) radius))
	 (y+ (- (list-ref uppercorner 1) radius))
	 (z+ (- (list-ref uppercorner 2) radius))
	 (x- (+ (list-ref lowercorner 0) radius))
	 (y- (+ (list-ref lowercorner 1) radius))
	 (z- (+ (list-ref lowercorner 2) radius))
	 (x+y+z+ (list x+ y+ z+))
	 (x+y+z- (list x+ y+ z-))
	 (x+y-z+ (list x+ y- z+))
	 (x+y-z- (list x+ y- z-))
	 (x-y+z+ (list x- y+ z+))
	 (x-y+z- (list x- y+ z-))
	 (x-y-z+ (list x- y- z+))
	 (x-y-z- (list x- y- z-)))
    (list (make-sphere x+y+z+ radius material)
	  (make-sphere x+y+z- radius material)
	  (make-sphere x+y-z+ radius material)
	  (make-sphere x+y-z- radius material)
	  (make-sphere x-y+z+ radius material)
	  (make-sphere x-y+z- radius material)
	  (make-sphere x-y-z+ radius material)
	  (make-sphere x-y-z- radius material)
	  ; Front
	  (make-cylinder x+y+z+ x+y-z+ radius material)
	  (make-cylinder x+y-z+ x-y-z+ radius material)
	  (make-cylinder x-y-z+ x-y+z+ radius material)
	  (make-cylinder x-y+z+ x+y+z+ radius material)
	  ; Back
	  (make-cylinder x+y+z- x+y-z- radius material)
	  (make-cylinder x+y-z- x-y-z- radius material)
	  (make-cylinder x-y-z- x-y+z- radius material)
	  (make-cylinder x-y+z- x+y+z- radius material)
	  ; Back to front
	  (make-cylinder x+y+z- x+y+z+ radius material)
	  (make-cylinder x+y-z- x+y-z+ radius material)
	  (make-cylinder x-y+z- x-y+z+ radius material)
	  (make-cylinder x-y-z- x-y-z+ radius material))))


(define (make-rounded-box lowercorner uppercorner radius material)
  ;; Makes a box with round edges.
  ;; The radius of the round edges are 'radius'.
  (let* (
	 (xo+ (list-ref uppercorner 0))
	 (yo+ (list-ref uppercorner 1))
	 (zo+ (list-ref uppercorner 2))
	 (xo- (list-ref lowercorner 0))
	 (yo- (list-ref lowercorner 1))
	 (zo- (list-ref lowercorner 2))
	 (x+ (- (list-ref uppercorner 0) radius))
	 (y+ (- (list-ref uppercorner 1) radius))
	 (z+ (- (list-ref uppercorner 2) radius))
	 (x- (+ (list-ref lowercorner 0) radius))
	 (y- (+ (list-ref lowercorner 1) radius))
	 (z- (+ (list-ref lowercorner 2) radius))
	 (x+y+z+ (list x+ y+ z+))
	 (x+y+z- (list x+ y+ z-))
	 (x+y-z+ (list x+ y- z+))
	 (x+y-z- (list x+ y- z-))
	 (x-y+z+ (list x- y+ z+))
	 (x-y+z- (list x- y+ z-))
	 (x-y-z+ (list x- y- z+))
	 (x-y-z- (list x- y- z-)))
    (list (make-sphere x+y+z+ radius material)
	  (make-sphere x+y+z- radius material)
	  (make-sphere x+y-z+ radius material)
	  (make-sphere x+y-z- radius material)
	  (make-sphere x-y+z+ radius material)
	  (make-sphere x-y+z- radius material)
	  (make-sphere x-y-z+ radius material)
	  (make-sphere x-y-z- radius material)
	  ; Front
	  (make-cylinder x+y+z+ x+y-z+ radius material)
	  (make-cylinder x+y-z+ x-y-z+ radius material)
	  (make-cylinder x-y-z+ x-y+z+ radius material)
	  (make-cylinder x-y+z+ x+y+z+ radius material)
	  ; Back
	  (make-cylinder x+y+z- x+y-z- radius material)
	  (make-cylinder x+y-z- x-y-z- radius material)
	  (make-cylinder x-y-z- x-y+z- radius material)
	  (make-cylinder x-y+z- x+y+z- radius material)
	  ; Back to front
	  (make-cylinder x+y+z- x+y+z+ radius material)
	  (make-cylinder x+y-z- x+y-z+ radius material)
	  (make-cylinder x-y+z- x-y+z+ radius material)
	  (make-cylinder x-y-z- x-y-z+ radius material)
          ; Fill in boxes
          (make-solid-box (list xo- y- z-) (list xo+ y+ z+) material)
          (make-solid-box (list x- yo- z-) (list x+ yo+ z+) material)
	  (make-solid-box (list x- y- zo-) (list x+ y+ zo+) material))))

(define (make-solid-rounded-box lowercorner uppercorner radius material)
  ;; Makes a box with round edges. 
  ;; The radius of the round edges are 'radius'.
  ;; The elements are joined in a union so that the object
  ;; can be used in a CSG operation.
  (apply 
    make-union 
    (append 
      (make-rounded-box lowercorner uppercorner radius material) 
      (list material))))



 
