/*
Copyright (C) 2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Serhii Iliukhin
*/

#ifndef sixit_core_units_h_included
#define sixit_core_units_h_included

#include "sixit/core/lwa.h"

namespace sixit::units {

struct physical_dimension
{ 
    int T = 0; // time
    int L = 0; // length
    int M = 0; // mass
    int I = 0; // electric current
    int Theta = 0; // temperature
    int N = 0; // amount of substance
    int J = 0; // luminous intensity
    constexpr bool operator == (const physical_dimension& other) const { 
        return 
            T == other.T &&
            L == other.L &&
            M == other.M &&
            I == other.I &&
            Theta == other.Theta &&
            N == other.N &&
            J == other.J;
    }
    constexpr bool operator != (const physical_dimension& other) const {
        return !(*this == other);
    }
    constexpr bool is_dimensionless() const {
        return *this == physical_dimension({ 0,0,0,0,0,0,0 });
    }
    constexpr physical_dimension operator * (const physical_dimension& other) const {
        return physical_dimension{
            T + other.T,
            L + other.L,
            M + other.M,
            I + other.I,
            Theta + other.Theta,
            N + other.N,
            J + other.J };
    }
    constexpr physical_dimension operator / (const physical_dimension& other) const {
        return physical_dimension{
            T - other.T,
            L - other.L,
            M - other.M,
            I - other.I,
            Theta - other.Theta,
            N - other.N,
            J - other.J };
    }
};

template<physical_dimension dim>
constexpr physical_dimension sqrt() {
    static_assert(!(dim.T & 1), "dim_ have odd dimensions");
    static_assert(!(dim.L & 1), "dim_ have odd dimensions");
    static_assert(!(dim.M & 1), "dim_ have odd dimensions");
    static_assert(!(dim.I & 1), "dim_ have odd dimensions");
    static_assert(!(dim.Theta & 1), "dim_ have odd dimensions");
    static_assert(!(dim.N & 1), "dim_ have odd dimensions");
    static_assert(!(dim.J & 1), "dim_ have odd dimensions");
    return physical_dimension{
        dim.T >> 1,
        dim.L >> 1,
        dim.M >> 1,
        dim.I >> 1,
        dim.Theta >> 1,
        dim.N >> 1,
        dim.J >> 1 };
}

template<class fp, physical_dimension dim_> 
class dimensional_scalar;

} // namespace sixit::units

namespace sixit::dmath::test_helpers {
    template <typename fp, sixit::units::physical_dimension dim>
	inline bool approximate_eq(sixit::units::dimensional_scalar<fp, dim> left, sixit::units::dimensional_scalar<fp, dim> right, [[maybe_unused]] size_t n = 1);
}

namespace sixit::geometry {
    template <typename fp>
    struct ellitical_arc_parameters;

    template<class fp> class meters; // revise necessity
    template<class fp> class centimeters; // revise necessity
    template<class fp> class inches; // revise necessity
}

namespace sixit::geometry::gpu {

template <typename fp>
struct /*alignas(16)*/ vec4base;

}

namespace sixit::geometry::low_level
{
    template <typename fp>
    struct complex_number_rotation;
 
    template <typename fp>
    struct vector2;

    template <typename fp, sixit::units::physical_dimension dim>
    struct dimensional_vector2;

    template <typename fp, sixit::units::physical_dimension dim>
    struct dimensional_vector3;

    template <typename fp, sixit::units::physical_dimension dim>
    struct dimensional_quaternion;
}

namespace sixit::dmath::mathf {
    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> abs(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> min(sixit::units::dimensional_scalar<fp, dim_> a, sixit::units::dimensional_scalar<fp, dim_> b);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> max(sixit::units::dimensional_scalar<fp, dim_> a, sixit::units::dimensional_scalar<fp, dim_> b);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> round(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_ / dim_> atan2(sixit::units::dimensional_scalar<fp, dim_> a, sixit::units::dimensional_scalar<fp, dim_> b);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline auto sqrt(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline int sign(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline bool isfinite(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline bool isnan(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> floor(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> sin(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> cos(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> tan(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> asin(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> acos(sixit::units::dimensional_scalar<fp, dim_> val); 

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> atan(sixit::units::dimensional_scalar<fp, dim_> val);        

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> trunc(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> ceil(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> exp(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> log(sixit::units::dimensional_scalar<fp, dim_> val);

    template <typename fp, sixit::units::physical_dimension dim_> 
    inline sixit::units::dimensional_scalar<fp, dim_> log10(sixit::units::dimensional_scalar<fp, dim_> val);
}

namespace sixit::geometry::low_level::mathf {
        template<typename fp>
    inline fp make_constant_from(float f);

    template<typename fp, sixit::lwa::floating_const_helper f>
    inline constexpr auto make_constant_from();
    template <typename fp>
    inline fp clamp01(fp val);
    
    template <typename fp, sixit::units::physical_dimension dim> 
    int fp2int(sixit::units::dimensional_scalar<fp, dim> val);
}

namespace sixit::units {

/* PARTICULAR NAMED UNITS */

struct unit_base {};

struct simple_scalar : public unit_base 
{
    static constexpr physical_dimension dim{0,0,0,0,0,0,0};
    static constexpr float to_base = 1.f;
};

struct length_unit : public unit_base
{
    static constexpr physical_dimension dim{0,1,0,0,0,0,0};
};
struct meter : public length_unit { static constexpr float to_base = 1.f; };
struct millimeter : public length_unit { static constexpr float to_base = 0.001f; };
// etc.
template<class T>
constexpr bool is_length_v = T::physical_dimension == length_unit::dim;


struct time_unit : public unit_base
{
    static constexpr physical_dimension dim{1,0,0,0,0,0,0};
};
struct second : public time_unit { static constexpr  float to_base = 1.f; };
struct hour : public time_unit { static constexpr  float to_base = 3600.f; };
// etc.
template<class T>
constexpr bool is_time_v = T::physical_dimension == time_unit::dim;


struct mass_unit : public unit_base
{
    static constexpr physical_dimension dim{0,0,1,0,0,0,0};
};
struct kilogram : public mass_unit { static constexpr  float to_base = 1.f; };
struct gram : public mass_unit { static constexpr  float to_base = 0.001f; };
// etc.
template<class T>
constexpr bool is_mass_v = T::physical_dimension == mass_unit::dim;

// TODO: add the rest


// composite units

struct area_unit : public unit_base
{
    static constexpr physical_dimension dim{0,2,0,0,0,0,0};
};
struct square_meter : public area_unit { static constexpr float to_base = 1.f; };
struct square_millimeter : public area_unit { static constexpr float to_base = 0.000001f; };
// etc.
template<class T>
constexpr bool is_area_v = T::physical_dimension == area_unit::dim;


struct force_unit : public unit_base
{
    static constexpr physical_dimension dim{-2,1,1,0,0,0,0};
};
struct newton : public force_unit { static constexpr float to_base = 1.f; };
// etc.
template<class T>
constexpr bool is_force_v = T::physical_dimension == force_unit::dim;


/* dimensional scalar */



template<class fp>
struct dimensional_scalar_rw_alias_helper // means for adding OPTIONAL support of sixit rw library: specializations should provide conversion details
{
    // values allowing to detect that an fp type does not provide any specialization (see usage below)
    using value_type = void;
    using alias_type = void;
};

struct dimensional_scalar_base {};

struct internal_constructor_of_dimensional_scalar_from_fp {};

template<class fp_, physical_dimension dim_>
class dimensional_scalar : public dimensional_scalar_base// : public dimensional_scalar_fp_special<fp, dim_>
{
  public:
    using fp = fp_;
    static constexpr physical_dimension dim = dim_;

  private:
    fp value;

    /**/template<typename fp1, sixit::lwa::floating_const_helper f>
    friend constexpr auto sixit::geometry::low_level::mathf::make_constant_from(); // revise
    template<typename fp1>
    friend fp1 sixit::geometry::low_level::mathf::make_constant_from(float f); // revise

    template <typename fp1>
    friend dimensional_scalar<fp1, simple_scalar::dim> create_dimensionless_scalar(fp1 val);
    template <typename fp1>
    friend fp1 extract_dim_less_scalar(dimensional_scalar<fp1, simple_scalar::dim> val);

    template<class fp1, physical_dimension dim1_>
    friend class dimensional_scalar;

    template<class fp1, class UnitT>
    friend dimensional_scalar<fp1, UnitT::dim> for_import_only_make_dimensional_scalar(fp1);
    template<class UnitT, class ScalarT>
    friend typename ScalarT::fp for_export_only_extract_fp_from_dimensional_scalar(ScalarT);

    template <typename fp1>
    friend struct sixit::geometry::ellitical_arc_parameters;

    template <typename fp1>
    friend struct sixit::geometry::low_level::vector2;

    template <typename fp1, physical_dimension dim1>
    friend struct sixit::geometry::low_level::dimensional_vector2;
    template <typename fp1, physical_dimension dim1>
    friend struct sixit::geometry::low_level::dimensional_vector3;
    template <typename fp1, sixit::units::physical_dimension dim1>
    friend struct sixit::geometry::low_level::dimensional_quaternion;

    template <typename fp1>
    friend struct sixit::geometry::low_level::complex_number_rotation;

    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::abs(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend auto sixit::dmath::mathf::sqrt(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend int sixit::dmath::mathf::sign(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend bool sixit::dmath::mathf::isfinite(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend bool sixit::dmath::mathf::isnan(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::floor(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::sin(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::cos(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::tan(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::ceil(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::asin(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::acos(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::atan(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::trunc(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::exp(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::log(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::log10(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::min(dimensional_scalar<fp1, dim1_> a, dimensional_scalar<fp1, dim1_> b);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::max(dimensional_scalar<fp1, dim1_> a, dimensional_scalar<fp1, dim1_> b);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_> sixit::dmath::mathf::round(dimensional_scalar<fp1, dim1_> val);
    template <typename fp1, physical_dimension dim1_> friend dimensional_scalar<fp1, dim1_ / dim1_> sixit::dmath::mathf::atan2(dimensional_scalar<fp1, dim1_> a, dimensional_scalar<fp1, dim1_> b);

    template <typename fp1, physical_dimension dim1_> friend int sixit::geometry::low_level::mathf::fp2int(sixit::units::dimensional_scalar<fp1, dim1_> val);
    template <typename fp1> friend fp1 sixit::geometry::low_level::mathf::clamp01(fp1 val);
    template <typename fp1, sixit::units::physical_dimension dim1>
	friend bool sixit::dmath::test_helpers::approximate_eq(sixit::units::dimensional_scalar<fp1, dim1> left, sixit::units::dimensional_scalar<fp1, dim1> right, size_t n);

    template <typename fp1>
    friend struct sixit::geometry::gpu::vec4base;

    template<class fp> friend class sixit::geometry::meters; // revise necessity
    template<class fp> friend class sixit::geometry::centimeters; // revise necessity
    template<class fp> friend class sixit::geometry::inches; // revise necessity

    // special ctor for friend classes to initialize non-dimensionless scalars with fp
    constexpr dimensional_scalar(fp v, internal_constructor_of_dimensional_scalar_from_fp) : value(v) {}

public:
    constexpr dimensional_scalar() {}

    /*constexpr dimensional_scalar(const dimensional_scalar& other) = default;
    constexpr dimensional_scalar& operator = (const dimensional_scalar& other) = default;
    constexpr dimensional_scalar(dimensional_scala<fp, dim>r&& other) = default;
    constexpr dimensional_scalar& operator = (dimensional_scalar&& other) = default;*/

    /*template<class fp_same>
    constexpr dimensional_scalar(fp_same f) {
        static_assert(std::is_same_v<fp, fp_same>);
        static_assert(dim.is_dimensionless(), "only dimensionless dimensional_scalar can be created from FP");
        value = f;
    }*/

    template<physical_dimension dim_other>
    auto operator + (dimensional_scalar<fp, dim_other> other) const
    {
        static_assert(dim == dim_other, "cannot add values with different dimensions");
        return dimensional_scalar<fp, dim>({ value + other.value, internal_constructor_of_dimensional_scalar_from_fp() });
    }

    template<physical_dimension dim_other>
    dimensional_scalar& operator += (dimensional_scalar<fp, dim_other> other)
    {
        static_assert(dim == dim_other, "cannot add values with different dimensions");
        value = value + other.value;
        return *this;
    }

    template<physical_dimension dim_other>
    auto operator - (dimensional_scalar<fp, dim_other> other) const
    {
        static_assert(dim == dim_other, "cannot subtract values with different dimensions");
        return dimensional_scalar<fp, dim>({ value - other.value, internal_constructor_of_dimensional_scalar_from_fp() });
    }

    auto operator - () const
    {
        return dimensional_scalar<fp, dim_>({ -value, internal_constructor_of_dimensional_scalar_from_fp() });
    }

    template<physical_dimension dim_other>
    dimensional_scalar& operator -= (dimensional_scalar<fp, dim_other> other)
    {
        static_assert(dim == dim_other, "cannot add values with different dimensions");
        value = value - other.value;
        return *this;
    }

    template<physical_dimension dim_other>
    auto operator * (dimensional_scalar<fp, dim_other> other) const
    {
        return dimensional_scalar<fp, dim * dim_other>({ value * other.value, internal_constructor_of_dimensional_scalar_from_fp() });
    }

    template<physical_dimension dim_other>
    dimensional_scalar& operator *= (dimensional_scalar<fp, dim_other> other)
    {
        static_assert(dim_other == simple_scalar::dim, "cannot multiply value unless multiplier has all dimensionals equal to zero");
        value = value * other.value;
        return *this;
    }

    template<physical_dimension dim_other>
    auto operator / (dimensional_scalar<fp, dim_other> other) const
    {
        return dimensional_scalar<fp, dim / dim_other>({ value / other.value, internal_constructor_of_dimensional_scalar_from_fp() });
    }

    template<physical_dimension dim_other>
    dimensional_scalar& operator /= (dimensional_scalar<fp, dim_other> other)
    {
        static_assert(dim_other == simple_scalar::dim, "cannot divide value unless multiplier has all dimensionals equal to zero");
        value = value / other.value;
        return *this;
    }

    template<physical_dimension dim_other>
    bool operator == (const dimensional_scalar<fp, dim_other> other) const
    {
        static_assert(dim_ == dim_other, "cannot compare values with different dimensions");
        if constexpr (dim == dim_other)
            return value == other.value;
        else
            return false;
    }

    template<physical_dimension dim_other>
    bool operator != (const dimensional_scalar<fp, dim_other> other) const
    {
        static_assert(dim == dim_other, "cannot compare values with different dimensions");
        if constexpr (dim != dim_other)
            return true;
        else
            return value != other.value;
    }

    template<physical_dimension dim_other>
    bool operator < (const dimensional_scalar<fp, dim_other> other) const
    {
        static_assert(dim == dim_other, "cannot compare values with different dimensions");
        return value < other.value;
    }

    template<physical_dimension dim_other>
    bool operator > (const dimensional_scalar<fp, dim_other> other) const
    {
        static_assert(dim == dim_other, "cannot compare values with different dimensions");
        return value > other.value;
    }

    template<physical_dimension dim_other>
    bool operator <= (const dimensional_scalar<fp, dim_other> other) const
    {
        static_assert(dim == dim_other, "cannot compare values with different dimensions");
        return value <= other.value;
    }

    template<physical_dimension dim_other>
    bool operator >= (const dimensional_scalar<fp, dim_other> other) const
    {
        static_assert(dim == dim_other, "cannot compare values with different dimensions");
        return value >= other.value;
    }

    // for comparision inside the functions
    static constexpr dimensional_scalar<fp, dim> zero() {
        static_assert(dim.Theta == 0, "for temperature zero value is, in general, not that well-defined (zeros of different systems do not coincide");
        return dimensional_scalar<fp, dim>(0.f, internal_constructor_of_dimensional_scalar_from_fp());
    }

public:
    // means for adding OPTIONAL support of sixit rw library
    struct rw_alias
    {
        using helper = dimensional_scalar_rw_alias_helper<fp>;
        using type = typename helper::alias_type;
        using fp_value_type = typename helper::value_type;
        static constexpr bool is_specified = !std::is_same_v<fp_value_type, void>;
        static_assert(std::is_same_v<fp_value_type, fp> || !is_specified);
        static_assert(!std::is_same_v<type, void> || !is_specified);

        static type value2alias(const dimensional_scalar<fp, dim>& val)
        {
            if constexpr (is_specified)
                return helper::value2alias(val.value);
            else
                return;
        }

        static auto alias2value(type val)
        {
            if constexpr (is_specified)
                return dimensional_scalar<fp, dim>{helper::alias2value(val), internal_constructor_of_dimensional_scalar_from_fp()};
            else
                return;
        }
    };
    struct no_rw_alias
    {
        using type = void;
    };
    using rw_type_alias = std::conditional_t<!std::is_same_v<typename dimensional_scalar_rw_alias_helper<fp>::value_type, void>, rw_alias, no_rw_alias>;
};

template <typename fp>
dimensional_scalar<fp, simple_scalar::dim> create_dimensionless_scalar(fp val)
{
    return { val, internal_constructor_of_dimensional_scalar_from_fp() };
}

template <typename fp>
fp extract_dim_less_scalar(dimensional_scalar<fp, simple_scalar::dim> val)
{
    return val.value;
}


template<class fp, class UnitT>
dimensional_scalar<fp, UnitT::dim> for_import_only_make_dimensional_scalar(fp value)
{
    // static_assert(UnitT::dim != simple_scalar::dim);
    return dimensional_scalar<fp, UnitT::dim>({value * fp(UnitT::to_base), internal_constructor_of_dimensional_scalar_from_fp()});
}

template<class UnitT, class ScalarT>
typename ScalarT::fp for_export_only_extract_fp_from_dimensional_scalar(ScalarT value)
{
    static_assert(UnitT::dim == ScalarT::dim, "cannot extract values of a different dimension");
    return value.value / decltype(value.value)(UnitT::to_base);
}

} // namespace sixit::units

namespace sixit::cpual
{

template<class fp>
class cpu_vec4
{
    fp arr[4];
};

template<>
class cpu_vec4<float>
{
    // TODO: low_level::vec4 arr;
};

} // namespace sixit::cpual

#endif //sixit_core_units_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Serhii Iliukhin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
