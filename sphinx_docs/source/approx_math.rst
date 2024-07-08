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
   \arctan(x) = Ax^5 + Bx^3 + Cx

where :math:`A = 0.0776509570923569`, :math:`B = -0.287434475393028`,
and :math:`C = \frac{\pi}{4} - A - B`.

For :math:`x > 1`, we use the identity:

.. math::
   \arctan(x) = \text{sgn}(x) \frac{\pi}{2} - \arctan(\frac{1}{x})

Comparing to std::atan, fast_atan is roughly 3-4 times faster.


fast_exp
============
An implementation of a fast version of ``std::exp`` by manipulating
the components of the standard (IEEE-754) floating-point representation
well explained in :cite:`Schraudolph_1999` and :cite:`Cawley_2000`.

To briefly summarize, the main idea of this is to use the identity:

 .. math::
   \exp(y) = 2^{\frac{y}{\log(2)}}

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

To achieve that, we start by creating an integer representation and shift by
the number of mantissa bits (23 bits for single precision and
52 bits for double precision) by multiplying the entire expression
by :math:`2^{m}`. Lastly, we can add a constant shift to minimize the error.
The choice of the constant is discussed in :cite:`Schraudolph_1999`.
After obtaining the desired bit representation in integer, we use
``memcpy`` to copy bit values to a floating-point variable.
This ``memcpy`` approach is to avoid undefined behavior from type-punning
when using ``union``, which we mainly follow
(https://gist.github.com/jrade/293a73f89dfef51da6522428c857802d).
Both single and double precision version are implemented in ``jrade_exp``
Overall, this is roughly 4-5 times faster than ``std::exp``
with ~2% relative error.


``fast_exp`` combines ``jrade_exp`` with a simple Taylor approximation when
:math:`x < 0.1`. This has a minor performance hit, but much better accuracy
when :math:`x < 0.1`.


If one needs slighly better accuracy, ``ekmett_exp``
(https://github.com/ekmett/approximate/blob/master/cbits/fast.c)
uses the fact that

.. math::
   \exp(x) = \frac{\exp(x/2)}{\exp(-x/2)}

By using this identity, ``ekmett_exp`` is roughly 2.5 times faster than
``std::exp`` with a 0.1% relative error.


fast_log
==========

An implementation of ``fast_log`` is based on inverting the
process of ``jrade_exp`` as discussed in
(https://martin.ankerl.com/2007/10/04/optimized-pow-approximation-for-java-and-c-c/)

``fast_log`` is roughly 10 times faster than ``std::log`` with ~1% relative error.


fast_pow
==========

An implementation of ``fast_pow`` relies on using the identity:

.. math::
   x^y = \exp(\log(x) y)

Therefore, we can simply combine the process of ``jrade_exp`` and ``fast_log``
to formulate a ``fast_pow`` algorithm (https://martin.ankerl.com/2007/10/04/optimized-pow-approximation-for-java-and-c-c/).
This is the ``vfast_pow`` function, which is ~30 times faster than ``std::pow``.
However, this gives poor accuracy when exponent >> 1.

In order to improve the accuracy, we can split up the exponent into its integer
and the fraction part and solve them independently. To solve :math:`x^n`,
where ``n`` is an integer, we can use exponentiation by squaring. Then we simply
use ``vfast_pow`` when solving for the integer part.

``fast_pow`` uses this combination, achieving ~2% relative error and
~5-6 times faster compared to ``std::pow``.
