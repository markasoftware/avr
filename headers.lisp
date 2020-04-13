(ql:quickload :alexandria)
(use-package :alexandria)

(defvar *newline* "
")

(defun symbol-name-snake (sym)
  (substitute #\_ #\- (symbol-name sym)))

(defun alist-to-c-header (alist)
  (apply #'concatenate 'string
         (loop for (name . value) in alist
            collect (concatenate 'string "#define " (symbol-name-snake name) " " (write-to-string value) *newline*))))


(defun gcd-euler (a b &optional (a-a-coef 1) (a-b-coef 0) (b-a-coef 0) (b-b-coef 1))
  "Find the GCD of a and b using Euler's method. Will return as multiple values
  the GCD of the numbers, then the coefficient for a, and then the coefficient
  for b, such that the sum of a and b after multiplication by their respective
  coefficients equals the GCD."
  (if (zerop b)
      (values a a-a-coef a-b-coef)
      (gcd-euler b (mod a b) b-a-coef b-b-coef
                 (- a-a-coef (* (floor a b) b-a-coef))
                 (- a-b-coef (* (floor a b) b-b-coef)))))

(defun lcm-fractional (a b)
  (/ (* a b) (gcd-euler a b)))

(defun discrete-resistor-headers (file r-div r1 r2)
  (let ((g1 (/ r1))
        (g2 (/ r2))) 
    (multiple-value-bind (gcd a-coef b-coef) (gcd-euler g1 g2)
      (write-string-into-file
       (alist-to-c-header
        `((r-divider . ,r-div)
          ;; G_GCD and G_LCM are over this
          (g-multiplier . ,(denominator gcd))
          (g-gcd . ,(numerator gcd))
          ;; g1*a-coef +g2*b-coef = gcd
          (g1-gcd-coef . ,a-coef)
          (g2-gcd-coef . ,b-coef)
          ;; g1*g1-lcm-coef = g2*g2-lcm-coef = lcm
          (g-lcm . ,(* (denominator gcd) (lcm-fractional g1 g2)))
          (g1-lcm-coef . ,(/ (lcm-fractional g1 g2) g1))
          (g2-lcm-coef . ,(/ (lcm-fractional g1 g2) g2))))
       file :if-exists :supersede))))
