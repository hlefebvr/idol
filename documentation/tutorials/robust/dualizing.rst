.. _dualizing:

Dualization
===========

Dualization plays a prominent role in robust optimization. In this tutorial we show how one can derive the dual problem
of any convex quadratic problem. This is done both from the theoretical and computational side. In the first section,
the mathematical expression of the dual is derived while we show how it can automatically be obtained by using Idol.

Dual of a convex quadratic problem
----------------------------------

Recall that Idol considers convex problems with quadratic expressions of the following form:

.. math::

    \begin{array}{lll}
        \textrm{minimize } & \displaystyle \sum_{j=1}^n c_jx_j + \sum_{j=1}^n\sum_{k=1}^n d_{jk}x_jx_k \\
        \textrm{subject to } & \displaystyle \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q^i_{jk}x_jx_k \le b_i & i=1,...,m \\
        & l_j \le x_j \le u_j & j=1,...,m \\
        & x_j\in\mathbb Z & j\in J_I,
    \end{array}

where it is assumed that :math:`C^i\cap[l,u]` are convex sets, with

.. math::

    C^i = \{ x\in\mathbb R^n : \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q^i_{jk}x_jx_k \le b_i \}`

and :math:`i=1,...,m`, and that the objective function :math:`f(x) = \sum_{j=1}^n c_jx_j + \sum_{j=1}^n\sum_{k=1}^n d_{jk}x_jx_k`
is convex over :math:`\mathbb R^n`. The bounds :math:`l_j` and :math:`u_j` (with :math:`j=1,...,n`) belong to the extended real number line.

We start by re-writing our problem in term of indicator functions. We recall here that the indicator function of a set :math:`C`
is noted :math:`\delta(\bullet|C)` and is defined as

.. math::

    \delta(x|C) = \begin{cases}
        0 & \textrm{if } x\in C \\
        +\infty & \textrm{otherwise}
    \end{cases}.

Our (*primal*) problem reads

.. math::

    \min_{x\in\mathbb R^n} f(x) + \sum_{i=1}^m\delta(x|C^i) + \delta(x|[l,u]).

As long as :math:`f` and :math:`C^i` are convex, this problem is convex and, assuming constraint qualification holds, the following
equality holds (see `Fenchel duality <https://en.wikipedia.org/wiki/Fenchel%27s_duality_theorem>`_).

.. math::

    \begin{multline}
    \min_{x\in\mathbb R^n} \sum_{i=1}^m\delta(x|C^i) + \delta(x|[l,u]) - (-f)(x) \\
     =
    \max_{\pi\in\mathbb R^n} (-f)_*(\pi) - \left( \sum_{i=1}^m\delta(\bullet|C^i) + \delta(\bullet|[l,u]) \right)^*(\pi)
    \end{multline}

Refering to this `cheatsheet on convex conjugates <https://hlefebvr.github.io/math/2022/06/22/conjugate.html>`_,
one sees that :math:`(-f)_*(\pi) = -f^*(-\pi)`. Moreover, refering to the "Non-separable sum" formula, one obtains the followig problem:

.. math::

    \begin{array}{lll}
            \textrm{maximize } & -f^*(-\pi) - \sum_{i=1}^m \delta^*(u^i|C^i) - \delta^*(v|[l,u]) \\
            \textrm{subject to } & v + \sum_{i=1}^m u^i = \pi \\
            & \pi\in\mathbb R^n
    \end{array}

Conjugate of the indicator function of bound restrictions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We start by computing :math:`\delta^*(v|[l,u])`. By definition, this equals

.. math::

    \delta^*(v|[l,u]) =
    \begin{array}[t]{lll}
        \max\ & v^\top x \\
        \textrm{s.t. } & x \le u & (\lambda^u \ge 0) \\
        & -x \le -l & (\lambda^l \ge 0) \\
        & x\in \mathbb R^n.
    \end{array}

By standard LP duality, one has

.. math::

    \begin{array}{lll}
        \min\ & u^\top\lambda^u - l^\top\lambda^l \\
        \textrm{s.t. } & \lambda^u - \lambda^l = v & (x\in\mathbb R^n)\\
        & \lambda^u \ge 0 \\
        & \lambda^l \ge 0.
    \end{array}

We end with some useful remarks:

- Assume :math:`l_j = -\infty` for some j, then one should remove :math:`\lambda^l_j` from this model;
- Assume :math:`u_j = +\infty` for some j, then one should remove :math:`\lambda^u_j` from this model;

Conjugate of convex sets defined by quadratic expressions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We may now take interest in :math:`\delta^*(u^i|C^i)`. Now, observe that there must
exist a symmetric matrix :math:`\tilde Q^i` such that

.. math::

    \sum_{j=1}^n a_{ij}x_j + \sum_{j=1}^n\sum_{k=1}^n q^i_{jk}x_jx_k - b_i
    =
    a_{(i)}x + x^\top \tilde Q^ix - b_i

Since :math:`\tilde Q^i` is a real symmetric matrix, it is diagonalizable by orthogonal matrices. Thus, there exists matrices :math:`P^i` and :math:`D^i` such that
:math:`\tilde Q^i = {P^i}^\top D^iP^i` where :math:`D^i` is diagonal.

There, idol considers two cases:

- If :math:`D^i\succeq 0`, then :math:`C^i` is a convex set and the following holds

    .. math::

        \begin{align}
            \delta^*(u^i|C^i) &=
            \begin{array}[t]{lll}
                \max\ & {u^i}^\top x \\
                \textrm{s.t. } & a_{(i)}x + x^\top P^iD^i{P^i}^\top x \le b_i \\
                & x\in\mathbb R^n
            \end{array}  \\
            &=
            \begin{array}[t]{lll}
                \max\ & {u^i}^\top x \\
                \textrm{s.t. } & || {D^i}^{\frac 12}{P^i}^\top x ||_2^2 \le b_i - a_{(i)} x \\
                & x\in\mathbb R^n
            \end{array}
            \\
            &=
            \begin{array}[t]{lll}
                \max\ & {u^i}^\top x \\
                \textrm{s.t. } & (\frac 12, b_i - a_{(i)} x, {D^i}^{\frac 12}{P^i}^\top x ) \in \mathcal Q_r^{n+2} \\
                & x\in\mathbb R^n
            \end{array}
        \end{align}

  where :math:`\mathcal Q_r^n` denotes the :math:`n`-dimensional quadratic rotated cone.
  By conic duality (see e.g., `here <https://hlefebvr.github.io/math/2023/03/02/conic-duality.html>`_), we obtain

    .. math::

        \begin{array}{lll}
            \min\ & \frac 12\pi_1 + b_i\pi_2 \\
            \textrm{s.t. } & -a_{(i)}^\top\pi_2 + P^i{D^i}^{\frac 12}\lambda = - u^i \\
            & \sum_{j=1}^n \lambda_i^2 \le \pi_1\pi_2 \\
            & \pi_1, \pi_2 \ge 0
        \end{array}

  In the special case where :math:`D^i = 0`, :math:`\pi_1` can be fixed to 0 and one recovers LP duality.

- Otherwise, Idol expects :math:`\tilde Q^i` to have one negative eigen value, :math:`a = 0` and :math:`b_i = 0`.
  We have,

  .. math::

    \tilde Q^i = PD^iP^\top = d_{kk}p^{(k)}{p^{(k)}}^\top + \sum_{j\neq k} d_{jj}p^{(j)}{p^{(j)}}^\top

  with :math:`d_{kk} < 0` and :math:`d_{jj} \ge 0` (with :math:`j\neq k`). For simplicity, we will assume that :math:`k = 1`.
  Then, constraint :math:`x^\top\tilde Q^ix \le 0` can written as

  .. math::

    \sum_{j=2}^n d_{jj}( {p^{(j)}}^\top x )^2 \le d_{11}( {p^{(1)}}^\top x )^2

  which can be expressed with a quadratic cone provided that :math:`{p^{(1)}}^\top x \ge 0` over :math:`[l,u]`. We obtain

  .. math::

    (\sqrt{-d_{11}} {p^{(1)}}^\top x, \sqrt{d_{22}} {p^{(2)}}^\top x, \sqrt{d_{33}} {p^{(3)}}^\top x, ..., \sqrt{d_{nn}} {p^{(n)}}^\top x) \in \mathcal Q^{n}

  where :math:`\mathcal Q^n` denotes the :math:`n`-dimensional quadratic cone. Finally, we obtain the following result.

    .. math::

          \begin{align}
              \delta^*(u^i|C^i) &=
              \begin{array}[t]{lll}
                  \max\ & {u^i}^\top x \\
                  \textrm{s.t. } & x^\top P^iD^i{P^i}^\top x \le 0 \\
                  & x\in\mathbb R^n
              \end{array}  \\
              &=
              \begin{array}[t]{lll}
                  \max\ & {u^i}^\top x \\
                  \textrm{s.t. } & (\sqrt{-d_{11}} {p^{(1)}}^\top x, \sqrt{d_{22}} {p^{(2)}}^\top x, \sqrt{d_{33}} {p^{(3)}}^\top x, ..., \sqrt{d_{nn}} {p^{(n)}}^\top x) \in \mathcal Q^{n} \\
                  & {p^{(1)}}^\top x \ge 0 \\
                  & x\in\mathbb R^n
              \end{array}
              \\
              &=
              \begin{array}[t]{lll}
                  \min\ & 0 \\
                  \textrm{s.t. } & \sqrt{-d_{11}}p^{(1)}(\lambda_1 + \pi) + \sum_{j=2}^n \sqrt{ d_{jj} }p^{(j)}\lambda_j = -u^{i} \\
                  & \sum_{j=2}^n \lambda_j^2 \le \lambda_1 \\
                  & \pi \ge 0
              \end{array}
          \end{align}

Conjugate of the objective function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The conjugate function of :math:`f` is well known to equal to

.. math::

    f^*(\pi) =
    \begin{array}[t]{lll}
        \min\ & \frac 14 (\pi - c)^\top \tilde D^{\dagger} (\pi - c) \\
        \textrm{s.t. } & \pi - b \in \textrm{span}(\tilde D)
    \end{array}

where :math:`\tilde D` is the symmetric matrix associated to :math:`f`.

Final result
^^^^^^^^^^^^

For now, we may assume that all :math:`C^i` are convex sets.

.. math::

    \begin{array}{lll}
        \max\ & \frac 14 (\mu + c)^\top\tilde D^{-1}(\mu + c) + \sum_{i=1}^m(-\frac 12\pi_1^i - b_i\pi_2^i) - u^\top\lambda^u + l^\top\lambda^l \\
        \textrm{s.t. } & \lambda^u - \lambda^l + \sum_{i=1}^m ( a_{(i)}^\top\pi_2^i - P^i{D^i}^{\frac 12}\lambda^i ) = \mu \\
        & \sum_{j=1}^n \lambda_j^i \le \pi_1^i\pi_2^i & i=1,...,m \\
        & \lambda^u, \lambda^l \ge 0 \\
        & \pi_1^i, \pi_2^i \ge 0 & i=1,...,m
    \end{array}
