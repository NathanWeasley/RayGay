
;; The teapot consist of 32 bezier patches.
(define (make-teapot xresolution yresolution material)
  (let loop 
    ((coords 
       '(
	 (-80 30 0) (-80 30 -44.80) (-44.80 30 -80) (0 30 -80)
	 (-80 12 0) (-80 12 -44.80) (-44.80 12 -80) (0 12 -80)
	 (-60 3 0) (-60 3 -33.60) (-33.60 3 -60) (0 3 -60)
	 (-60 0 0) (-60 0 -33.60) (-33.60 0 -60) (0 0 -60)

	 (0 30 -80) (44.80 30 -80) (80 30 -44.80) (80 30 0)
	 (0 12 -80) (44.80 12 -80) (80 12 -44.80) (80 12 0)
	 (0 3 -60) (33.60 3 -60) (60 3 -33.60) (60 3 0)
	 (0 0 -60) (33.60 0 -60) (60 0 -33.60) (60 0 0)

	 (-60 90 0) (-60 90 -33.60) (-33.60 90 -60) (0 90 -60)
	 (-70 69 0) (-70 69 -39.20) (-39.20 69 -70) (0 69 -70)
	 (-80 48 0) (-80 48 -44.80) (-44.80 48 -80) (0 48 -80)
	 (-80 30 0) (-80 30 -44.80) (-44.80 30 -80) (0 30 -80)

	 (0 90 -60) (33.60 90 -60) (60 90 -33.60) (60 90 0)
	 (0 69 -70) (39.20 69 -70) (70 69 -39.20) (70 69 0)
	 (0 48 -80) (44.80 48 -80) (80 48 -44.80) (80 48 0)
	 (0 30 -80) (44.80 30 -80) (80 30 -44.80) (80 30 0)

	 (-56 90 0) (-56 90 -31.36) (-31.36 90 -56) (0 90 -56)
	 (-53.50 95.25 0) (-53.50 95.25 -29.96) (-29.96 95.25 -53.50) (0 95.25 -53.50)
	 (-57.50 95.25 0) (-57.50 95.25 -32.20) (-32.20 95.25 -57.50) (0 95.25 -57.50)
	 (-60 90 0) (-60 90 -33.60) (-33.60 90 -60) (0 90 -60)

	 (0 90 -56) (31.36 90 -56) (56 90 -31.36) (56 90 0)
	 (0 95.25 -53.50) (29.96 95.25 -53.50) (53.50 95.25 -29.96) (53.50 95.25 0)
	 (0 95.25 -57.50) (32.20 95.25 -57.50) (57.50 95.25 -32.20) (57.50 95.25 0)
	 (0 90 -60) (33.60 90 -60) (60 90 -33.60) (60 90 0)

	 (80 30 0) (80 30 44.80) (44.80 30 80) (0 30 80)
	 (80 12 0) (80 12 44.80) (44.80 12 80) (0 12 80)
	 (60 3 0) (60 3 33.60) (33.60 3 60) (0 3 60)
	 (60 0 0) (60 0 33.60) (33.60 0 60) (0 0 60)

	 (0 30 80) (-44.80 30 80) (-80 30 44.80) (-80 30 0)
	 (0 12 80) (-44.80 12 80) (-80 12 44.80) (-80 12 0)
	 (0 3 60) (-33.60 3 60) (-60 3 33.60) (-60 3 0)
	 (0 0 60) (-33.60 0 60) (-60 0 33.60) (-60 0 0)

	 (60 90 0) (60 90 33.60) (33.60 90 60) (0 90 60)
	 (70 69 0) (70 69 39.20) (39.20 69 70) (0 69 70)
	 (80 48 0) (80 48 44.80) (44.80 48 80) (0 48 80)
	 (80 30 0) (80 30 44.80) (44.80 30 80) (0 30 80)

	 (0 90 60) (-33.60 90 60) (-60 90 33.60) (-60 90 0)
	 (0 69 70) (-39.20 69 70) (-70 69 39.20) (-70 69 0)
	 (0 48 80) (-44.80 48 80) (-80 48 44.80) (-80 48 0)
	 (0 30 80) (-44.80 30 80) (-80 30 44.80) (-80 30 0)

	 (56 90 0) (56 90 31.36) (31.36 90 56) (0 90 56)
	 (53.50 95.25 0) (53.50 95.25 29.96) (29.96 95.25 53.50) (0 95.25 53.50)
	 (57.50 95.25 0) (57.50 95.25 32.20) (32.20 95.25 57.50) (0 95.25 57.50)
	 (60 90 0) (60 90 33.60) (33.60 90 60) (0 90 60)

	 (0 90 56) (-31.36 90 56) (-56 90 31.36) (-56 90 0)
	 (0 95.25 53.50) (-29.96 95.25 53.50) (-53.50 95.25 29.96) (-53.50 95.25 0)
	 (0 95.25 57.50) (-32.20 95.25 57.50) (-57.50 95.25 32.20) (-57.50 95.25 0)
	 (0 90 60) (-33.60 90 60) (-60 90 33.60) (-60 90 0)

	 (-64 75 0) (-64 75 12) (-60 84 12) (-60 84 0)
	 (-92 75 0) (-92 75 12) (-100 84 12) (-100 84 0)
	 (-108 75 0) (-108 75 12) (-120 84 12) (-120 84 0)
	 (-108 66 0) (-108 66 12) (-120 66 12) (-120 66 0)

	 (-60 84 0) (-60 84 -12) (-64 75 -12) (-64 75 0)
	 (-100 84 0) (-100 84 -12) (-92 75 -12) (-92 75 0)
	 (-120 84 0) (-120 84 -12) (-108 75 -12) (-108 75 0)
	 (-120 66 0) (-120 66 -12) (-108 66 -12) (-108 66 0)

	 (-108 66 0) (-108 66 12) (-120 66 12) (-120 66 0)
	 (-108 57 0) (-108 57 12) (-120 48 12) (-120 48 0)
	 (-100 39 0) (-100 39 12) (-106 31.50 12) (-106 31.50 0)
	 (-80 30 0) (-80 30 12) (-76 18 12) (-76 18 0)

	 (-120 66 0) (-120 66 -12) (-108 66 -12) (-108 66 0)
	 (-120 48 0) (-120 48 -12) (-108 57 -12) (-108 57 0)
	 (-106 31.50 0) (-106 31.50 -12) (-100 39 -12) (-100 39 0)
	 (-76 18 0) (-76 18 -12) (-80 30 -12) (-80 30 0)

	 (68 51 0) (68 51 26.40) (68 18 26.40) (68 18 0)
	 (104 51 0) (104 51 26.40) (124 27 26.40) (124 27 0)
	 (92 78 0) (92 78 10) (96 75 10) (96 75 0)
	 (108 90 0) (108 90 10) (132 90 10) (132 90 0)

	 (68 18 0) (68 18 -26.40) (68 51 -26.40) (68 51 0)
	 (124 27 0) (124 27 -26.40) (104 51 -26.40) (104 51 0)
	 (96 75 0) (96 75 -10) (92 78 -10) (92 78 0)
	 (132 90 0) (132 90 -10) (108 90 -10) (108 90 0)

	 (108 90 0) (108 90 10) (132 90 10) (132 90 0)
	 (112 93 0) (112 93 10) (141 93.75 10) (141 93.75 0)
	 (116 93 0) (116 93 6) (138 94.50 6) (138 94.50 0)
	 (112 90 0) (112 90 6) (128 90 6) (128 90 0)

	 (132 90 0) (132 90 -10) (108 90 -10) (108 90 0)
	 (141 93.75 0) (141 93.75 -10) (112 93 -10) (112 93 0)
	 (138 94.50 0) (138 94.50 -6) (116 93 -6) (116 93 0)
	 (128 90 0) (128 90 -6) (112 90 -6) (112 90 0)

	 (50 90 0) (50 90 28) (28 90 50) (0 90 50)
	 (52 90 0) (52 90 29.12) (29.12 90 52) (0 90 52)
	 (54 90 0) (54 90 30.24) (30.24 90 54) (0 90 54)
	 (56 90 0) (56 90 31.36) (31.36 90 56) (0 90 56)

	 (0 90 50) (-28 90 50) (-50 90 28) (-50 90 0)
	 (0 90 52) (-29.12 90 52) (-52 90 29.12) (-52 90 0)
	 (0 90 54) (-30.24 90 54) (-54 90 30.24) (-54 90 0)
	 (0 90 56) (-31.36 90 56) (-56 90 31.36) (-56 90 0)

	 (-50 90 0) (-50 90 -28) (-28 90 -50) (0 90 -50)
	 (-52 90 0) (-52 90 -29.12) (-29.12 90 -52) (0 90 -52)
	 (-54 90 0) (-54 90 -30.24) (-30.24 90 -54) (0 90 -54)
	 (-56 90 0) (-56 90 -31.36) (-31.36 90 -56) (0 90 -56)

	 (0 90 -50) (28 90 -50) (50 90 -28) (50 90 0)
	 (0 90 -52) (29.12 90 -52) (52 90 -29.12) (52 90 0)
	 (0 90 -54) (30.24 90 -54) (54 90 -30.24) (54 90 0)
	 (0 90 -56) (31.36 90 -56) (56 90 -31.36) (56 90 0)

	 (8 102 0) (8 102 4.48) (4.48 102 8) (0 102 8)
	 (16 96 0) (16 96 8.96) (8.96 96 16) (0 96 16)
	 (52 96 0) (52 96 29.12) (29.12 96 52) (0 96 52)
	 (52 90 0) (52 90 29.12) (29.12 90 52) (0 90 52)

	 (0 102 8) (-4.48 102 8) (-8 102 4.48) (-8 102 0)
	 (0 96 16) (-8.96 96 16) (-16 96 8.96) (-16 96 0)
	 (0 96 52) (-29.12 96 52) (-52 96 29.12) (-52 96 0)
	 (0 90 52) (-29.12 90 52) (-52 90 29.12) (-52 90 0)

	 (-8 102 0) (-8 102 -4.48) (-4.48 102 -8) (0 102 -8)
	 (-16 96 0) (-16 96 -8.96) (-8.96 96 -16) (0 96 -16)
	 (-52 96 0) (-52 96 -29.12) (-29.12 96 -52) (0 96 -52)
	 (-52 90 0) (-52 90 -29.12) (-29.12 90 -52) (0 90 -52)

	 (0 102 -8) (4.48 102 -8) (8 102 -4.48) (8 102 0)
	 (0 96 -16) (8.96 96 -16) (16 96 -8.96) (16 96 0)
	 (0 96 -52) (29.12 96 -52) (52 96 -29.12) (52 96 0)
	 (0 90 -52) (29.12 90 -52) (52 90 -29.12) (52 90 0)

	 (0 120 0) (0 120 0) (0 120 0) (0 120 0)
	 (32 120 0) (32 120 18) (18 120 32) (0 120 32)
	 (0 108 0) (0 108 0) (0 108 0) (0 108 0)
	 (8 102 0) (8 102 4.48) (4.48 102 8) (0 102 8)

	 (0 120 0) (0 120 0) (0 120 0) (0 120 0)
	 (0 120 32) (-18 120 32) (-32 120 18) (-32 120 0)
	 (0 108 0) (0 108 0) (0 108 0) (0 108 0)
	 (0 102 8) (-4.48 102 8) (-8 102 4.48) (-8 102 0)

	 (0 120 0) (0 120 0) (0 120 0) (0 120 0)
	 (-32 120 0) (-32 120 -18) (-18 120 -32) (0 120 -32)
	 (0 108 0) (0 108 0) (0 108 0) (0 108 0)
	 (-8 102 0) (-8 102 -4.48) (-4.48 102 -8) (0 102 -8)

	 (0 120 0) (0 120 0) (0 120 0) (0 120 0)
	 (0 120 -32) (18 120 -32) (32 120 -18) (32 120 0)
	 (0 108 0) (0 108 0) (0 108 0) (0 108 0)
	 (0 102 -8) (4.48 102 -8) (8 102 -4.48) (8 102 0)
	 )))
    (if (null? coords)
      '()
      (cons
	(make-bezierpatch
	  (list-head coords 16)
	  xresolution yresolution material)
	(loop (list-tail coords 16))))
    ))

