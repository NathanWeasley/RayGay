
(load "lib/raygay.scm")

(set-image-size '(1024 768))
(set-background #(0.3 0.6 0.7))

(set-renderer "raytracer")
(set-camera 
  (make-pinhole-camera 
    '( pos #(2 17 20)
       lookat #(0 0 0)
       up #(0 1 0)
       fov 45
       aa 0)))

(define green
  (make-material
    '( diffuse #(0.2 0.9 0.3)
       kd 0.8
       specular #(1.0 1.0 1.0)
       ks 0.0
       specpow 45)))

(add-to-scene (make-pointlight #(-500 1300 1300)))

(define materials
  (list 
     (make-material
       '( diffuse #(0.9 0.8 0.6)
          kd 1.0
          ks 0.0
          specpow 45))
      (make-material
        '( diffuse #(0.9 0.8 0.6)
           kd 1.0
           ks 0.0
           specpow 45))
       (make-material
         '( diffuse #(0.99 0.9 0.6)
            kd 1.0
            ks 0.0
            specpow 45))
))          
                
(define (make-jittered-box x z)
(translate
  (rotate-z
    (rotate-x
      (rotate-y        
        (make-rounded-box #(-0.5 -0.2 -0.5) #(0.5 0.2 0.5) 0.1 (pick-random-from-list materials))        
        (random2 -10 10))
      (random2 -5 5))
    (random2 -5 5))
  (vector x 0 z)))
 
(do ((x -5 (+ 1 x)))
  ((= x 5))
    (do ((z -5 (+ 1 z)))
      ((= z 5))
        (add-to-scene (make-jittered-box x z))))

; True case: just a single dragon
; False case: a load of them
(define many #t)

(if many
   (define dragon (make-bound (scale (make-ply-mesh "ply/dragon_vrip_res2.ply" green) #(12 12 12)))))

(if many
    (do ((i 0 (+ i 1)))
        ((= i 40))
        (add-to-scene 
             (translate
                 (rotate-y
                     (make-instance dragon (pick-random-from-list materials))
                     (random2 -90 90))
              (vector (random2 -4 4) -0.5 (random2 -4 4)))))

     (add-to-scene
         (translate
               (scale 
                       (make-ply-mesh "ply/dragon_vrip_res2.ply" green)          
              #(40 40 40))
          #(0 -2 1))))