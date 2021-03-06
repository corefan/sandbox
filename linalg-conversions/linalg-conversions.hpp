#ifndef linalg_conversions_hpp
#define linalg_conversions_hpp

#include "math-common.hpp"

using namespace avl;

//////////////////////////////////////////////////////////////
// Eigen to OpenCV Conversions for Vectors and NxM Matrices //
//////////////////////////////////////////////////////////////

#ifdef HAVE_OPENCV

#include <opencv2/core/core.hpp>

template <typename scalar_t>
inline cv::Vec<scalar_t, 2> eigen_to_cv(const Eigen::Matrix<scalar_t, 2, 1> & v)
{
	return cv::Vec<scalar_t, 2>(v(0), v(1));
}

template <typename scalar_t>
inline cv::Vec<scalar_t, 3>eigen_to_cv(const Eigen::Matrix<scalar_t, 3, 1> & v)
{
	return cv::Vec<scalar_t, 3>(v(0), v(1), v(2));
}

template <typename scalar_t>
inline cv::Vec<scalar_t, 4> eigen_to_cv(const Eigen::Matrix<scalar_t, 4, 1> & v)
{
	return cv::Vec<scalar_t, 4>(v(0), v(1), v(2), v(3));
}

inline cv::Point2f eigen_to_cv(const Eigen::Vector2f & v)
{
	return cv::Point2f(v(0), v(1));
}

inline cv::Point3f eigen_to_cv(const Eigen::Vector3f & v)
{
	return cv::Point3f(v(0), v(1), v(2));
}

template <typename scalar_t, int H, int W>
inline cv::Mat_<scalar_t> eigen_to_cv(const Eigen::Matrix<scalar_t, H, W> & ep)
{
	cv::Mat cvMat(H, W, CV_32F);
	for (int r = 0; r < H; ++r)
	{
		for (int c = 0; c < W; ++c)
		{
			cvMat.at<scalar_t>(r, c) = ep(r, c);
		}
	}
	return cvMat;
}

//////////////////////////////////////////////////////////////
// OpenCV to Eigen Conversions for Vectors and NxM Matrices //
//////////////////////////////////////////////////////////////

template <typename scalar_t>
inline Eigen::Matrix<scalar_t, 2, 1> cv_to_eigen(const cv::Vec<scalar_t, 2> & v)
{
	return Eigen::Matrix<scalar_t, 2, 1>(v[0], v[1]);
}

template <typename scalar_t>
inline Eigen::Matrix<scalar_t, 3, 1> cv_to_eigen(const cv::Vec<scalar_t, 3> & v)
{
	return Eigen::Matrix<scalar_t, 3, 1>(v[0], v[1], v[2]);
}

template <typename scalar_t>
inline Eigen::Matrix<scalar_t, 4, 1> cv_to_eigen(const cv::Vec<scalar_t, 4> & v)
{
	return Eigen::Matrix<scalar_t, 4, 1>(v[0], v[1], v[2], v[3]);
}

inline Eigen::Vector2f cv_to_eigen(const cv::Point2f & v)
{
	return Eigen::Vector2f(v.x, v.y);
}

inline Eigen::Vector3f cv_to_eigen(const cv::Point3f & v)
{
	return Eigen::Vector3f(v.x, v.y, v.z);
}

template <typename scalar_t, int H, int W>
inline Eigen::Matrix<scalar_t, H, W> cv_to_eigen(const cv::Mat_<scalar_t> & cvMat)
{
	Eigen::Matrix<scalar_t, H, W> eigenMat;
	for (int r = 0; r < H; ++r)
	{
		for (int c = 0; c < W; ++c)
		{
			eigenMat(r, c) = cvMat.at<scalar_t>(r, c);
		}
	}
	return eigenMat;
}

#endif

////////////////////////////////////////////////////////////////
// Linalg.h to Eigen Conversions for Vectors and NxM Matrices //
////////////////////////////////////////////////////////////////

#include <Eigen/Core>

template<typename scalar_t>
struct AffineTransform
{
    Eigen::Matrix<scalar_t, 3, 1> position;
    Eigen::Quaternion<scalar_t> orientation;

    AffineTransform() : position(0, 0, 0), orientation(0, 0, 0, 1) { };

    template<class scalar_t> AffineTransform(const Eigen::Quaternion<scalar_t> & o, const Eigen::Matrix<scalar_t, 3, 1> & p) : orientation(o), position(p) { }
    template<class scalar_t> AffineTransform(const AffineTransform<scalar_t> & other) : orientation(other.orientation), position(other.position) { }

    template<class scalar_t> AffineTransform<scalar_t> operator * (const AffineTransform<scalar_t> & other) const
    {
        return AffineTransform<scalar_t>(Eigen::Quaternion<scalar_t>(orientation * other.orientation), Eigen::Matrix<scalar_t 3, 1>(position + orientation._transformVector(other.position)));
    }

    AffineTransform<scalar_t> inverse()
    {
        return AffineTransform<scalar_t>(Eigen::Quaternion<scalar_t>(orientation.conjugate()), Eigen::Matrix<scalar_t, 3, 1>(orientation.conjugate()._transformVector(-position)));
    }
};

template <typename scalar_t>
inline linalg::vec<scalar_t, 2> to_linalg(const Eigen::Matrix<scalar_t, 2, 1> & v)
{
    return{ v(0), v(1) };
}

template <typename scalar_t>
inline linalg::vec<scalar_t, 3> to_linalg(const Eigen::Matrix<scalar_t, 3, 1> & v)
{
    return{ v(0), v(1), v(2) };
}

template <typename scalar_t>
inline linalg::vec<scalar_t, 4> to_linalg(const Eigen::Matrix<scalar_t, 4, 1> & v)
{
    return{ v(0), v(1), v(2), v(3) };
}

template <typename scalar_t>
inline linalg::vec<scalar_t, 4> to_linalg(const Eigen::Quaternion<scalar_t> & q)
{
    return{ q(0), q(1), q(2), q(3) };
}

template <typename scalar_t>
inline linalg::mat<scalar_t, 3, 3> to_linalg(const Eigen::Matrix<scalar_t, 3, 3> & ep)
{
    linalg::mat<scalar_t, 3, 3> matrix;
    std::memcpy(&matrix[0].x, ep.data(), sizeof(scalar_t) * 9);
    return matrix;
}

template <typename scalar_t>
inline linalg::mat<scalar_t, 4, 4> to_linalg(const Eigen::Matrix<scalar_t, 4, 4> & ep)
{
    linalg::mat<scalar_t, 4, 4> matrix;
    std::memcpy(&matrix[0].x, ep.data(), sizeof(scalar_t) * 16);
    return matrix;
}

// To Eigen

template <typename scalar_t>
inline Eigen::Matrix<scalar_t, 2, 1> to_eigen(const linalg::vec<scalar_t, 2> & v)
{
    return{ v.x, v.y };
}

template <typename scalar_t>
inline Eigen::Matrix<scalar_t, 3, 1> to_eigen(const linalg::vec<scalar_t, 3> & v)
{
    return{ v.x, v.y, v.z};
}

template <typename scalar_t>
inline const Eigen::Matrix<scalar_t, 4, 1> to_eigen(const linalg::vec<scalar_t, 4> & v)
{
    return{ v.x, v.y, v.z, v.w };
}

template <typename scalar_t>
inline Eigen::Matrix<scalar_t, 3, 3> to_eigen(const linalg::mat<scalar_t, 3, 3> & m)
{
    Eigen::Matrix<scalar_t, 3, 3> matrix;
    std::memcpy(matrix.data(), &m[0].x, sizeof(scalar_t) * 9);
    return matrix;
}

template <typename scalar_t>
inline Eigen::Matrix<scalar_t, 4, 4> to_eigen(const linalg::mat<scalar_t, 4, 4> & m)
{
    Eigen::Matrix<scalar_t, 4, 4> matrix;
    std::memcpy(matrix.data(), &m[0].x, sizeof(scalar_t) * 16);
    return matrix;
}

///////////////////////////////////////////////////////////
// GLM to Eigen Conversions for Vectors and NxM Matrices //
///////////////////////////////////////////////////////////

// to do

#endif // linalg_conversions_hpp
