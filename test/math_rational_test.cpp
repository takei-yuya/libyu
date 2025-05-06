#include "yu/math/rational.hpp"

#include "yu/test.hpp"

class RationalTest : public yu::Test {
};

TEST(RationalTest, testDefaultConstructor) {
  yu::math::Rational r;
  EXPECT(true, ==, r.is_positive());
  EXPECT(0, ==, r.numerator());
  EXPECT(1, ==, r.denominator());
}

TEST(RationalTest, testUnsignedConstructor) {
  {
    yu::math::Rational r(3u, 4u);
    EXPECT(true, ==, r.is_positive());
    EXPECT(3u, ==, r.numerator());
    EXPECT(4u, ==, r.denominator());
  }

  {
    yu::math::Rational r(6u, 8u);
    EXPECT(true, ==, r.is_positive());
    EXPECT(3u, ==, r.numerator());
    EXPECT(4u, ==, r.denominator());
  }

  {
    yu::math::Rational r(6u);
    EXPECT(true, ==, r.is_positive());
    EXPECT(6u, ==, r.numerator());
    EXPECT(1u, ==, r.denominator());
  }
}

TEST(RationalTest, testSignedConstructor) {
  {
    yu::math::Rational r(3, 4);
    EXPECT(true, ==, r.is_positive());
    EXPECT(3u, ==, r.numerator());
    EXPECT(4u, ==, r.denominator());
  }

  {
    yu::math::Rational r(-3, 4);
    EXPECT(false, ==, r.is_positive());
    EXPECT(3u, ==, r.numerator());
    EXPECT(4u, ==, r.denominator());
  }

  {
    yu::math::Rational r(3, -4);
    EXPECT(false, ==, r.is_positive());
    EXPECT(3u, ==, r.numerator());
    EXPECT(4u, ==, r.denominator());
  }

  {
    yu::math::Rational r(-3, -4);
    EXPECT(true, ==, r.is_positive());
    EXPECT(3u, ==, r.numerator());
    EXPECT(4u, ==, r.denominator());
  }

  {
    yu::math::Rational r(-6, -8);
    EXPECT(true, ==, r.is_positive());
    EXPECT(3u, ==, r.numerator());
    EXPECT(4u, ==, r.denominator());
  }

  {
    yu::math::Rational r(6);
    EXPECT(true, ==, r.is_positive());
    EXPECT(6u, ==, r.numerator());
    EXPECT(1u, ==, r.denominator());
  }

  {
    yu::math::Rational r(-6);
    EXPECT(false, ==, r.is_positive());
    EXPECT(6u, ==, r.numerator());
    EXPECT(1u, ==, r.denominator());
  }
}

TEST(RationalTest, testTrinaryConstructor) {
  {
    yu::math::Rational r(true, 3u, 4u);
    EXPECT(true, ==, r.is_positive());
    EXPECT(3u, ==, r.numerator());
    EXPECT(4u, ==, r.denominator());
    EXPECT(false, ==, r.is_zero());
    EXPECT(true, ==, r.is_valid());
    EXPECT(false, ==, r.is_nan());
    EXPECT(false, ==, r.is_inf());
    EXPECT(false, ==, r.is_pos_inf());
    EXPECT(false, ==, r.is_neg_inf());
  }
  {
    yu::math::Rational r(false, 3u, 4u);
    EXPECT(false, ==, r.is_positive());
    EXPECT(3u, ==, r.numerator());
    EXPECT(4u, ==, r.denominator());
    EXPECT(false, ==, r.is_zero());
    EXPECT(true, ==, r.is_valid());
    EXPECT(false, ==, r.is_nan());
    EXPECT(false, ==, r.is_inf());
    EXPECT(false, ==, r.is_pos_inf());
    EXPECT(false, ==, r.is_neg_inf());
  }

  {  // +0/4 is normalized to +0/1
    yu::math::Rational r(true, 0u, 4u);
    EXPECT(true, ==, r.is_positive());
    EXPECT(0u, ==, r.numerator());
    EXPECT(1u, ==, r.denominator());
    EXPECT(true, ==, r.is_zero());
    EXPECT(true, ==, r.is_valid());
    EXPECT(false, ==, r.is_nan());
    EXPECT(false, ==, r.is_inf());
    EXPECT(false, ==, r.is_pos_inf());
    EXPECT(false, ==, r.is_neg_inf());
  }
  {  // -0/4 is normalized to +0/1
    yu::math::Rational r(false, 0u, 4u);
    EXPECT(true, ==, r.is_positive());
    EXPECT(0u, ==, r.numerator());
    EXPECT(1u, ==, r.denominator());
    EXPECT(true, ==, r.is_zero());
    EXPECT(true, ==, r.is_valid());
    EXPECT(false, ==, r.is_nan());
    EXPECT(false, ==, r.is_inf());
    EXPECT(false, ==, r.is_pos_inf());
    EXPECT(false, ==, r.is_neg_inf());
  }

  {  // +0/0 is normalized to +0/0 (NaN)
    yu::math::Rational r(true, 0u, 0u);
    EXPECT(true, ==, r.is_positive());
    EXPECT(0u, ==, r.numerator());
    EXPECT(0u, ==, r.denominator());
    EXPECT(false, ==, r.is_zero());
    EXPECT(false, ==, r.is_valid());
    EXPECT(true, ==, r.is_nan());
    EXPECT(false, ==, r.is_inf());
    EXPECT(false, ==, r.is_pos_inf());
    EXPECT(false, ==, r.is_neg_inf());
  }
  {  // -0/0 is normalized to +0/0 (NaN)
    yu::math::Rational r(false, 0u, 0u);
    EXPECT(true, ==, r.is_positive());
    EXPECT(0u, ==, r.numerator());
    EXPECT(0u, ==, r.denominator());
    EXPECT(false, ==, r.is_zero());
    EXPECT(false, ==, r.is_valid());
    EXPECT(true, ==, r.is_nan());
    EXPECT(false, ==, r.is_inf());
    EXPECT(false, ==, r.is_pos_inf());
    EXPECT(false, ==, r.is_neg_inf());
  }

  {  // +4/0 is normalized to +1/0 (+Inf)
    yu::math::Rational r(true, 4u, 0u);
    EXPECT(true, ==, r.is_positive());
    EXPECT(1u, ==, r.numerator());
    EXPECT(0u, ==, r.denominator());
    EXPECT(false, ==, r.is_zero());
    EXPECT(false, ==, r.is_valid());
    EXPECT(false, ==, r.is_nan());
    EXPECT(true, ==, r.is_inf());
    EXPECT(true, ==, r.is_pos_inf());
    EXPECT(false, ==, r.is_neg_inf());
  }
  {  // -4/0 is normalized to -1/0 (-Inf)
    yu::math::Rational r(false, 4u, 0u);
    EXPECT(false, ==, r.is_positive());
    EXPECT(1u, ==, r.numerator());
    EXPECT(0u, ==, r.denominator());
    EXPECT(false, ==, r.is_zero());
    EXPECT(false, ==, r.is_valid());
    EXPECT(false, ==, r.is_nan());
    EXPECT(true, ==, r.is_inf());
    EXPECT(false, ==, r.is_pos_inf());
    EXPECT(true, ==, r.is_neg_inf());
  }
}

TEST(RationalTest, testArithmetic) {
  {
    yu::math::Rational r1(3, 22);
    yu::math::Rational r2(5, 77);

    yu::math::Rational pos = +r1;
    EXPECT(true, ==, pos.is_positive());
    EXPECT(3u, ==, pos.numerator());
    EXPECT(22u, ==, pos.denominator());

    yu::math::Rational neg = -r1;
    EXPECT(false, ==, neg.is_positive());
    EXPECT(3u, ==, neg.numerator());
    EXPECT(22u, ==, neg.denominator());

    yu::math::Rational inv = r1.invert();
    EXPECT(true, ==, inv.is_positive());
    EXPECT(22u, ==, inv.numerator());
    EXPECT(3u, ==, inv.denominator());

    yu::math::Rational add = r1 + r2;
    EXPECT(true, ==, add.is_positive());
    EXPECT(31u, ==, add.numerator());
    EXPECT(154u, ==, add.denominator());

    yu::math::Rational sub = r1 - r2;
    EXPECT(true, ==, sub.is_positive());
    EXPECT(1u, ==, sub.numerator());
    EXPECT(14u, ==, sub.denominator());

    yu::math::Rational rsub = r2 - r1;
    EXPECT(false, ==, rsub.is_positive());
    EXPECT(1u, ==, rsub.numerator());
    EXPECT(14u, ==, rsub.denominator());

    yu::math::Rational mul = r1 * r2;
    EXPECT(true, ==, mul.is_positive());
    EXPECT(15u, ==, mul.numerator());
    EXPECT(1694u, ==, mul.denominator());

    yu::math::Rational div = r1 / r2;
    EXPECT(true, ==, div.is_positive());
    EXPECT(21u, ==, div.numerator());
    EXPECT(10u, ==, div.denominator());

    yu::math::Rational rdiv = r2 / r1;
    EXPECT(true, ==, rdiv.is_positive());
    EXPECT(10u, ==, rdiv.numerator());
    EXPECT(21u, ==, rdiv.denominator());
  }
  {
    yu::math::Rational r1(1, 2);
    yu::math::Rational sub = r1 - r1;
    EXPECT(true, ==, sub.is_positive());
    EXPECT(0u, ==, sub.numerator());
    EXPECT(1u, ==, sub.denominator());
  }
}

TEST(RationalTest, testArithmeticInfNaN) {
  yu::math::Rational zero = yu::math::Rational::Zero();
  yu::math::Rational pos_one = yu::math::Rational(1);
  yu::math::Rational neg_one = yu::math::Rational(-1);
  yu::math::Rational pos_inf = yu::math::Rational::PosInf();
  yu::math::Rational neg_inf = yu::math::Rational::NegInf();
  yu::math::Rational nan = yu::math::Rational::NaN();

  yu::math::Rational pos_two = yu::math::Rational(2);
  yu::math::Rational neg_two = yu::math::Rational(-2);

  std::vector<yu::math::Rational> rationals = {
      zero,     pos_one,  neg_one,  pos_inf,  neg_inf,  nan,
  };

  // a + b ||    0 |    1 |   -1 | +Inf | -Inf | NaN
  // ------++------+------+------+------+------+-----
  // 0     ||    0 |    1 |   -1 | +Inf | -Inf | NaN
  // 1     ||    1 |    2 |    0 | +Inf | -Inf | NaN
  // -1    ||   -1 |    0 |   -2 | +Inf | -Inf | NaN
  // +Inf  || +Inf | +Inf | +Inf | +Inf |  NaN | NaN
  // -Inf  || -Inf | -Inf | -Inf |  NaN | -Inf | NaN
  // NaN   ||  NaN |  NaN |  NaN |  NaN |  NaN | NaN
  std::vector<std::vector<yu::math::Rational>> add_table = {
      {zero,    pos_one,  neg_one,  pos_inf,  neg_inf,  nan},
      {pos_one, pos_two,  zero,     pos_inf,  neg_inf,  nan},
      {neg_one, zero,     neg_two,  pos_inf,  neg_inf,  nan},
      {pos_inf, pos_inf,  pos_inf,  pos_inf,  nan,      nan},
      {neg_inf, neg_inf,  neg_inf,  nan,      neg_inf,  nan},
      {nan,     nan,      nan,      nan,      nan,      nan},
  };

  // a - b ||    0 |    1 |   -1 | +Inf | -Inf | NaN
  // ------++------+------+------+------+------+-----
  // 0     ||    0 |   -1 |    1 | -Inf | +Inf | NaN
  // 1     ||    1 |    0 |    2 | -Inf | +Inf | NaN
  // -1    ||   -1 |   -2 |    0 | -Inf | +Inf | NaN
  // +Inf  || +Inf | +Inf | +Inf |  NaN | +Inf | NaN
  // -Inf  || -Inf | -Inf | -Inf | -Inf |  NaN | NaN
  // NaN   ||  NaN |  NaN |  NaN |  NaN |  NaN | NaN
  std::vector<std::vector<yu::math::Rational>> sub_table = {
      {zero,    neg_one,  pos_one,  neg_inf,  pos_inf,  nan},
      {pos_one, zero,     pos_two,  neg_inf,  pos_inf,  nan},
      {neg_one, neg_two,  zero,     neg_inf,  pos_inf,  nan},
      {pos_inf, pos_inf,  pos_inf,  nan,      pos_inf,  nan},
      {neg_inf, neg_inf,  neg_inf,  neg_inf,  nan,      nan},
      {nan,     nan,      nan,      nan,      nan,      nan},
  };

  // a * b ||    0 |    1 |   -1 | +Inf | -Inf | NaN
  // ------++------+------+------+------+------+-----
  // 0     ||    0 |    0 |    0 |  NaN |  NaN | NaN
  // 1     ||    0 |    1 |   -1 | +Inf | -Inf | NaN
  // -1    ||    0 |   -1 |    1 | -Inf | +Inf | NaN
  // +Inf  ||  NaN | +Inf | -Inf | +Inf | -Inf | NaN
  // -Inf  ||  NaN | -Inf | +Inf | -Inf | +Inf | NaN
  // NaN   ||  NaN |  NaN |  NaN |  NaN |  NaN | NaN
  std::vector<std::vector<yu::math::Rational>> mul_table = {
      {zero,    zero,     zero,     nan,      nan,      nan},
      {zero,    pos_one,  neg_one,  pos_inf,  neg_inf,  nan},
      {zero,    neg_one,  pos_one,  neg_inf,  pos_inf,  nan},
      {nan,     pos_inf,  neg_inf,  pos_inf,  neg_inf,  nan},
      {nan,     neg_inf,  pos_inf,  neg_inf,  pos_inf,  nan},
      {nan,     nan,      nan,      nan,      nan,      nan},
  };

  // a / b ||    0 |    1 |   -1 | +Inf | -Inf | NaN
  // ------++------+------+------+------+------+-----
  // 0     ||  NaN |    0 |    0 |    0 |    0 | NaN
  // 1     || +Inf |    1 |   -1 |    0 |    0 | NaN
  // -1    || -Inf |   -1 |    1 |    0 |    0 | NaN
  // +Inf  || +Inf | +Inf | -Inf |  NaN |  NaN | NaN
  // -Inf  || -Inf | -Inf | +Inf |  NaN |  NaN | NaN
  // NaN   ||  NaN |  NaN |  NaN |  NaN |  NaN | NaN
  std::vector<std::vector<yu::math::Rational>> div_table = {
      {nan,     zero,     zero,     zero,     zero,     nan},
      {pos_inf, pos_one,  neg_one,  zero,     zero,     nan},
      {neg_inf, neg_one,  pos_one,  zero,     zero,     nan},
      {pos_inf, pos_inf,  neg_inf,  nan,      nan,      nan},
      {neg_inf, neg_inf,  pos_inf,  nan,      nan,      nan},
      {nan,     nan,      nan,      nan,      nan,      nan},
  };

  for (size_t i = 0; i < rationals.size(); ++i) {
    for (size_t j = 0; j < rationals.size(); ++j) {
      yu::math::Rational r1 = rationals[i];
      yu::math::Rational r2 = rationals[j];
      EXPECT(add_table[i][j], ==, r1 + r2);
      EXPECT(sub_table[i][j], ==, r1 - r2);
      EXPECT(mul_table[i][j], ==, r1 * r2);
      EXPECT(div_table[i][j], ==, r1 / r2);
    }
  }

  // negate
  EXPECT(zero, ==, -zero);
  EXPECT(neg_one, ==, -pos_one);
  EXPECT(pos_one, ==, -neg_one);
  EXPECT(neg_inf, ==, -pos_inf);
  EXPECT(pos_inf, ==, -neg_inf);
  EXPECT(nan, ==, -nan);

  // invert
  EXPECT(pos_inf, ==, zero.invert());
  EXPECT(pos_one, ==, pos_one.invert());
  EXPECT(neg_one, ==, neg_one.invert());
  EXPECT(zero, ==, pos_inf.invert());
  EXPECT(zero, ==, neg_inf.invert());
  EXPECT(nan, ==, nan.invert());
}

TEST(RationalTest, testComparison) {
  {
    yu::math::Rational r1(3, 22);
    yu::math::Rational r2(5, 77);

    EXPECT(false, ==, r1 == r2);
    EXPECT(true, ==, r1 != r2);
    EXPECT(false, ==, r1 < r2);
    EXPECT(false, ==, r1 <= r2);
    EXPECT(true, ==, r1 > r2);
    EXPECT(true, ==, r1 >= r2);

    EXPECT(true, ==, r1 == r1);
    EXPECT(false, ==, r1 != r1);
    EXPECT(false, ==, r1 < r1);
    EXPECT(true, ==, r1 <= r1);
    EXPECT(false, ==, r1 > r1);
    EXPECT(true, ==, r1 >= r1);
  }
}

template <typename T>
bool near_equal(T a, T b, T epsilon = static_cast<T>(1e-6)) {
  return std::abs(a - b) < epsilon;
}

TEST(RationalTest, testConversion) {
  {
    yu::math::Rational r1(-3, 2);
    EXPECT(true, ==, near_equal(static_cast<long double>(r1), -1.5l));
    EXPECT(true, ==, near_equal(static_cast<double>(r1), -1.5));
    EXPECT(true, ==, near_equal(static_cast<float>(r1), -1.5f));
  }
}
