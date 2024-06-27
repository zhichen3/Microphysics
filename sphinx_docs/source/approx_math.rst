*************
Approx Math
*************

fast_atan
============
An implementation of a fast version of ``std::atan`` discussed in:
https://stackoverflow.com/questions/42537957/fast-accurate-atan-arctan-approximation-algorithm
with a maximum absolute error of ~0.00063 rad along with
Taylor approximation for :math:`|x| < 0.113`.

The following expression works well for :math:`-1 < x < 1`:

.. math::
   arctan(x) = Ax^5 + Bx^3 + Cx

where :math:`A = 0.0776509570923569`, :math:`B = -0.287434475393028`,
and :math:`C = \frac{\pi}{4} - A - B`.

For :math:`x > 1`, we use the identity:

.. math::
   arctan(x) = sign(x) \frac{\pi}{2} - arctan(\frac{1}{x})

Comparing to std::atan, fast_atan is roughly 3-4 times faster.


fast_exp
============

An implementation of a fast version of ``std::exp`` by manipulating
the components of the standard (IEEE-754) floating-point representation
well explained in :cite:`Schraudolph_1999` and :cite:`Cawley_2000`.

To briefly summarize, the main idea of this is to use the identity:

.. math::
   \Exp(y) = 2^{\frac{y}{\log(2)}}

and since floating numbers are represented as :math:`(-1)^s(1+m)2^{x - x_0}`,
where ``s`` is the sign bit, ``m`` is the mantissa bit,
``x`` is the exponent bit, and :math:`x_0` is the exponent bias.

.. note::
   Single-precision (32 bit) has 8 bits for exponent
   with :math:`x_0 = 127`, while double-precision (64 bit)
   has 11 bits for exponent with :math:`x_0 = 1023`.

Therefore, we simply require mantissa bits to be equal to 0, and by setting

.. math::
   x = \frac{y}{\log(2)} + x_0




We also use memcpy approach to avoid undefined behavior from type-punning
when using union, which we mainly follow
(https://gist.github.com/jrade/293a73f89dfef51da6522428c857802d).
