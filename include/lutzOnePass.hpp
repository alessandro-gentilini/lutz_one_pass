
/***************************************************************************
 *  lutzOnePass.hpp - Implements the Lutz One Pass algorithm               *
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
 * @file lutzOnePass.hpp
 * @brief Implementaion of the Lutz One Pass algorithm
 * @author Josh Cardenzana
 */

#ifndef LUTZONEPASS_HPP
#define LUTZONEPASS_HPP

#include <iostream>
#include <vector>
#include <string>

#include "lutzObject.hpp"

class lutzOnePass {
public:
    // Constructors
    lutzOnePass();
    lutzOnePass(double* image,
                int xpixels, int ypixels);
    // Destructor
    virtual ~lutzOnePass();
    
    /******  Methods  ******/
    
    // Set image information
    virtual void SetImage(double* image);
    virtual void SetXpixels(int xpixels);
    virtual void SetYpixels(int ypixels);
    virtual void SetThreshold(double threshold);
    virtual void SetNPixelMin(int npixelmin);
    
    // Run the actual analysis
    virtual void run();
    
    // Get the current value of a given bin
    virtual double GetPixValue(int xbin, int ybin);
    
    // Assess whether or not this pixel is an image pixel
    virtual bool AssessPixel(int xbin, int ybin);
    
    // Return the list of pixel information for a given object
    lutzObject GetObject(const int& obj_id);
    std::vector<lutzObject> GetObjects(void);
    int NumObjects(void);
    
    // enums
    enum LUTZSTATUS {COMPLETE, INCOMPLETE, OBJECT, NONOBJECT};
    enum LUTZ_STACK_ACTION {PUSH, POP};
    
    // Define a vector of pixel data as an Object
    typedef std::vector<lutzObject::pixData> Object;
    
protected:
    
    /******  Methods  ******/
    virtual void init_members(void);
    
    // Methods for managing OBSTACK and PSSTACK
    void ModOBSTACK(LUTZ_STACK_ACTION status,
                    int& co, int& pstop, int xbin);
    void ModPSSTACK(LUTZ_STACK_ACTION status, int& pstop);
    
    // Methods for taking specific actions based on where we are and
    // current values of parameters
    virtual void StartSegment(int& xindx, int& co, int& pstop);
    virtual void EndSegment(int& xindx, int& co, int& pstop);
    virtual void ProcessNewMarker(char& prev_marker,
                                  int& xindx, int& co, int& pstop);
    virtual void StoreClearance(void);
    virtual void WriteObject(Object& obj);
    
    /****** Variables ******/
    int     m_xpix;                 //!< Number of bins in x
    int     m_ypix;                 //!< Number of bins in y
    int     m_npixelmin;            //!< Minimum number of pixels required to store an object
    double* m_image;                //!< Image values (1D)
    double  m_threshold;            //!< Threshold above which a pixel is
                                    //!< considered an image pixel
    
    std::vector<Object> m_pixData;  //!< Pixel data for all objects
    
    // Some book keeping parameters
    std::vector<char>        m_MARKER;
    std::vector<lutzObject> m_Objects;   //!< List of completed objects
    std::vector<Object>      m_STORE;     //!< Stores cached objects
    std::vector<LUTZSTATUS>  m_PSSTACK;   //!< Pixel status from previous line
    
    // The following store information relevant to OBSTACK
    std::vector<int>         m_START;     //!< Identifies first pixel associated with object 'co'
    std::vector<int>         m_END;       //!< Last pixel associated with object 'co'
    std::vector<Object>      m_INFO;      //!< Caches objects currently being processed
    LUTZSTATUS m_PS;                      //!< Status relevant to pixels on previous row
    LUTZSTATUS m_CS;                      //!< Status of current pixel
    
private:
    
};

/************************************************************//**
 * Set the image values
 *
 * @param[in] image         Vector containing actual data counts
 ****************************************************************/
inline void lutzOnePass::SetImage(double* image)
{
    m_image = image;
}


/************************************************************//**
 * Set the number of pixels in x
 *
 * @param[in] xpixels       Number of pixels in x
 ****************************************************************/
inline void lutzOnePass::SetXpixels(int xpixels)
{
    m_xpix = xpixels;
}


/************************************************************//**
 * Set the number of pixels in y
 *
 * @param[in] ypixels       Number of pixels in y
 ****************************************************************/
inline void lutzOnePass::SetYpixels(int ypixels)
{
    m_ypix = ypixels;
}


/************************************************************//**
 * Set the threshold for considering a pixel as an "image" pixel
 *
 * @param[in] threshold     Threshold value
 ****************************************************************/
inline void lutzOnePass::SetThreshold(double threshold)
{
    m_threshold = threshold;
}


/************************************************************//**
 * @brief Set the minimum number of pixels required to save an object
 *
 * @param[in] npixelmin     Minimum number of pixels
 ****************************************************************/
inline void lutzOnePass::SetNPixelMin(int npixelmin)
{
    m_npixelmin = npixelmin;
}

/************************************************************//**
 * Return the pixel data associated with a given object
 *
 * @param[in] obj_id        Object ID number
 * @return Vector of pixel information for the requested object
 ****************************************************************/
inline lutzObject lutzOnePass::GetObject(const int& obj_id)
{
    return m_Objects[obj_id];
}


/************************************************************//**
 * @brief Return all of the found objects
 *
 * @return Vector of object information
 ****************************************************************/
inline std::vector<lutzObject> lutzOnePass::GetObjects(void)
{
    return m_Objects;
}


/************************************************************//**
 * Return the number of found objects
 *
 * @return Number of objects found in the image
 ****************************************************************/
inline int lutzOnePass::NumObjects()
{
    return m_Objects.size();
}

#endif /* LUTZONEPASS_HPP */
