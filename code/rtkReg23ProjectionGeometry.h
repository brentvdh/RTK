/*=========================================================================
 *
 *  Copyright RTK Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef __rtkReg23ProjectionGeometry_h
#define __rtkReg23ProjectionGeometry_h


//RTK
#include "rtkThreeDCircularProjectionGeometry.h"
#include "rtkWin32Header.h"

//ITK
#include <itkPoint.h>
#include <itkMatrix.h>

namespace rtk
{

/** \class Reg23ProjectionGeometry
 * \brief {A simple utility class which makes the REG23 projection geometry
 * compatible with RTK's geometry.}
 *
 * REG23 geometry (open-radART, plastimatch) is very simplicistic: a perspective
 * projection is defined by a source position (S) in world coordinate system
 * (WCS), a detector position (R) in WCS and the detector orientation in WCS
 * by the detector row vector (r) and the detector column vector (c).
 *
 * In order to make RTK aware of this geometry, the parameters mentioned above
 * have to be brought into RTK's native geometry format. This is achieved by
 * deriving from RTK's circular 3D projection geometry class and adding a new
 * dedicated method for adding a REG23 projection. Internally, this method
 * converts the S,R,r,c set into Euler angles and offsets as required by RTK's
 * native projection geometry definition format.
 *
 * @warning {The detector offset and direction is fully modeled by the resultant
 * RTK geometry entry. This means that the projection stack input into back-
 * projection filters or forward-projectors is expected to have zero-origin and
 * identity-direction!}
 *
 * @see rtk::ThreeDCircularProjectionGeometry
 *
 * \test {TestReg23ProjectionGeometry.cxx}
 *
 * @author phil steininger
 * @version 1.0
 */
class RTK_EXPORT Reg23ProjectionGeometry :
    public rtk::ThreeDCircularProjectionGeometry
{
public:
  /** General typedefs **/
  typedef Reg23ProjectionGeometry               Self;
  typedef rtk::ThreeDCircularProjectionGeometry Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;
  typedef itk::Point<double, 3>                 PointType;
  typedef itk::Matrix<double, 3, 3>             Matrix3x3Type;

  /** Method for creation through the object factory. */
  itkNewMacro(Self)

  /**
   * @brief Add a REG23-based geometry set to the RTK projections list.
   * @param sourcePosition absolute position of the point source S in WCS
   * @param detectorPosition absolute position of the detector origin R in WCS
   * @param detectorRowVector absolute direction vector indicating the
   * orientation of the detector's rows r (sometimes referred to as v1)
   * @param detectorColumnVector absolute direction vector indicating the
   * orientation of the detector's columns c (sometimes referred to as v2)
   * @return TRUE if the projection could be added to the RTK projections list
   */
  bool AddReg23Projection(const PointType &sourcePosition,
                          const PointType &detectorPosition,
                          const VectorType &detectorRowVector,
                          const VectorType &detectorColumnVector);

protected:
  /** Standard constructor. **/
  Reg23ProjectionGeometry();
  /** Destructor. **/
  virtual ~Reg23ProjectionGeometry();

  /** Verify that the specified Euler angles in ZXY result in a rotation matrix
   * which corresponds to the specified detector orientation. Rationale for this
   * utility method is that in some situations numerical instabilities (e.g. if
   * gantry=+90deg,in-plane=-90deg or vice versa, "invalid" angles may be
   * computed using the standard ITK Euler transform) may occur.
   * @param outOfPlaneAngleRAD out-of-plane angle of the detector in radians
   * @param gantryAngleRAD gantry angle of the detector in radians
   * @param inPlaneAngleRAD in-plane angle of the detector in radians
   * @param referenceMatrix reference matrix which reflects detector orientation
   * in WCS
   * @return TRUE if the angles correspond the implicitly specified final
   * rotation matrix; if FALSE is returned, the angles should be fixed
   * (@see FixAngles())
   * @warning {Internally, the matrix check is performed with a tolerance level
   * of 1e-6!}
   */
  bool VerifyAngles(const double outOfPlaneAngleRAD, const double gantryAngleRAD,
                    const double inPlaneAngleRAD,
                    const Matrix3x3Type &referenceMatrix) const;

  /** Try to fix Euler angles, which were found incorrect, to match the specified
   * reference matrix.
   * @param [out] outOfPlaneAngleRAD out-of-plane angle of the detector in radians;
   * if this method returns TRUE, this angle can be safely considered
   * @param [out] gantryAngleRAD gantry angle of the detector in radians;
   * if this method returns TRUE, this angle can be safely considered
   * @param [out] inPlaneAngleRAD in-plane angle of the detector in radians;
   * if this method returns TRUE, this angle can be safely considered
   * @param referenceMatrix reference matrix which reflects detector orientation
   * in WCS
   * @return TRUE if the angles were fixed and can be safely considered;
   * if FALSE is returned, the method could not find angles which generate the
   * desired matrix with respect to ZXY Euler order and the internal tolerance
   * level
   * @see VerifyAngles()
   * @warning {Internally, the matrix check is performed with a tolerance level
   * of 1e-6!}
   */
  bool FixAngles(double &outOfPlaneAngleRAD, double &gantryAngleRAD,
                 double &inPlaneAngleRAD,
                 const Matrix3x3Type &referenceMatrix) const;

private:
  /** Purposely not implemented. **/
  Reg23ProjectionGeometry(const Self&);
  /** Purposely not implemented. **/
  void operator=(const Self&);

};

}

#endif // __rtkReg23ProjectionGeometry_h
