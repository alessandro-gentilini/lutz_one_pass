/***************************************************************************
 *  lutzOnePass.cpp - Implements the Lutz One Pass algorithm               *
 * ----------------------------------------------------------------------- *
 *  copyright (C) 2017 by Josh Cardenzana                                  *
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
 * @file lutzOnePass.cpp
 * @brief Implementaion of the Lutz One Pass algorithm
 * @author Josh Cardenzana
 */

#include "lutzOnePass.hpp"

/************************************************************//**
 * Default constructor
 ****************************************************************/
lutzOnePass::lutzOnePass() :
    m_image(nullptr),
    m_xpix(0),
    m_ypix(0),
    m_threshold(0.0),
    m_npixelmin(0)
{}


/************************************************************//**
 * Primary constructor from a vector
 *
 * @param[in] image         1D vector containing image data
 * @param[in] xpixels       Number of pixels in x
 * @param[in] ypixels       Number of pixels in y
 ****************************************************************/
lutzOnePass::lutzOnePass(double* image,
                   int xpixels, int ypixels) :
    m_image(image),
    m_xpix(xpixels),
    m_ypix(ypixels),
    m_threshold(0.0),
    m_npixelmin(0)
{}


/************************************************************//**
 * Destructor
 ****************************************************************/
lutzOnePass::~lutzOnePass()
{
}


/************************************************************//**
 * Run the analysis
 ****************************************************************/
void lutzOnePass::run()
{
    // Reset all of the data structures
    init_members();
    int co(0), pstop(0);
    char prev_marker;
    
    // Loop through each row of the image
    for (int yindx=0; yindx < m_ypix; yindx++) {
        
        // Reset PS and CS
        m_PS = COMPLETE;
        m_CS = NONOBJECT;
        
        int xindx;
        for (xindx=0; xindx < m_xpix; xindx++) {
            
            // Store the information for this bin
            lutzObject::pixData pixel (xindx, yindx);
            pixel.m_value = GetPixValue(xindx, yindx);
            
            // Get the value of the marker in the previous row and
            // reset the marker value so we can fill it from this row
            prev_marker = m_MARKER[xindx];
            m_MARKER[xindx] = 0;
            
            // Check if this bin is an image pixel
            bool is_image_pixel = AssessPixel(xindx, yindx);
            
            // If this is an image pixel, do some assessment on the current
            // object status
            if (is_image_pixel) {
                
                // Check whether or not we're currently analyzing a pixel
                if (m_CS == NONOBJECT) {
                    // Previous pixel is not an image pixel
                    
                    // Start a new segment
                    StartSegment(xindx, co, pstop);
                }
                
                // Process previous marker based
                if (prev_marker) ProcessNewMarker(prev_marker, xindx, co, pstop);
                
                // Update INFO
                m_INFO[co].push_back(pixel);
                
            } // end if(is_image_pixel)
            
            // If this is not an image pixel then handle the values from the
            // previous line
            else {
                
                if (prev_marker) {
                    ProcessNewMarker(prev_marker, xindx, co, pstop);
                }
                
                if (m_CS == OBJECT) {
                    EndSegment(xindx, co, pstop);
                }
            }
        }
        
        // Get the value of the marker in the previous row and
        // reset the marker value so we can fill it from this row
        prev_marker = m_MARKER.back();
        m_MARKER.back() = 0;
        
        if (prev_marker) {
            ProcessNewMarker(prev_marker, xindx, co, pstop);
        }
        
        if (m_CS == OBJECT) {
            EndSegment(xindx, co, pstop);
        }
    }
    
    StoreClearance();
}


/************************************************************//**
 * Start a new segment
 *
 * @param[in] xindx         x-bin position along current row
 * @param[in] co            Current object ID
 * @param[in] pstop         Current postion in PSSTACK
 ****************************************************************/
void lutzOnePass::StartSegment(int& xindx, int& co, int& pstop)
{
    // We are traversing an object, so set CS to reflect that
    m_CS = OBJECT;
    
    // Check if we are traversing an object
    if (m_PS == OBJECT) {
        
        // Check if we've already started ths object on this row
        if (m_START[co] == -1) {
            // This is the first pixel of this object
            m_MARKER[xindx] = 'S';
            m_START[co] = xindx;
        } else {
            // This is NOT the first pixel of this object, but
            // it is the first pixel of a new segment of this
            // object.
            m_MARKER[xindx] = 's';
        }
        
    }
    
    // Otherwise the current pixel is not joined to an object,
    // therefore this is the start of a new object!
    else {
        ModOBSTACK(PUSH, co, pstop, xindx);
        m_MARKER[xindx] = 'S';
    }
}


/************************************************************//**
 * End a segment
 *
 * @param[in] xindx         x-bin position along current row
 * @param[in] co            Current object ID
 * @param[in] pstop         Current postion in PSSTACK
 ****************************************************************/
void lutzOnePass::EndSegment(int& xindx, int& co, int& pstop)
{
    m_CS = NONOBJECT;
    if (m_PS != COMPLETE) {
        // End of the segment, but there may be more object pixels to come
        m_MARKER[xindx] = 'f';
        m_END[co] = xindx;
    } else {
        // This ends all segments associated with the current object
        ModOBSTACK(POP, co, pstop, xindx);
        m_MARKER[xindx] = 'F';
    }
}


/************************************************************//**
 * Process marker from previous row
 *
 * @param[in] prev_marker   Value of MARKER at this position on previous row
 * @param[in] xindx         x-bin position along current row
 * @param[in] co            Current object ID
 * @param[in] pstop         Current postion in PSSTACK
 ****************************************************************/
void lutzOnePass::ProcessNewMarker(char& prev_marker, int& xindx, int& co, int& pstop)
{
    // Handle the new marker
    if (prev_marker == 'S') {
        // Start of object on preceding scan
        
        // Push PS onto PSSTACK
        ModPSSTACK(PUSH, pstop);
        
        if (m_CS == NONOBJECT) {
            // The S marker is the first encounter with this object
            // Modify the PSSTACK at this point to be complete
            m_PSSTACK[pstop++] = COMPLETE;
            
            // Fill INFO from STORE
            m_INFO[++co] = m_STORE[xindx];
            m_START[co] = -1;
            
            // Empty the STORE so that we dont duplicate entries when we
            // refill it from INFO
            m_STORE[xindx].clear();
            
        } else {
            // we are currently analyzing a segment that we need to now associate with
            // an object from the previous line
            for (int i=0; i<m_STORE[xindx].size(); i++) {
                m_INFO[co].push_back(m_STORE[xindx][i]);
            }
            // Reset the store at this position
            m_STORE[xindx].clear();
        }
        m_PS = OBJECT;
        
    } else if (prev_marker == 's') {
        // starting a secondary (or tertiary, etc...) segment of an object from the previous row
        if ((m_CS == OBJECT) && (m_PS == COMPLETE)) {
            // need to join the current object to the preceding object
            pstop--;
            int k = m_START[co];
            
            if (m_START[--co] == -1) {
                m_START[co] = k;
            } else {
                m_MARKER[k] = 's';
            }
        }
        m_PS = OBJECT;
    } else if (prev_marker == 'f') {
        // Done with this object on the previous row, but more to come
        m_PS = INCOMPLETE;
    } else if (prev_marker == 'F') {
        // End of the object on the previous row
        
        m_PS = m_PSSTACK[--pstop];
        if ((m_CS == NONOBJECT) && (m_PS == COMPLETE)) {
            // No more of current object to come on this row
            
            if (m_START[co] == -1) {
                // We didnt find any of this object on this row,
                // so no more of object to come EVER!
                WriteObject( m_INFO[co] );
                m_INFO[co].clear();
                
            } else {
                // There may still be more of this object on next row
                m_MARKER[m_END[co]] = 'F';
                m_STORE[m_START[co]] = m_INFO[co];
                
            }
            
            co--;
            m_PS = m_PSSTACK[--pstop];
        }
    }
}


/************************************************************//**
 * Store all of the unfinished objects
 ****************************************************************/
void lutzOnePass::StoreClearance()
{
    // Loop through all of the objects still in STORE and save them
    // to the list of objects
    for (int i=0; i<m_STORE.size(); i++) {
        // If the STORE isn't empty, then save it as a new object
        WriteObject( m_STORE[i] );
        m_STORE[i].clear();
    }
}


/************************************************************//**
 * @brief
 ****************************************************************/
void lutzOnePass::WriteObject(Object& obj)
{
    if (!obj.empty() && (obj.size() >= m_npixelmin)) {
        // Create a SmartObject from the supplied object and append it to
        // the final list of objects
        m_Objects.push_back(lutzObject());
        m_Objects.back().append(obj);
    }
}


/************************************************************//**
 * Return value of given pixel
 *
 * @param[in] xbin          x bin of pixel
 * @param[in] ybin          y bin of pixel
 * @return Value of requested pixel
 ****************************************************************/
double lutzOnePass::GetPixValue(int xbin, int ybin)
{
    int bin = m_xpix * ybin + xbin;
    return m_image[bin];
}


/************************************************************//**
 * Assess whether or not this pixel is an image pixel
 *
 * @param[in] xbin          x bin of pixel
 * @param[in] ybin          y bin of pixel
 * @return Whether or not this bin is significant
 ****************************************************************/
bool lutzOnePass::AssessPixel(int xbin, int ybin)
{
    if (GetPixValue(xbin,ybin) > m_threshold) {
        return true;
    } else {
        return false;
    }
}


/************************************************************//**
 * Clear internal data structures and re-allocate memory
 ****************************************************************/
void lutzOnePass::init_members()
{
    m_Objects.clear();
    m_pixData.clear();
    m_MARKER.clear();
    m_PSSTACK.clear();
    m_START.clear();
    m_END.clear();
    m_INFO.clear();
    m_STORE.clear();
    
    m_MARKER = std::vector<char>(m_xpix + 1, 0);
    m_PSSTACK = std::vector<LUTZSTATUS>(m_xpix , COMPLETE);
    m_START = std::vector<int>(m_xpix , -1);
    m_END = std::vector<int>(m_xpix , -1);
    m_INFO = std::vector<Object>(m_xpix , Object(0));
    m_STORE = std::vector<Object>(m_xpix, Object(0));
}


/************************************************************//**
 * Manage the objects related to OBSTACK
 *
 * @param[in] status        Specifies whether to POP or PUSH onto the stack
 * @param[in] co            Current index of OBSTACK
 * @param[in] pstop         Current index of PSSTACK
 * @param[in] xbin          Current bin of the row
 ****************************************************************/
void lutzOnePass::ModOBSTACK(lutzOnePass::LUTZ_STACK_ACTION status,
                          int& co, int& pstop, int xbin)
{
    if (status == PUSH) {
        ModPSSTACK(PUSH, pstop);
        co++;
        m_START[co] = xbin;
        m_INFO[co].clear();
    } else if (status == POP) {
        ModPSSTACK(POP, pstop);
        for (int i=0; i<m_INFO[co].size(); i++) {
            m_STORE[ m_START[co] ].push_back(m_INFO[co][i]);
        }
        m_INFO[co].clear();
        m_START[co] = -1;
        m_END[co] = -1;
        co--;
    }
}


/************************************************************//**
 * Manage PSSTACK
 *
 * @param[in] status        Specifies whether to POP or PUSH onto the stack
 * @param[in] pstop         Index corresponding to current entry
 ****************************************************************/
void lutzOnePass::ModPSSTACK(lutzOnePass::LUTZ_STACK_ACTION status, int& pstop)
{
    if (status == PUSH) {
        m_PSSTACK[pstop] = m_PS;
        m_PS = COMPLETE;
        pstop++;
    } else if (status == POP) {
        m_PSSTACK[pstop] = COMPLETE;
        m_PS = m_PSSTACK[--pstop];
    }
}
