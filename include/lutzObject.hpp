/***************************************************************************
 *  lutzObject.hpp - Defines an object extracted using lutz one pass algo  *
 * ----------------------------------------------------------------------- *
 *  copyright (C) 2018 by Josh Cardenzana                                  *
 * ----------------------------------------------------------------------- *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/
/**
 * @file lutzObject.hpp
 * @brief Defines an object extracted using lutz one pass algo
 * @author Josh Cardenzana
 */

#ifndef LUTZOBJECT_HPP
#define LUTZOBJECT_HPP

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class lutzObject {
public:
    
    /* ============================================================= */
    
    // Create a class for holding the pixel information
    class pixData {
    public:
        pixData(int x=0, int y=0, double val=0.0) :
            m_xbin(x), m_ybin(y), m_value(val), m_scale(1.0)
        {}
        void reset();
        
        /****** Operators ******/
        operator double() const;
        bool operator<(const pixData& lhs) const;
        bool operator>(const pixData& lhs) const;
        
        /******  Methods  ******/
        // Swap this pixel's info with another pixel's
        void swap(pixData& other, bool value_only=false);
        
        /****** Variables ******/
        int    m_xbin;
        int    m_ybin;
        double m_value;
        double m_scale;
    };
    
    /* ============================================================= */
    
    lutzObject();
    lutzObject(std::vector<pixData>& pixels);
    lutzObject(const lutzObject& other);
    virtual ~lutzObject();
    
    /****** Operators ******/
    
    pixData&       operator[] (const int& index);
    const pixData& operator[] (const int& index) const;
    
    /******  Methods  ******/
    
    void   append(const pixData& pixel);
    void   append(std::vector<pixData>& pixels);
    void   clear();
    void   centroid(double& xcenter, double& ycenter,
                    bool weight_bins=true);
    bool   contains(const pixData& pixel) const;
    bool   overlaps(const lutzObject& other) const;
    void   remove(const int& index);
    void   sort();
    
    size_t size() const;
    int    GetXMin() const;
    int    GetXMax() const;
    int    GetYMin() const;
    int    GetYMax() const;
    double GetMinimum() const;
    double GetMaximum() const;
    double Sum() const;
    
protected:
    
    /******  Methods  ******/
    
    void   copy_members(const lutzObject& other);
    
    /****** Variables ******/
    
    int    m_xmin;                    //!< minimum pixel position in x
    int    m_xmax;                    //!< maximum pixel position in x
    int    m_ymin;                    //!< minimum pixel position in y
    int    m_ymax;                    //!< maximum pixel position in y
    double m_value_max;               //!< maximum pixel value
    double m_value_min;               //!< minimum pixel value
    double m_value_sum;               //!< Sum of all pixel values
    std::vector<pixData> m_pixInfo;   //!< Container for pixel information
    
};

/***************************************************************//**
 * @brief Overload operator double() for a pixel to return the value
 *
 * @param[in] index         Pixel index to be returned
 * @return pixel data associated with a given index
 *******************************************************************/
inline
lutzObject::pixData& lutzObject::operator[] (const int& index)
{
    return m_pixInfo[index];
}


/***************************************************************//**
 * @brief Overload operator double() for a pixel to return the value
 *
 * @param[in] index         Pixel index to be returned
 * @return pixel data associated with a given index
 *******************************************************************/
inline
const lutzObject::pixData& lutzObject::operator[] (const int& index) const
{
    return m_pixInfo[index];
}


/***************************************************************//**
 * @brief Return number of pixels in this object
 *
 * @return Number of pixels in this object
 *******************************************************************/
inline
size_t lutzObject::size() const
{
    return m_pixInfo.size();
}


/***************************************************************//**
 * @brief Return smallest pixel position in x
 *
 * @return Smallest pixel position in x
 *******************************************************************/
inline
int lutzObject::GetXMin() const
{
    return m_xmin;
}


/***************************************************************//**
 * @brief Return largest pixel position in x
 *
 * @return Largest pixel position in x
 *******************************************************************/
inline
int lutzObject::GetXMax() const
{
    return m_xmax;
}


/***************************************************************//**
 * @brief Return smallest pixel position in y
 *
 * @return Smallest pixel position in y
 *******************************************************************/
inline
int lutzObject::GetYMin() const
{
    return m_ymin;
}


/***************************************************************//**
 * @brief Return largest pixel position in x
 *
 * @return Largest pixel position in x
 *******************************************************************/
inline
int lutzObject::GetYMax() const
{
    return m_ymax;
}


/***************************************************************//**
 * @brief Return smallest pixel value
 *
 * @return Smallest pixel value
 *******************************************************************/
inline
double lutzObject::GetMinimum() const
{
    return m_value_min;
}


/***************************************************************//**
 * @brief Return largest pixel value
 *
 * @return Largest pixel value
 *******************************************************************/
inline
double lutzObject::GetMaximum() const
{
    return m_value_max;
}

/***************************************************************//**
 * @brief Sort pixels from lowest value to largest value
 *******************************************************************/
inline
void lutzObject::sort()
{
    std::sort(m_pixInfo.begin(), m_pixInfo.end());
}

/*****************************************************************
 * @brief Return sum of all pixel values
 *
 * @return Sum of all pixel values
 *******************************************************************/
inline
double lutzObject::Sum() const
{
    return m_value_sum;
}


/*==========================================================================
 =                                                                         =
 =                      lutzObject::pixData methods                        =
 =                                                                         =
 ==========================================================================*/

/***************************************************************//**
 * @brief Overload operator double() for a pixel to return the value
 *******************************************************************/
inline
lutzObject::pixData::operator double() const
{
    return m_value;
}

/***************************************************************//**
 * @brief Overload operator> for a pixel
 *
 * @param[in] lhs       Left hand side pixel for the comparison
 * @return Whether this pixel value is greater than lhs
 *******************************************************************/
inline
bool lutzObject::pixData::operator>(const pixData& lhs) const
{
    return (m_value > lhs.m_value);
}

/***************************************************************//**
 * @brief Overload operator< for a pixel
 *
 * @param[in] lhs       Left hand side pixel for the comparison
 * @return Whether this pixel value is less than lhs
 *******************************************************************/
inline
bool lutzObject::pixData::operator<(const pixData& lhs) const
{
    return (m_value < lhs.m_value);
}

#endif /* LUTZOBJECT_HPP */
