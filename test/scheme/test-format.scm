(load "../../scenes/lib/format.scm")

(define (test-format)
  (test "Passthrough" (equal? (format #f "Foo") "Foo"))
  (test "Format decimal" (equal? (format #f "Foo~dbar" 255) "Foo255bar"))
  (test "Format hex" (equal? (format #f "Foo~xbar" 255) "Fooffbar"))
  (test "Format octal" (equal? (format #f "Foo~obar" 16) "Foo20bar"))
  (test "Format binary" (equal? (format #f "Foo~bbar" 3) "Foo11bar"))
  (test "Format list" (equal? (format #f "Foo~abar" '(1 2)) "Foo(1 2)bar"))
  (test "Format char with a" (equal? (format #f "Foo~abar" #\x) "Fooxbar"))
  (test "Format char with c" (equal? (format #f "Foo~cbar" #\x) "Fooxbar"))
  (test "Escaped ~" (equal? (format #f "Foo~~bar") "Foo~bar"))
  (test "Escaped space" (equal? (format #f "Foo~_bar") "Foo bar"))
  (test "Escaped tab" (equal? (format #f "Foo~tbar") "Foo\tbar"))
  (test "Escaped newline" (equal? (format #f "Foo~%bar") "Foo\nbar"))
  (test "Embed string" (equal? (format #f "Foo~abar" "ss") "Foossbar"))
  (test "1 = 1" (= 1 1)))


(run-test "Format" test-format)