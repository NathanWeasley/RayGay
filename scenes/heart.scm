
(load "globals.scm")
(load "objects.scm")

(define balls 10)
(define size 3)
(define radius (* 100 100 100))    

(set! image-size '(640 480))
(set! background '(0.94 0.7 0.06))

(set! renderer "raytracer")
(set! camera 
  (make-pinhole-camera 
    '( pos (0 600 1800)
       lookat (0 0 0)
       up (0 1 0)
       fov 45
       aa 4)))

(define ground
  (make-material
    '( diffuse (1.0 1.0 0.7)
       kd 0.9
       specular (1.0 1.0 1.0)
       ks 0.1
       specpow 15)))

(set! scene (list (make-pointlight '(500 500 1300))))

(append!
  scene (list (make-box '(-1000000 -500 -1000000) '(1000000 -400 1000000) ground)))


(let loop 
  ((i 0))
  (if (not(= i balls) )
    (let* ((x (random2 (* -1 size) size))
	   (y (random2 (* -1 size) size))
	   (z (random2 (* -1 size) size))
	   (v (- (expt (+ (* 2 x x) (* y y) (* z z) -1) 3)
		 (* x x z z z 0.1)
		 (* y y z z z))))
      (if (< v 0)
	(begin
	  (append scene (list 
			  (make-ellipsoid 
			    (list (* x 400) (* y 400) (* z 400))
			    (list (random2 10 30) (random2 10 30) (random2 10 30))
			    (make-material
			      (list 
				'diffuse
				(list (random2 0.9 1) (random2 0.3 0.5) (random2 0.3 0.35))
				'kd 0.8
				'specular (list 0.5 0.5 0.5)
				'ks 0.2
				'specpow 25)))))
	  (loop (+ i 1)))
	(loop i)))))


