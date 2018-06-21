/***************************************************************************
 *  lutzObject - Container class for a group of pixels                     *
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
 * @file lutzObject.cpp
 * @brief Implements the lutzObject class
 * @author Josh Cardenzana
 */

#include <stdio.h>
#include "lutzObject.hpp"


/***************************************************************//**
 * @brief Default constructor for lutzObject
 *******************************************************************/
lutzObject::lutzObject()
{
    clear();
}


/***************************************************************//**
 * @brief construct an object from a vector of pixels
 *
 * @param[in] pixels        Vector containing a list of pixel information
 *******************************************************************/
lutzObject::lutzObject(std::vector<pixData>& pixels)
{
    clear();
    append(pixels);
}


/***************************************************************//**
 * @brief Copy constructor from another object
 *
 * @param[in] other     Another lutzObject to be copied
 *******************************************************************/
lutzObject::lutzObject(const lutzObject& other)
{
    copy_members(other);
}


/***************************************************************//**
 * @brief Deconstructor
 *******************************************************************/
lutzObject::~lutzObject()
{}


/***************************************************************//**
 * @brief Append a single pixel to this object
 *
 * @param[in] pixel         Pixel to be appended
 *******************************************************************/
void lutzObject::append(const pixData& pixel)
{
    // Make sure this pixel doesnt already exist in this object
    if (contains(pixel)) return;
    
    // Adjust the stored minimum/maximum values
    if (pixel.m_xbin < m_xmin) m_xmin = pixel.m_xbin;
    if (pixel.m_xbin > m_xmax) m_xmax = pixel.m_xbin;
    if (pixel.m_ybin < m_ymin) m_ymin = pixel.m_ybin;
    if (pixel.m_ybin > m_ymax) m_ymax = pixel.m_ybin;
    if (pixel.m_value < m_value_min) m_value_min = pixel.m_value;
    if (pixel.m_value > m_value_max) m_value_max = pixel.m_value;
    m_pixInfo.push_back(pixel);
    m_value_sum += pixel.m_value;
}

/***************************************************************//**
 * @brief Append a list of pixels to this object
 *
 * @param[in] pixels        Vector of pixels to be appended
 *******************************************************************/
void lutzObject::append(std::vector<pixData>& pixels)
{
    // Append all of the pixels in the container
    for (int i=0; i<pixels.size(); i++) {
        append(pixels[i]);
    }
}

/***************************************************************//**
 * @brief Remove a single pixel from the container
 *
 * @param[in] index         Index of pixel to be removed
 *******************************************************************/
void lutzObject::remove(const int& index)
{
    m_value_sum -= m_pixInfo[index];
    m_pixInfo.erase(m_pixInfo.begin() + index);
}


/***************************************************************//**
 * @brief Clear the contents of this object
 *******************************************************************/
void lutzObject::clear()
{
    m_pixInfo.clear();
    m_xmin = 1e7;
    m_xmax = -1e7;
    m_ymin = 1e7;
    m_ymax = -1e7;
    m_value_min = 1.0e30;
    m_value_max = -1.0e30;
    m_value_sum = 0.0;
}


/********************************************************************//**
 * @brief Compute the central x,y position of the object
 *
 * @param[out] xcenter          Center x-position
 * @param[out] ycenter          Center y-position
 * @param[in] weight_bins       Specifies whether to compute a weighted position
 *                              based on the values in each pixel
 *
 * Computes the central x,y position of the object. The computation can
 * also weight the central position based on the values in each bin. If
 * the weights are ignored, the position is computed via:
 * \f[
 *      xcenter = \frac{1}{Nbins} \sum_{n=1}^{nbins} x_{n}
 *      ycenter = \frac{1}{Nbins} \sum_{n=1}^{nbins} y_{n}
 * \f]
 * Alternatively, if weights are applied (which is the default) the
 * computation is:
 * \f[
 *      xcenter = \frac{1}{\sum_{n=1}^{nbins}w_{n}} \sum_{n=1}^{nbins} w_{n} x_{n}
 *      ycenter = \frac{1}{Nbins} \sum_{n=1}^{nbins} y_{n}
 * \f]
 ************************************************************************/
void lutzObject::centroid(double& xcenter, double& ycenter,
                          bool weight_bins)
{
    // Initialize the sum of weights and reset the center positions
    double weight_sum(0.0);
    xcenter = 0.0;
    ycenter = 0.0;
    
    // Loop through each of the pixels and compute the centroid
    for (int p=0; p<size(); p++) {
        double weight = m_pixInfo[p].m_scale;
        
        // Multiply the weight by the bin value
        if (weight_bins) {
            weight *= m_pixInfo[p].m_value;
        }
        
        xcenter += weight * m_pixInfo[p].m_xbin;
        ycenter += weight * m_pixInfo[p].m_ybin;
        weight_sum += weight;
    }
    
    // Normalize by weights if weight is above 0
    if (weight_sum > 0.0) {
        xcenter /= weight_sum;
        ycenter /= weight_sum;
    }
    // Otherwise recompute the centroid without weights
    else {
        centroid(xcenter, ycenter, false);
    }
    
    return;
}


/***************************************************************//**
 * @brief Check whether this object has a pixel at a given position
 *
 * @param[in] pixel         Pixel whose position will be checked
 * @return Whether or not this object has a pixel at pixel's position
 *******************************************************************/
bool lutzObject::contains(const pixData& pixel) const
{
    for (int p=0; p < m_pixInfo.size(); p++) {
        if ((pixel.m_xbin == m_pixInfo[p].m_xbin) &&
            (pixel.m_ybin == m_pixInfo[p].m_ybin)){
            // This pixel currently exists in this object
            return true;
        }
    }
    // No pixels with coinciding position were found
    return false;
}


/***************************************************************//**
 * @brief Check whether this object overlaps with another
 *
 * @param[in] other         Pixel whose position will be checked
 * @return Whether or not this object has a pixel at pixel's position
 *******************************************************************/
bool lutzObject::overlaps(const lutzObject& other) const
{
    // Iterate through all of the pixels
    std::vector<pixData>::const_iterator iter;
    for (iter = other.m_pixInfo.begin(); iter!=other.m_pixInfo.end(); ++iter) {
        if (contains(*iter)) return true;
    }
    return false;
}


/*==========================================================================
 =                                                                         =
 =                           Protected methods                             =
 =                                                                         =
 ==========================================================================*/

/***************************************************************//**
 * @brief Copy the values from another
 *
 * @param[in] other         Another lutzObject to be copied to here
 *******************************************************************/
void lutzObject::copy_members(const lutzObject& other)
{
    clear();
    
    m_xmin = other.m_xmin;
    m_xmax = other.m_xmax;
    m_ymin = other.m_ymin;
    m_ymax = other.m_ymax;
    m_value_min = other.m_value_min;
    m_value_max = other.m_value_max;
    m_value_sum = other.m_value_sum;
    m_pixInfo = other.m_pixInfo;
}


/*==========================================================================
 =                                                                         =
 =                      lutzObject::pixData methods                        =
 =                                                                         =
 ==========================================================================*/

/***************************************************************//**
 * @brief Clear all of the information associated with this pixel
 *******************************************************************/
void lutzObject::pixData::reset()
{
    m_xbin = 0;
    m_ybin = 0;
    m_value = 0.0;
    m_scale = 0.0;
}


/***************************************************************//**
 * @brief Swap all values of this pixel with those of another.
 *
 * @param[in] other         Pixel to swap info with
 *
 * Note that this will swap all of the information with the other pixel
 * including pixel position, unless 'value_only=true' is set.
 *******************************************************************/
void lutzObject::pixData::swap(pixData& other, bool value_only) {
    
    // If we also want to swap the pixel positions
    if (!value_only) {
        other.m_xbin += m_xbin;
        other.m_ybin += m_ybin;
        m_xbin        = other.m_xbin - m_xbin;
        m_ybin        = other.m_ybin - m_ybin;
        other.m_xbin -= m_xbin;
        other.m_ybin -= m_ybin;
    }
        
    other.m_value += m_value;
    other.m_scale += m_scale;
    m_value        = other.m_value - m_value;
    m_scale        = other.m_scale - m_scale;
    other.m_value -= m_value;
    other.m_scale -= m_scale;
}
